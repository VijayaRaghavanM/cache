#include "Common.h"
#include "CacheBlock.h"

// Represents a single block of data
CacheBlock::CacheBlock() {
    valid = false;
    dirty = false;
    tag = -1;
    data = 0;
    time = 0;
    access_count = 0;
}
// method to write values to an existing block
void CacheBlock::write_data(int memory, int data) {
    this->data = data;
    this->valid = true;
    this->tag = memory;
    this->time = ++counter;
    this->dirty = true;
}

string CacheBlock::contents() {
    stringstream s;
    s   << setw(10) <<  to_string(this->time)      
        << setw(10) <<  to_string(this->tag)    
        << setw(10) <<  to_string(this->data)   
        << setw(10) <<  to_string(this->valid)  
        << setw(10) <<  to_string(this->dirty)
        << endl;
    return s.str();
}