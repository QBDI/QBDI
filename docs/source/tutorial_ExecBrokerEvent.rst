Execution transfert event
=========================

Introduction
------------

One limitation of QBDI is to share the heap and some shared library with the instrumented code.
With this design, the user may use any shared library and doesn't need to statically link all his
dependencies with his code. However, some method must not be instrumented in QBDI:

- The heap allocator method (malloc, free, ...).
- Any no reentrant method shared between the target code and QBDI itself.
- Any no reentrant method shared between the target code and user callbacks.

When the target code calls one of these methods, QBDI restores the native execution.
The return address is changed in order to catch the return and to continue the instrumentation of the
code. Two events allow the user to detect this mechanism:

- ``EXEC_TRANSFER_CALL``: called before restoring native execution for the method.
- ``EXEC_TRANSFER_RETURN``: called after the execution of the method.

These two events can be used to retrieve the method and its parameters before the call and its return value after.
``EXEC_TRANSFER_CALL`` can also be used to emulate a method call.


Get native call symbols
-----------------------

When QBDI needs to restore the native execution, the user may retrieve the name of the calling method
based on the current address. The associated symbol can be found with ``dladdr`` (on Linux and OSX) or ``SymFromAddr`` (on Windows).

We recommend forcing the linker to resolve all symbols before running the VM. This can be achieved with:

- ``LD_BIND_NOW=1`` on Linux
- ``DYLD_BIND_AT_LAUNCH=1`` on OSX


With dladdr
+++++++++++

``dladdr`` may not find the symbol associated with an address if it's not an exported symbol.
If many symbols are associated, only one will be returning.

.. code:: c

    static VMAction transfertcbk(VMInstanceRef vm, const VMState *vmState, GPRState *gprState, FPRState *fprState, void *data) {
        Dl_info info = {0};
        dladdr((void*)gprState->rip, &info);

        if (info.dli_sname != NULL) {
            printf("Call %s (addr: 0x%" PRIRWORD ")\n", info.dli_sname, gprState->rip);
        } else {
            printf("Call addr: 0x%" PRIRWORD "\n", gprState->rip);
        }
        return QBDI_CONTINUE;
    }

    qbdi_addVMEventCB(vm, QBDI_EXEC_TRANSFER_CALL, transfertcbk, NULL);

.. code:: python

    import ctypes
    import ctypes.util

    class Dl_info(ctypes.Structure):
        _fields_ = [('dli_fname', ctypes.c_char_p),
                    ('dli_fbase', ctypes.c_void_p),
                    ('dli_sname', ctypes.c_char_p),
                    ('dli_saddr', ctypes.c_void_p)]

    libdl_path = ctypes.util.find_library('dl')
    assert libdl_path != None
    libdl = ctypes.cdll.LoadLibrary(libdl_path)
    libdl.dladdr.argtypes = (ctypes.c_void_p, ctypes.POINTER(Dl_info))

    def dladdr(addr):

        res = Dl_info()
        result = libdl.dladdr(ctypes.cast(addr, ctypes.c_void_p), ctypes.byref(res))

        return res.dli_sname

    def transfertcbk(vm, vmState, gpr, fpr, data):

        print("Call {} (addr: 0x{:x})".format(
            dladdr(gpr.rip),
            gpr.rip))

        return pyqbdi.CONTINUE

    vm.addVMEventCB(pyqbdi.EXEC_TRANSFER_CALL, transfertcbk, None)

With lief
+++++++++

`Lief <https://lief.quarkslab.com/>`_ is a C, C++ and python library
that aims to parse ELF, PE and MachO file formats. With this library,
we can extract all the symbols associated with an address, included the no-exported one.
This solution can resolve more address, but could be slower than ``dladdr``.

For ELF binary, the followed code will print for each ``EXEC_TRANSFER_CALL``
event, the symbols associated with the target address. For PE library, you may
need to parse the PDB file of the library to get the symbol associated with the
target address.

