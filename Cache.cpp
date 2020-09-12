#include "Common.h"
#include "Cache.h"
#include "Log.h"
#include "Memory.h"
#include "CacheBlock.h"
#include "Set.h"

Cache::Cache(int cache_size, int line_size, int cache_associativity, int tolerance) {
    memory = new Memory(MEMORY_SIZE);
    log = new Log();
    this->block_count = cache_size / line_size;
    this->set_count = block_count / cache_associativity;
    // Initialize sets and blocks
    this->sets.resize(set_count);
    partition_index = cache_associativity / 2;
    for (Set &set : sets) {
      set.init(cache_associativity);
    }
    this->cache_size = cache_size;
    this->line_size = line_size;
    this->cache_associativity = cache_associativity;
    this->tolerance = tolerance;
  }

int Cache::lp_begin() { 
    return this->partition_index; 
}
  
int Cache::lp_end() {
    return this->cache_associativity - 1;
}

int Cache::hp_begin() { 
    return 0; 
}

int Cache::hp_end() {
    return this->partition_index - 1;
}

int Cache::begin() { 
    return hp_begin(); 
}

int Cache::end() { 
    return lp_end(); 
}

void Cache::print_block(int set_index, int block_index) {
    CacheBlock *target = &sets[set_index].cache_blocks[block_index];
    cout << target->contents();
}

void Cache::print_set(int set_index) {
    for (int idx = 0; idx < sets[set_index].cache_blocks.size(); idx++) {
      print_block(set_index, idx);
    }
}

void Cache::print() {
    print_line();
    print_header();
    for (int i = 0; i < sets.size(); i++) {
      cout << "\t\t\t\t\tSET - " << i << endl;
      print_set(i);
    }
    print_line();
    print_memory();
}

void Cache::print_to_file() {
    ofstream output_file(OUTPUT_FILE);
    output_file << "TIME\tTAG\tDATA\tVALID\tDIRTY" << endl;
    for (int set_index = 0; set_index < sets.size(); set_index++) {
      for (int block_index = begin(); block_index <= end(); block_index++) {
        CacheBlock *target = &sets[set_index].cache_blocks[block_index];
        output_file << target->contents();
      }
    }
    output_file << endl;
    output_file.close();
}

void Cache::print_memory() {
    print_line();
    cout << "MEMORY STATUS" << endl;
    this->memory->print();
    cout << endl;
    print_line();
}

void Cache::print_line() {
    cout << "---------------------------------------------" << endl;
}

void Cache::print_header() { 
    cout  << setw(10) << "TIME" 
          << setw(10) << "TAG" 
          << setw(10) << "DATA" 
          << setw(10) << "VALID" 
          << setw(10) << "DIRTY" 
          << endl; 
}

void Cache::details() {
    print_line();
    cout << "CACHE SIZE:"          << cache_size          << endl
         << "CACHE ASSOCIATIVITY:" << cache_associativity << endl
         << "LINE SIZE:"           << line_size           << endl
         << "TOLERANCE:"           << tolerance           << endl
         << "NO OF SETS:"          << set_count           << endl
         << "NO OF BLOCKS:"        << block_count         << endl;   
    print_line();
}

void Cache::increase_priority(int set_index, int block_index) {
    Set *set = &sets[set_index];
    int min_value = INT_MAX, min_index = 0;
    // Pick the LRU block
    for (int i = hp_begin(); i <= hp_end(); i++) {
      CacheBlock *current = &set->cache_blocks[i];
      if (current->time < min_value) {
        min_value = current->time;
        min_index = i;
      }
    }
    CacheBlock *target = &set->cache_blocks[min_index];
    // If the target block is dirty, write the changes to the memory, else
    // discard it
    if (target->dirty) {
      memory->set(target->tag, target->data);
    }
    // Copy the block to target
    set->cache_blocks[min_index] = set->cache_blocks[block_index];
    // Mark the source block invalid
    set->cache_blocks[block_index].valid = false;
}

void Cache::decrease_priority(int set_index, int block_index) {
    Set *set = &sets[set_index];
    // Get LRU Block in LP group
    CacheBlock *target = getWriteTarget(set_index);
    CacheBlock *source = &set->cache_blocks[block_index];
    // Before replacing the target block, write it to memory if it is dirty
    if (target->dirty) {
      target->write_data(source->tag, source->data);
    }
    // copy the source block properties to the target block
    target->dirty = source->dirty;
    target->time = source->time;
    // Decrease priority is not considered an operation, so decrement the
    // counter
    counter--;
    // Mark the source block as invalid
    source->valid = false;
}

