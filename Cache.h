#pragma once

#include "Memory.h"
#include "Log.h"
#include "Set.h"

class Cache {
public:
  int cache_size;
  int line_size;
  int cache_associativity;
  int tolerance;
  int set_count;
    // The index separating High Priority and Low Priority Groups
  int partition_index;
  Memory *memory;
  Log *log;
  int block_count;
  vector<Set> sets;
  // Constructor
  Cache(int cache_size, int line_size, int cache_associativity, int tolerance);
  // Utility functions to get the start and end index of sets and groups
  int lp_begin();
  int lp_end();
  int hp_begin();
  int hp_end();
  int begin();
  int end();
  // method to print a block
  void print_block(int set_index, int block_index);
  // method to print a set
  void print_set(int set_index);
  // method to print the whole cache
  void print();
  // method to write cache contents to output file
  void print_to_file();
  // Print the contents of the memory block attached to the cache
  void print_memory();
  // Utility functions for debugging
  void print_line();

  void print_header();
  // Print the cache details to the console
  void details();
  // Push the block from LP group to HP group replacing the LRU block
  void increase_priority(int set_index, int block_index);

  void decrease_priority(int set_index, int block_index);
  void read(int memory_address);
  // Push the blocks in the HP group which weren't accessed for sufficiently
  // long
  void calibrate();
  // Set the timestamp to current value of the next value of counter
  void update_timestamp(int set_index, int block_index);
  // Pick a target block to write data
  CacheBlock *getWriteTarget(int set_index);
  // Write data to the cache
  void write(int memory_address, int data);
};
