#include <iostream>

#include <QBDI.h>

int main(int argc, char** argv) {
    for(const QBDI::MemoryMap& m :  QBDI::getCurrentProcessMaps()) {
        std::cout << m.name << " (" << m.permission << ") ";
        QBDI::Range<QBDI::rword> range(m.start, m.end);
        range.display(std::cout);
        std::cout << std::endl;
    }
}