void Cache::read(int memory_address) {
    if (DEBUG) {
      print_line();
      cout << "Reading data from " << memory_address << endl;
    }
    int set_index = memory_address % set_count;
    Set *set = &sets[set_index];
    bool found = false;
    // Iterate through all the blocks in the set
    for (int block_index = begin(); block_index <= end(); block_index++) {
        CacheBlock *target = &set->cache_blocks[block_index];
        // Check if the needed block is present
        if (target->valid && target->tag == memory_address) {
            found = true;
            cout << "SET " << set_index << " BLOCK " << block_index << endl;
            // Print the block and update timestamp of last access
            print_header();
            print_block(set_index, block_index);
            update_timestamp(set_index, block_index);
            target->access_count++;
            // If block present in LP, then move it to the HP group
            if ((block_index >= lp_begin() && block_index <= lp_end())) {
                if (DEBUG) {
                    cout << "Increasing priority" << endl;
                }
                increase_priority(set_index, block_index);
            }
        }
    }

    // Push the blocks in the HP group which weren't accessed for sufficiently
    // long
    calibrate();
    if (found) {
      // Mark the read as successful one
      log->read_hit_count++;
    } else {
      // Block not found in cache, need to pull it from the main memory
      if (DEBUG)
        cout << "Cannot find, Importing from memory...." << endl;
      // Get the target block to write data from memory
      CacheBlock *target = getWriteTarget(set_index);
      target->write_data(memory_address, memory->get(memory_address));
      if (DEBUG)
        cout << "Imported, reading now..." << endl;
      // Call the function again, but now the block is present in the set
      read(memory_address);
    }
    if (DEBUG)
      print_line();

}

void Cache::calibrate() {
  for (int set_index = 0; set_index < sets.size(); set_index++) {
    for (int block_index = hp_begin(); block_index <= hp_end(); block_index++) {
      // Iterate through every block in the cache
      CacheBlock *current = &sets[set_index].cache_blocks[block_index];
      // If block hasn't been accessed for too long, then move it from HP to
      // LP
      if (current->valid) {
        if ((counter - current->time) >= tolerance) {
          if (DEBUG)
            cout << "Decreasing priority";
          decrease_priority(set_index, block_index);
        }
      }
    }
  }
}

void Cache::update_timestamp(int set_index, int block_index) {
    sets[set_index].cache_blocks[block_index].time = ++counter;
}

CacheBlock *Cache::getWriteTarget(int set_index) {
    int start = lp_begin();
    int end = lp_end();
    Set *set = &sets[set_index];
    CacheBlock *target = NULL;
    // Search for empty or invalid blocks, if not found then pick the LRU block
    for (int idx = start; idx <= end; idx++) {
        CacheBlock *temp = &set->cache_blocks[idx];
        if (temp->valid) {
            if (target != NULL) {
                if (target->time > temp->time) {
                    target = temp;
                }
            } else {
                target = temp;
            }
        } else {
            target = temp;
            break;
        }
    }
    // If the target block is dirty, write it to main memory
    if (target->valid && target->dirty) {
        memory->set(target->tag, target->data);
    }
    return target;
}

void Cache::write(int memory_address, int data) {
    int set_index = memory_address % set_count;
    if (DEBUG) {
        print_line();
        cout << "Writing " << data << " to " << memory_address
            << "( SET INDEX:" << set_index << ")" << endl;
        print_line();
    }

    Set *set = &sets[set_index];
    bool found = false;
    // before writing data, if blocks corresponding to the memory address exist,
    // then mark them as invalid
    for (int i = begin(); i <= end(); i++) {
      if (set->cache_blocks[i].tag == memory_address) {
        set->cache_blocks[i].valid = false;
        set->cache_blocks[i].dirty = true;
        found = true;
      }
    }
    // If such blocks exist, then mark the write as successful
    if (found) {
      log->write_hit_count++;
    }
    // Get the target block
    CacheBlock *target = getWriteTarget(set_index);
    // Write the data to the target block
    target->write_data(memory_address, data);
}
