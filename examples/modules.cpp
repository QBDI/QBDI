#include <iostream>
#include "Memory.hpp"

int main(int argc, char** argv) {
    for(const QBDI::MemoryMap& m :  QBDI::getCurrentProcessMaps()) {
        std::cout << m.name << " (" << m.permission << ") ";
        m.range.display(std::cout);
        std::cout << std::endl;
    }
}
