#include "Common.h"
#include "Log.h"

// Log class to record and display statistics
Log::Log() {
    read_count = 0;
    read_hit_count = 0;
    write_count = 0;
    write_hit_count = 0;
  }

  // Readable representation of all the data
  string Log::to_string() {
    stringstream s;
    float hit_ratio = (read_hit_count + write_hit_count) /
          (float)(read_count + write_count);
    s << std::setw(13) << "READ COUNT:"   << std::setw(10) << read_count                      << endl
      << std::setw(13) << "READ HIT:"     << std::setw(10) << read_hit_count                  << endl
      << std::setw(13) << "READ MISS:"    << std::setw(10) << (read_count - read_hit_count)   << endl
      << std::setw(13) << "WRITE COUNT:"  << std::setw(10) << write_count                     << endl
      << std::setw(13) << "WRITE HIT:"    << std::setw(10) << write_hit_count                 << endl
      << std::setw(13) << "WRITE MISS:"   << std::setw(10) << (write_count - write_hit_count) << endl
      << std::setw(13) << "ACCESS COUNT:" << std::setw(10) << (read_count + write_count)      << endl
      << std::setw(13) << "HIT RATIO:"    << std::setw(10) << hit_ratio                       << endl;

    return s.str();
  }

  // Print to console
  void Log::print() { 
    cout << to_string(); 
  }

  // Print to output file
  void Log::print_to_file() {
    ofstream output_file(OUTPUT_FILE, fstream::app);
    output_file << to_string();
    output_file.close();
  }