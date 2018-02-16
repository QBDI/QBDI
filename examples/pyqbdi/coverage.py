#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import pyqbdi
import struct
import atexit


# write coverage (in drcov format)
def writeCoverage(stats):
    with open("a.cov", "w") as fp:
        # write header
        fp.write("DRCOV VERSION: 2\n")
        fp.write("DRCOV FLAVOR: drcov\n")
        modules = stats["modules"]
        fp.write("Module Table: version 2, count %u\n" % len(modules))
        fp.write("Columns: id, base, end, entry, checksum, timestamp, path\n")
        # write modules
        fmt = "%2u, 0x%x, 0x%x, 0x0000000000000000, "
        fmt += "0x00000000, 0x00000000, %s\n"
        for mid, m in enumerate(modules):
            fp.write(fmt % (mid, m.range[0], m.range[1], m.name))
        addrs = stats["addrs"]
        sizes = stats["sizes"]
        # find address offsets from their module base
        starts = {}
        for addr in addrs:
            for mid, m in enumerate(modules):
                if addr >= m.range[0] and addr < m.range[1]:
                    starts[addr] = (mid, addr - m.range[0])
                    break
        # write basicblocks
        fp.write("BB Table: %u bbs\n" % len(starts))
        for addr, (mid, start) in starts.items():
            # uint32_t start; uint16_t size; uint16_t id;
            bb = struct.pack("<IHH", start, sizes[addr], mid)
            fp.write(bb)


def vmCB(vm, evt, gpr, fpr, data):
    if evt.event & pyqbdi.BASIC_BLOCK_ENTRY:
        addr = evt.basicBlockStart
        size = evt.basicBlockEnd - addr
        data["addrs"].add(addr)
        data["sizes"][addr] = size


def pyqbdipreload_on_run(vm, start, stop):
    # list modules
    modules = [m for m in pyqbdi.getCurrentProcessMaps()
               if m.permission & pyqbdi.PF_EXEC and m.name]
    # setup statistics
    stats = dict(modules=modules, addrs=set(), sizes=dict())
    # add callback on Basic blocks
    vm.addVMEventCB(pyqbdi.BASIC_BLOCK_ENTRY | pyqbdi.BASIC_BLOCK_EXIT,
                    vmCB, stats)
    # write coverage on exit
    atexit.register(writeCoverage, stats)
    # run program
    vm.run(start, stop)
