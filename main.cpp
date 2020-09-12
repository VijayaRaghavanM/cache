#include "Common.h"
#include "Cache.h"

// Counter variable to keep track of time
int counter = 0;

int main() {
  ifstream input_file;
  input_file.open(INPUT_FILE);
  int cache_size;
  int line_size;
  int cache_associativity;
  int tolerance;
  // Read Cache details from input
  input_file >> cache_size >> line_size >> cache_associativity >> tolerance;
  Cache cache(cache_size, line_size, cache_associativity, tolerance);
  if (DEBUG)
    cache.details();
  // Read instructions from the file
  while (input_file.peek() != EOF) {
    int memory, data;
    char command;
    // Read memory address
    input_file >> memory;
    input_file.ignore(100, ',');
    // Read the command
    input_file >> command;
    if (command == 'W') {
      cache.log->write_count++;
      // If the command is to write, then read the data to be written
      input_file.ignore(100, ',');
      input_file >> data;
      if (DEBUG)
        cout << "Write command:" << data << " to " << memory << endl;
      // Perform the write operation
      cache.write(memory, data);
    } else {
      if (DEBUG)
        cout << "Read command:" << memory << endl;
      cache.log->read_count++;
      // Perform the read operation
      cache.read(memory);
    }
    // For every instruction, execute the command and print the state of the
    // cache and memory for debugging
    if (DEBUG) {
      cache.print();
      cache.log->print();
      cin.get();
    }
  }
  input_file.close();
  cout << "Writing output to file" << endl;
  cache.print_to_file();
  cache.log->print_to_file();
}