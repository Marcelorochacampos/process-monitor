#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() const { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
  return LinuxParser::Cpu(pid_);
}

// TODO: Return the command that generated this process
string Process::Command() { 
  return LinuxParser::Command(pid_);
}

// TODO: Return this process's memory utilization
string Process::Ram() {
  std::string ram_usage = LinuxParser::Ram(pid_);
  long i_ram_usage = std::stol(ram_usage);
  long ram_usage_mb = i_ram_usage / 1024;
  return std::to_string(ram_usage_mb);
}

// TODO: Return the user (name) that generated this process
string Process::User() { 
  return LinuxParser::User(pid_);
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
  long uptime_in_seconds = LinuxParser::UpTime(pid_);
  return static_cast<long int>(uptime_in_seconds);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& other) const { 
  return pid_ < other.Pid();
}

Process::Process(int pid) : pid_(pid) {}