.. code:: cpp

    #include <LIEF/LIEF.hpp>

    class Module {
        public:
            std::string path;
            QBDI::Range<QBDI::rword> range;

            Module(const QBDI::MemoryMap& m) : path(m.name), range(m.range) {}

            void append(const QBDI::MemoryMap& m) {
                if (m.range.start() < range.start()) {
                    range.setStart(m.range.start());
                }
                if (m.range.end() > range.end()) {
                    range.setEnd(m.range.end());
                }
            }
    };

    class AddrResolver {
        private:
            std::vector<Module> modules;
            std::vector<std::string> loaded_path;
            std::map<QBDI::rword, std::vector<std::string>> resolv_cache;

            void cacheModules();
            const Module* getModule(QBDI::rword addr);
            void loadModule(const Module& m);

        public:
            AddrResolver() {
                cacheModules();
            }

            const std::vector<std::string>& resolve(QBDI::rword addr);
    };

    void AddrResolver::cacheModules() {
        modules.clear();

        for (const auto& map : QBDI::getCurrentProcessMaps(true)) {
            auto r = std::find_if(std::begin(modules), std::end(modules),
                    [&](const Module& m){return m.path == map.name;});
            if (r != std::end(modules)) {
                r->append(map);
            } else if (map.name.find("/") != std::string::npos) {
                modules.emplace_back(map);
            }
        }
    }

    const Module* AddrResolver::getModule(QBDI::rword addr) {
        const auto r = std::find_if(std::begin(modules), std::end(modules),
                [&](const Module& m){return m.range.contains(addr);});
        if (r != std::end(modules)) {
            return &*r;
        }
        cacheModules();
        const auto r2 = std::find_if(std::begin(modules), std::end(modules),
                [&](const Module& m){return m.range.contains(addr);});
        if (r2 != std::end(modules)) {
            return &*r2;
        } else {
            return nullptr;
        }
    }

    void AddrResolver::loadModule(const Module& m) {
        std::cout << "Load Module " << m.path << std::endl;
        const auto r = std::find_if(std::begin(loaded_path), std::end(loaded_path),
                [&](const std::string& path){return path == m.path;});
        if (r != std::end(loaded_path)) {
            return;
        }
        std::unique_ptr<LIEF::ELF::Binary> externlib = LIEF::ELF::Parser::parse(m.path);
        if (not externlib) {
            return;
        }
        for (const auto& s: externlib->symbols()) {
            QBDI::rword addr = s.value() + m.range.start();
            auto it = resolv_cache.find(addr);
            if (it != resolv_cache.end()) {
                std::string symname = s.demangled_name();
                const auto it_str = std::find_if(std::begin(it->second), std::end(it->second),
                        [&](const std::string& symbol){return symbol == symname;});
                if (it_str == std::end(it->second)) {
                    it->second.emplace_back(symname);
                }
            } else {
                resolv_cache[addr] = {s.demangled_name()};
            }
        }

        loaded_path.emplace_back(m.path);
    }

    const std::vector<std::string>& AddrResolver::resolve(QBDI::rword addr) {
        const auto it = resolv_cache.find(addr);
        if (it != resolv_cache.end()) {
            return it->second;
        }
        std::cout << std::setbase(16) << "Fail to found 0x" << addr << std::endl;
        const Module* m = getModule(addr);
        if (m != nullptr) {
            loadModule(*m);
            const auto it2 = resolv_cache.find(addr);
            if (it2 != resolv_cache.end()) {
                return it2->second;
            }
        }
        resolv_cache[addr] = {};
        return resolv_cache[addr];
    }

    QBDI::VMAction transfertCBK(QBDI::VMInstanceRef vm, const QBDI::VMState* vmState, QBDI::GPRState* gprState, QBDI::FPRState* fprState, void* data) {
        const std::vector<std::string>& r = static_cast<AddrResolver*>(data)->resolve(gprState->rip);

        if (r.empty()) {
            std::cout << std::setbase(16) << "Call addr: 0x" << gprState->rip << std::endl;
        } else {
            std::cout << "Call ";
            for (const auto& s: r) {
                std::cout << s << " ";
            }
            std::cout << std::setbase(16) << "(addr: 0x" << gprState->rip << ")" << std::endl;
        }
        return QBDI::CONTINUE;
    }

    AddrResolver data;
    vm->addVMEventCB(QBDI::EXEC_TRANSFER_CALL, transfertCBK, &data);



