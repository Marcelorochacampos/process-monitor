#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <boost/filesystem.hpp>

#include "linux_parser.h"

namespace fs = boost::filesystem;

using std::stof;
using std::string;
using std::to_string;
using std::vector;


// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  std::vector<int> pids;
  fs::path directory_path(kProcDirectory.c_str());
  if (!fs::exists(directory_path) || !fs::is_directory(directory_path)) {
  	return pids;
  }
  
  for (const auto& entry : fs::directory_iterator(directory_path)) {
    if (fs::is_directory(entry.path())) {
      std::string directoryName = entry.path().filename().string();
      if (std::all_of(directoryName.begin(), directoryName.end(), ::isdigit)) {
        int pid = std::stoi(directoryName);
        pids.push_back(pid);
      }
    }
  }
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  std::string path = kProcDirectory + kMeminfoFilename;
  std::ifstream stream(path);
  std::string line;
  std::string key;
  std::string value;
  std::string type;
  
  long total_memory = 0;
  long free_memory = 0;
  long buffers = 0;
  long cached = 0;
  float ram_usage_percentage = 0.0;
  
  if (stream) {
  	while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value >> type) {
      	if (key == "MemTotal:") {
          total_memory = std::stol(value);
        } else if ( key == "MemFree:" ) {
          free_memory = std::stol(value);
        } else if ( key == "Buffers:" ) {
          buffers = std::stol(value);
        } else if ( key == "Cached:" ) {
          cached = std::stol(value);
        }    
      }
    }

    int calculated = total_memory - ( free_memory + buffers + cached );
    ram_usage_percentage = (static_cast<float>( calculated ) / total_memory);
  }
  
  return ram_usage_percentage;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::string path = kProcDirectory + kUptimeFilename;
  std::ifstream stream(path.c_str());
  std::string line;
  
  std::string uptime;
  std::string idletime;

  if (stream) {
  	while(std::getline(stream, line)) {
      std::istringstream streamline(line);
      streamline >> uptime >> idletime;
    }
  }
  return std::stol(uptime);
}
 
// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  std::vector<std::string> cpu_jiffies = LinuxParser::CpuUtilization();
  long total_jiffies = 0;
  for(std::string jiffie : cpu_jiffies) {
    total_jiffies += std::stol(jiffie);
  }
  return total_jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  std::string path = kProcDirectory + std::to_string(pid) + kStatFilename;
  std::ifstream stream(path.c_str());
  
  long total_jiffies = 0;
  if (stream) {
  	std::string line;
    std::getline(stream, line);
    std::istringstream streamline(line);
    
    std::string ignore;
    
    for(int count = 1; count <= 13; count++) {
      streamline >> ignore;
    }
    
    std::string utime, stime;
    streamline >> utime >> stime;
    
    long l_utime = std::stol(utime);
    long l_stime = std::stol(stime);
    total_jiffies = l_utime + l_stime;
  }
      
  return total_jiffies;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  std::vector<std::string> cpu_jiffies = LinuxParser::CpuUtilization();
  
  std::string user = cpu_jiffies[0];
  std::string nice = cpu_jiffies[1];
  std::string system = cpu_jiffies[2];
  
  long l_user = std::stol(user);
  long l_nice = std::stol(nice);
  long l_system = std::stol(system);
  
  return l_user + l_nice + l_system;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  std::vector<std::string> cpu_jiffies = LinuxParser::CpuUtilization();
  std::string idle = cpu_jiffies[3];
  long l_idle = std::stol(idle);
  return l_idle;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
	std::string formatted_path = kProcDirectory + kStatFilename;
  	std::ifstream stream(formatted_path.c_str());
  	std::string line;
  	std::vector<std::string> cpu_values;
  
  	// I didnt really know if i should just get the first line or use the while loop to get all lines, maybe later update to show all cpu's usages.
  	// I put a description of each value so i know what each one mean.
  	std::string key; // The Cpu key
  	std::string user; // Time spent running non-kernel code. This includes user-space processes.
  	std::string nice; // Time spent running user-kernel code with a lower priority.
  	std::string system; // Time spent running kernel code. This includes system calls and kernel-related processes.
  	std::string idle; // Time spent idle, doing nothing.
  	std::string iowait; // Time spent waiting for I/O to complete.
  	std::string irq; // Time spent handling hardware interrupts.
  	std::string softirq; // Time spent handling software interrupts.
  	std::string steal; // Time spent in other operating systems when running in a virtualized environment.
  	std::string guest; // Time spent running a virtual CPU for guest operating systems.
  	std::string guest_nice; // Time spent running a low-priority virtual CPU for guest operating systems.
    
  	if (stream.is_open()) {
      while (std::getline(stream, line)) {
        std::istringstream linestream(line);
        while (linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice) {
          if (key == "cpu") {
            cpu_values.emplace_back(user);
            cpu_values.emplace_back(nice);
            cpu_values.emplace_back(system);
            cpu_values.emplace_back(idle);
            cpu_values.emplace_back(iowait);
            cpu_values.emplace_back(irq);
            cpu_values.emplace_back(softirq);
            cpu_values.emplace_back(steal);
            cpu_values.emplace_back(guest);
            cpu_values.emplace_back(guest_nice);
          }
        }
      }
  	}
  return cpu_values;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::string path = kProcDirectory + kStatFilename;
  std::ifstream stream(path.c_str());
  
  int total_processes_amount = 0;
  if (stream) {
  	std::string line;
    std::string key, value;
    
    while(std::getline(stream, line)) {
      std::istringstream streamline(line);
      streamline >> key >> value;
      if (key == "processes") {
      	total_processes_amount = std::stoi(value);
        break;
      }
    }
  }
  return total_processes_amount;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  vector<int> processes = LinuxParser::Pids();
  vector<int> running_processes;
  
  for(int process : processes) {
    std::string formatted_path = kProcDirectory + std::to_string(process) + "/status";
    boost::filesystem::path path(formatted_path.c_str());
    
    if (boost::filesystem::exists(path)) {
      running_processes.emplace_back(process);
    }
  }
  
  return running_processes.size();
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  std::string path = kProcDirectory + std::to_string(pid) + "/cmdline";
  std::ifstream stream(path.c_str());
  
  std::string command;
  
  if (stream) {
    std::getline(stream, command);
  
    for (char& c : command) {
      if (c == '\0') {
        c = ' ';
      }
    }
  }

  return command;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  std::string path = kProcDirectory + std::to_string(pid) + "/status";
  std::ifstream stream(path.c_str());
  
  std::string key;
  std::string value;
  std::string mem_type;
  
  std::string ram_usage;

  if (stream) {
  	std::string line;
    while(std::getline(stream, line)) {
    	std::istringstream streamline(line);
        streamline >> key >> value >> mem_type;
        if (key == "VmRSS:") {
          ram_usage = value;
          break;
        }
    }    
  }
  
  
  return ram_usage;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::string path = kProcDirectory + std::to_string(pid) + "/status";
  std::ifstream stream(path.c_str());
  
  std::string uid;
  if (stream) {
  	std::string line;
    
    std::string key;
    std::string value;
    while (std::getline(stream,line)) {
      std::istringstream streamline(line);
      streamline >> key >> value;
      
      if (key == "Uid:") {
      	uid = value;
        break;
      }
    }
  }
  
  return uid;
}
 
// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  std::ifstream stream(kPasswordPath.c_str());
  
  std::string uid = LinuxParser::Uid(pid);
  std::string user_name;
  std::string password;
  std::string file_uid;
  
  if (stream) {
  	std::string line;
    while(std::getline(stream, line)) {
      std::istringstream streamline(line);
      std::getline(streamline, user_name, ':'); 
      std::getline(streamline, password, ':');
      std::getline(streamline, file_uid, ':');
      
      if (file_uid == uid) {
      	return user_name;
      }
    }
  }
  
  return user_name;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  std::string path = kProcDirectory + std::to_string(pid) + kStatFilename;
  std::ifstream stream(path.c_str());
  
  long uptime = 0;
  std::string start_time;
  long system_uptime = LinuxParser::UpTime();
  
  if (stream) {
    std::string line;
    std::getline(stream, line);
    std::istringstream streamline(line);
    std::string ignore_output;
    for (int count = 0; count < 21; count++) {
    	streamline >> ignore_output;
    }
    streamline >> start_time;
    
    long hertz = sysconf(_SC_CLK_TCK);  // Get the number of clock ticks per second
    long starttime_seconds = static_cast<long>(std::stol(start_time)) / hertz;
    uptime = system_uptime - starttime_seconds;
  }
  
  return uptime;
}

float LinuxParser::Cpu(int pid) {
  std::string path = kProcDirectory + std::to_string(pid) + kStatFilename;
  std::ifstream stream(path.c_str());
  
  float cpu;
  if (stream) {
  	std::string line;
    std::getline(stream, line);
    std::istringstream streamline(line);
    
    std::string ignore;
    
    std::string utime;
    std::string stime;
    std::string start_time;
    
    for (int count = 1; count <= 13; count ++) {
      streamline >> ignore;
    }
    streamline >> utime >> stime;
    for (int count = 16; count <= 21; count ++) {
      streamline >> ignore;
    }
    streamline >> start_time;

    long l_utime = stol(utime);
    long l_stime = stol(stime);
   	long l_start_time = stol(start_time);
    
    long hertz = sysconf(_SC_CLK_TCK);
    long total_time = l_utime + l_stime;
    float seconds = static_cast<float>(l_start_time) / hertz;
    cpu = 100.0 * (total_time / static_cast<float>(hertz)) / seconds;
  }
  return cpu;
}