#include <vector>
#include <string>
#include <iostream>
#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  std::vector<std::string> cpu_values = LinuxParser::CpuUtilization();

  int total_value = 0;
  int idle_value = std::stoi(cpu_values[3]); // maybe use key-value pair
  for(std::string curr_value : cpu_values ) {
 	 total_value += std::stoi(curr_value);
  }

  return static_cast<float>(total_value - idle_value) / total_value;
}