.. code:: python

    import lief
    import pyqbdi

    class Module:
        def __init__(self, module):
            self.name = module.name
            self.range = pyqbdi.Range(module.range.start, module.range.end)

        def append(self, module):
            assert module.name == self.name
            if module.range.start < self.range.start:
                self.range.start = module.range.start
            if self.range.end < module.range.end:
                self.range.end = module.range.end

    class AddrResolver:

        def __init__(self):
            self.lib_cache = []
            self.resolv_cache = {}
            self.map_cache = self.get_exec_maps()

        def get_exec_maps(self):
            maps = {}
            for m in pyqbdi.getCurrentProcessMaps(True):
                if m.name in maps:
                    maps[m.name].append(m)
                elif '/' in m.name:
                    maps[m.name] = Module(m)
            return maps

        def get_addr_maps(self, addr):
            for _, m in self.map_cache.items():
                if addr in m.range:
                    return m
            self.map_cache = self.get_exec_maps()
            for _, m in self.map_cache.items():
                if addr in m.range:
                    return m
            return None

        def load_lib(self, maps):
            if maps.name in self.lib_cache:
                return

            # use lief.PE or lief.MACO if not ELF file
            lib = lief.ELF.parse(maps.name)
            if lib is None:
                return

            for s in lib.symbols:
                addr = s.value + maps.range.start
                if addr in self.resolv_cache:
                    if s.name not in self.resolv_cache[addr]:
                        self.resolv_cache[addr].append(s.name)
                else:
                    self.resolv_cache[addr] = [s.name]

            self.lib_cache.append(maps.name)

        def get_names(self, addr):

            if addr in self.resolv_cache:
                return self.resolv_cache[addr]

            maps = self.get_addr_maps(addr)
            if maps == None:
                return []
            self.load_lib(maps)
            if addr in self.resolv_cache:
                return self.resolv_cache[addr]
            self.resolv_cache[addr] = []
            return []

    def transfertcbk(vm, vmState, gpr, fpr, data):

        f_names = data['resolver'].get_names(gpr.rip)
        if f_names != []:
            print("Call {} (addr: 0x{:x})".format(f_names, gpr.rip))
        else:
            print("Call addr: 0x{:x}".format(gpr.rip))

        return pyqbdi.CONTINUE

    ctx = {
        "resolver": AddrResolver(),
    }

    vm.addVMEventCB(pyqbdi.EXEC_TRANSFER_CALL, transfertcbk, ctx)

When combine the python snippet with PyQBDIPreload, the call to the libc library
is displayed.

.. code:: text

    $ python -m pyqbdipreload test.py ls
    Call ['__strrchr_avx2'] (addr: 0x7f2aed2a8330)
    Call ['setlocale', '__GI_setlocale'] (addr: 0x7f2aed17a7f0)
    Call ['bindtextdomain', '__bindtextdomain'] (addr: 0x7f2aed17e000)
    Call ['textdomain', '__textdomain'] (addr: 0x7f2aed1815f0)
    Call ['__cxa_atexit', '__GI___cxa_atexit'] (addr: 0x7f2aed1879b0)
    Call ['getopt_long'] (addr: 0x7f2aed22d3f0)
    Call ['getenv', '__GI_getenv'] (addr: 0x7f2aed186b20)
    Call ['getenv', '__GI_getenv'] (addr: 0x7f2aed186b20)
    Call ['getenv', '__GI_getenv'] (addr: 0x7f2aed186b20)
    Call ['getenv', '__GI_getenv'] (addr: 0x7f2aed186b20)
    Call ['getenv', '__GI_getenv'] (addr: 0x7f2aed186b20)
    Call ['isatty', '__isatty'] (addr: 0x7f2aed239250)
    Call ['ioctl', '__ioctl', '__GI_ioctl', '__GI___ioctl'] (addr: 0x7f2aed23d590)
    Call ['getenv', '__GI_getenv'] (addr: 0x7f2aed186b20)
    Call ['getenv', '__GI_getenv'] (addr: 0x7f2aed186b20)
    Call ['__errno_location', '__GI___errno_location'] (addr: 0x7f2aed16fde0)
    Call ['__libc_malloc', 'malloc', '__GI___libc_malloc', '__malloc'] (addr: 0x7f2aed1d3320)
    Call ['__memcpy_avx_unaligned', '__memmove_avx_unaligned'] (addr: 0x7f2aed2ab4a0)
    Call ['__errno_location', '__GI___errno_location'] (addr: 0x7f2aed16fde0)
    Call ['__libc_malloc', 'malloc', '__GI___libc_malloc', '__malloc'] (addr: 0x7f2aed1d3320)
    Call ['__memcpy_avx_unaligned', '__memmove_avx_unaligned'] (addr: 0x7f2aed2ab4a0)
    Call ['getenv', '__GI_getenv'] (addr: 0x7f2aed186b20)
    ....

