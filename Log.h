#pragma once

#include "Common.h"

// Log class to record and display statistics
class Log {
public:
  int read_count;
  int read_hit_count;
  int write_count;
  int write_hit_count;
  
//   // Initialize all values
  Log();

  // Readable representation of all the data
  string to_string();

  // Print to console
  void print();

  // Print to output file
  void print_to_file();

};
