/*
 * This file is part of pyQBDI (python binding for QBDI).
 *
 * Copyright 2017 Quarkslab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "pyqbdi.hpp"

namespace QBDI {
namespace pyQBDI {

// Callback to python managment
template<typename T>
struct TrampData {
    T cbk;
    py::object obj;
    TrampData(T& cbk, py::object& obj) : cbk(cbk), obj(obj) {}
};

template<typename T>
static void removeTrampData(uint32_t n, std::map<uint32_t, std::unique_ptr<TrampData<T>>>& map) {
    auto it = map.find(n);
    if (it != map.end()) {
        map.erase(it);
    }
}

template<typename T>
static py::object addTrampData(uint32_t n, std::map<uint32_t, std::unique_ptr<TrampData<T>>>& map, std::unique_ptr<TrampData<T>> data) {
    if (n == VMError::INVALID_EVENTID) {
        return py::cast(VMError::INVALID_EVENTID);
    }
    map[n] = std::move(data);
    return py::cast(n);
}

// Python Callback
using PyInstCallback = std::function<VMAction(VMInstanceRef, GPRState*, FPRState*, py::object&)>;
using PyVMCallback = std::function<VMAction(VMInstanceRef, const VMState*, GPRState*, FPRState*, py::object&)>;

// Map of python callback <=> QBDI number
static std::map<uint32_t, std::unique_ptr<TrampData<PyInstCallback>>> InstCallbackMap;
static std::map<uint32_t, std::unique_ptr<TrampData<PyVMCallback>>> VMCallbackMap;

static void clearTrampDataMap() {
    InstCallbackMap.clear();
    VMCallbackMap.clear();
}

// QBDI trampoline for python callback
static VMAction trampoline_InstCallback(VMInstanceRef vm, GPRState *gprState, FPRState *fprState, void *data) {
    TrampData<PyInstCallback>* cbk = static_cast<TrampData<PyInstCallback>*>(data);
    VMAction res;
    try {
        res = cbk->cbk(vm, gprState, fprState, cbk->obj);
    } catch (const std::exception& e) {
        std::cerr << "Error during InstCallback : " << e.what() << std::endl;
        exit(1);
    }
    return res;
}

static VMAction trampoline_VMCallback(VMInstanceRef vm, const VMState *vmState, GPRState *gprState, FPRState *fprState, void *data) {
    TrampData<PyVMCallback>* cbk = static_cast<TrampData<PyVMCallback>*>(data);
    VMAction res;
    try {
        res = cbk->cbk(vm, vmState, gprState, fprState, cbk->obj);
    } catch (const std::exception& e) {
        std::cerr << "Error during VMCallback : " << e.what() << std::endl;
        exit(1);
    }
    return res;
}


void init_binding_VM(py::module& m) {

    py::module atexit = py::module::import("atexit");
    atexit.attr("register")(std::function<void()>(clearTrampDataMap));

    py::class_<VM>(m, "VM")
        .def(py::init<const std::string&, const std::vector<std::string>&>(),
                "Construct a new VM for a given CPU with specific attributes",
                "cpu"_a = "", "mattrs"_a = std::vector<std::string>())
        .def("getGPRState", &VM::getGPRState, py::return_value_policy::reference_internal,
                "Obtain the current general purpose register state.")
        .def("getFPRState", &VM::getFPRState, py::return_value_policy::reference_internal,
                "Obtain the current floating point register state.")
        .def("setGPRState", &VM::setGPRState,
                "Set the GPR state.",
                "gprState"_a)
        .def("setFPRState", &VM::setFPRState,
                "Set the FPR state.",
                "fprState"_a)
        .def("addInstrumentedRange", &VM::addInstrumentedRange,
                "Add an address range to the set of instrumented address ranges.",
                "start"_a, "end"_a)
        .def("addInstrumentedModule", &VM::addInstrumentedModule,
                "Add the executable address ranges of a module to the set of instrumented address ranges.",
                "name"_a)
        .def("addInstrumentedModuleFromAddr", &VM::addInstrumentedModuleFromAddr,
                "Add the executable address ranges of a module to the set of instrumented address ranges "
                "using an address belonging to the module.",
                "addr"_a)
        .def("instrumentAllExecutableMaps", &VM::instrumentAllExecutableMaps,
                "Adds all the executable memory maps to the instrumented range set.")
        .def("removeInstrumentedRange", &VM::removeInstrumentedRange,
                "Remove an address range from the set of instrumented address ranges.",
                "start"_a, "end"_a)
        .def("removeInstrumentedModule", &VM::removeInstrumentedModule,
                "Remove the executable address ranges of a module from the set of instrumented address ranges.",
                "name"_a)
        .def("removeInstrumentedModuleFromAddr", &VM::removeInstrumentedModuleFromAddr,
                "Remove the executable address ranges of a module from the set of instrumented address ranges "
                "using an address belonging to the module.",
                "addr"_a)
        .def("removeAllInstrumentedRanges", &VM::removeAllInstrumentedRanges,
                "Remove all instrumented ranges.")
        .def("run", &VM::run,
                "Start the execution by the DBI.",
                "start"_a, "stop"_a)
        .def("call",
                [](VM& vm, rword function, std::vector<rword>& args) {
                    rword retvalue;
                    bool ret = vm.call(&retvalue, function, args);
                    return std::make_tuple(ret, retvalue);
                },
                "Call a function using the DBI (and its current state).",
                "function"_a, "args"_a)
        .def("addMnemonicCB",
                [](VM& vm, const char* mnemonic, InstPosition pos, PyInstCallback& cbk, py::object& obj) {
                    std::unique_ptr<TrampData<PyInstCallback>> data {new TrampData<PyInstCallback>(cbk, obj)};
                    uint32_t n = vm.addMnemonicCB(mnemonic, pos, &trampoline_InstCallback, static_cast<void*>(data.get()));
                    return addTrampData(n, InstCallbackMap, std::move(data));
                },
                "Register a callback event if the instruction matches the mnemonic.",
                "mnemonic"_a, "pos"_a, "cbk"_a, "data"_a)
        .def("addCodeCB",
                [](VM& vm, InstPosition pos, PyInstCallback& cbk, py::object& obj) {
                    std::unique_ptr<TrampData<PyInstCallback>> data {new TrampData<PyInstCallback>(cbk, obj)};
                    uint32_t n = vm.addCodeCB(pos, &trampoline_InstCallback, static_cast<void*>(data.get()));
                    return addTrampData(n, InstCallbackMap, std::move(data));
                },
                "Register a callback event for every instruction executed.",
                "pos"_a, "cbk"_a, "data"_a)
        .def("addCodeAddrCB",
                [](VM& vm, rword address, InstPosition pos, PyInstCallback& cbk, py::object& obj) {
                    std::unique_ptr<TrampData<PyInstCallback>> data {new TrampData<PyInstCallback>(cbk, obj)};
                    uint32_t n = vm.addCodeAddrCB(address, pos, &trampoline_InstCallback, static_cast<void*>(data.get()));
                    return addTrampData(n, InstCallbackMap, std::move(data));
                },
                "Register a callback for when a specific address is executed.",
                "address"_a, "pos"_a, "cbk"_a, "data"_a)
        .def("addCodeRangeCB",
                [](VM& vm, rword start, rword end, InstPosition pos, PyInstCallback& cbk, py::object& obj) {
                    std::unique_ptr<TrampData<PyInstCallback>> data {new TrampData<PyInstCallback>(cbk, obj)};
                    uint32_t n = vm.addCodeRangeCB(start, end, pos, &trampoline_InstCallback, static_cast<void*>(data.get()));
                    return addTrampData(n, InstCallbackMap, std::move(data));
                },
                "Register a callback for when a specific address range is executed.",
                "start"_a, "end"_a, "pos"_a, "cbk"_a, "data"_a)
        .def("addMemAccessCB",
                [](VM& vm, MemoryAccessType type, PyInstCallback& cbk, py::object& obj) {
                    std::unique_ptr<TrampData<PyInstCallback>> data {new TrampData<PyInstCallback>(cbk, obj)};
                    uint32_t n = vm.addMemAccessCB(type, &trampoline_InstCallback, static_cast<void*>(data.get()));
                    return addTrampData(n, InstCallbackMap, std::move(data));
                },
                "Register a callback event for every memory access matching the type bitfield made by the instructions.",
                "type"_a, "cbk"_a, "data"_a)
        .def("addMemAddrCB",
                [](VM& vm, rword address, MemoryAccessType type, PyInstCallback& cbk, py::object& obj) {
                    std::unique_ptr<TrampData<PyInstCallback>> data {new TrampData<PyInstCallback>(cbk, obj)};
                    uint32_t n = vm.addMemAddrCB(address, type, &trampoline_InstCallback, static_cast<void*>(data.get()));
                    return addTrampData(n, InstCallbackMap, std::move(data));
                },
                "Add a virtual callback which is triggered for any memory access at a specific address "
                "matching the access type. Virtual callbacks are called via callback forwarding by a "
                "gate callback triggered on every memory access. This incurs a high performance cost.",
                "address"_a ,"type"_a, "cbk"_a, "data"_a)
        .def("addMemRangeCB",
                [](VM& vm, rword start, rword end, MemoryAccessType type, PyInstCallback& cbk, py::object& obj) {
                    std::unique_ptr<TrampData<PyInstCallback>> data {new TrampData<PyInstCallback>(cbk, obj)};
                    uint32_t n = vm.addMemRangeCB(start, end, type, &trampoline_InstCallback, static_cast<void*>(data.get()));
                    return addTrampData(n, InstCallbackMap, std::move(data));
                },
                "Add a virtual callback which is triggered for any memory access at a specific address range "
                "matching the access type. Virtual callbacks are called via callback forwarding by a "
                "gate callback triggered on every memory access. This incurs a high performance cost.",
                "start"_a, "end"_a ,"type"_a, "cbk"_a, "data"_a)
        .def("addVMEventCB",
                [](VM& vm, VMEvent mask, PyVMCallback& cbk, py::object& obj) {
                    std::unique_ptr<TrampData<PyVMCallback>> data {new TrampData<PyVMCallback>(cbk, obj)};
                    uint32_t n = vm.addVMEventCB(mask, &trampoline_VMCallback, static_cast<void*>(data.get()));
                    return addTrampData(n, VMCallbackMap, std::move(data));
                },
                "Register a callback event for a specific VM event.",
                "mask"_a, "cbk"_a, "data"_a)
        .def("addVMEventCB",
                [](VM& vm, int mask, PyVMCallback& cbk, py::object& obj) {
                    std::unique_ptr<TrampData<PyVMCallback>> data {new TrampData<PyVMCallback>(cbk, obj)};
                    uint32_t n = vm.addVMEventCB(static_cast<VMEvent>(mask), &trampoline_VMCallback, static_cast<void*>(data.get()));
                    return addTrampData(n, VMCallbackMap, std::move(data));
                },
                "Register a callback event for a specific VM event.",
                "mask"_a, "cbk"_a, "data"_a)
        .def("deleteInstrumentation",
                [](VM& vm, uint32_t id) {
                    vm.deleteInstrumentation(id);
                    removeTrampData(id, InstCallbackMap);
                    removeTrampData(id, VMCallbackMap);
                },
                "Remove an instrumentation.",
                "id"_a)
        .def("deleteAllInstrumentations",
                [](VM& vm) {
                    vm.deleteAllInstrumentations();
                    clearTrampDataMap();
                },
                "Remove all the registered instrumentations.")
        .def("getInstAnalysis",
                [](VM& vm, AnalysisType type) {
                    return vm.getInstAnalysis(type);
                },
                "Obtain the analysis of the current instruction. Analysis results are cached in the VM.",
                py::arg_v("type",AnalysisType::ANALYSIS_INSTRUCTION | AnalysisType::ANALYSIS_DISASSEMBLY,
                            "AnalysisType.ANALYSIS_INSTRUCTION|AnalysisType.ANALYSIS_DISASSEMBLY"),
                py::return_value_policy::copy)
        .def("getInstAnalysis",
                [](VM& vm, int type) {
                    return vm.getInstAnalysis(static_cast<AnalysisType>(type));
                },
                "Obtain the analysis of the current instruction. Analysis results are cached in the VM.",
                "type"_a, py::return_value_policy::copy)
        .def("getCachedInstAnalysis",
                [](VM& vm, rword address, AnalysisType type) {
                    return vm.getCachedInstAnalysis(address, type);
                },
                "Obtain the analysis of a cached instruction. Analysis results are cached in the VM.",
                "address"_a,
                py::arg_v("type",AnalysisType::ANALYSIS_INSTRUCTION | AnalysisType::ANALYSIS_DISASSEMBLY,
                            "AnalysisType.ANALYSIS_INSTRUCTION|AnalysisType.ANALYSIS_DISASSEMBLY"),
                py::return_value_policy::copy)
        .def("getCachedInstAnalysis",
                [](VM& vm, rword address, int type) {
                    return vm.getCachedInstAnalysis(address, static_cast<AnalysisType>(type));
                },
                "Obtain the analysis of a cached instruction. Analysis results are cached in the VM.",
                "address"_a, "type"_a, py::return_value_policy::copy)
        .def("recordMemoryAccess", &VM::recordMemoryAccess,
                "Add instrumentation rules to log memory access using inline instrumentation and instruction shadows.",
                "type"_a)
        .def("getInstMemoryAccess", &VM::getInstMemoryAccess,
                "Obtain the memory accesses made by the last executed instruction.",
                py::return_value_policy::copy)
        .def("getBBMemoryAccess", &VM::getBBMemoryAccess,
                "Obtain the memory accesses made by the last executed basic block.",
                py::return_value_policy::copy)
        .def("precacheBasicBlock", &VM::precacheBasicBlock,
                "Pre-cache a known basic block",
                "pc"_a)
        .def("clearCache", &VM::clearCache,
                "Clear a specific address range from the translation cache.",
                "start"_a, "end"_a)
        .def("clearAllCache", &VM::clearAllCache,
                "Clear the entire translation cache.");

}

}}
