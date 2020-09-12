#include "Set.h"

// Initialize the set witha fixed number of blocks and compute the partition
// index
void Set::init(int count) {
    cache_blocks.resize(count);
}
