#pragma once

#include "Common.h"

// A Simple simulation of the Main memory as an array of data
class Memory {
  vector<int> values;

  public:
  // Initialize the memory with a size as argument
  Memory(int size);
  
  // Getter and setter for the memory block
  int get(int address);
  void set(int address, int data);

  // Print the whole memory to console
  void print();
};
