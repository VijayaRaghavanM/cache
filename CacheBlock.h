#pragma once

#include "Common.h"

// Represents a single block of data
class CacheBlock {
public:
  bool valid;
  int tag;
  int data;
  int time;
  int access_count;
  bool dirty;

  // Initialize the values
  CacheBlock();
  
  // method to write values to an existing block
  void write_data(int memory, int data);

  string contents();

};

