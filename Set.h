#pragma once

#include "Common.h"
#include "CacheBlock.h"


// Set is used to hold a collection of blocks
class Set {
public:
  // List of Blocks
  vector<CacheBlock> cache_blocks;
  // Initialize the set witha fixed number of blocks and compute the partition
  // index
  void init(int count);

};
