#include "Common.h"
#include "Memory.h"

// A Simple simulation of the Main memory as an array of data
Memory::Memory(int size) { 
    values.resize(size, DEFAULT_VALUE);
}
  
// Getter and setter for the memory block
int Memory::get(int address) { 
    return values[address % values.size()]; 
}
void Memory::set(int address, int data) {
    values[address % values.size()] = data;
}

// Print the whole memory to console
void Memory::print() {
    for (int i = 0; i < values.size(); i++) {
        cout  << "|"       << std::setw(5) 
            << i         << std::setw(5)
            << "|"       << std::setw(5) 
            << values[i] << std::setw(5) 
            << "|"       << endl;
    }
}
