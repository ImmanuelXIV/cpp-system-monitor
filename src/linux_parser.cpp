#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <sstream>

#include "linux_parser.h"
#include "format.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Read data from the filesystem
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

// Read data from the filesystem
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

// Maybe: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::string line, key;
  float value, memtotal, memfree;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()){
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          memtotal = value;
        }
        if (key == "MemFree:") {
          memfree = value;
          return (memtotal - memfree) / memtotal;
        }
      }
    }
  }
  return 0.0;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  long value{0};
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> value;
    return value;
  }
  return 0;
}


// Read and return CPU utilization of the System (!) (used by process.cpp)
// for each process use CpuProcessUtilization(pid)
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> utils{};
  string key, line, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10;
  int n{1};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    for (int i=0; i<n; i++){
      std::getline(filestream, line);
    }
    std::istringstream linestream(line);
    linestream >> key >> v1 >> v2 >> v3 >> v4 >> v5 >> v6 >> v7 >> v8 >> v9 >> v10;
    if (key == "cpu") {
      utils.push_back(v1);
      utils.push_back(v2);
      utils.push_back(v3);
      utils.push_back(v4);
      utils.push_back(v5);
      utils.push_back(v6);
      utils.push_back(v7);
      utils.push_back(v8);
      utils.push_back(v9);
      utils.push_back(v10);
      return utils;
    }
  }
  return utils;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string key, line;
  int value{0}, n{7};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    for (int i=0; i<n; i++){
      std::getline(filestream, line);
    }
    std::istringstream linestream(line);
    linestream >> key >> value;
    if (key == "processes") {
      return value;
    }
  }
  return value;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key, line;
  int value{0}, n{8};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    for (int i=0; i<n; i++){
      std::getline(filestream, line);
    }
    std::istringstream linestream(line);
    linestream >> key >> value;
    if (key == "procs_running") {
      return value;
    }
  }
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line{};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line, key, value{"0"};
  int ram{0};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:"){
          ram = std::stoi(value) * 0.001; //KB to MB
          return std::to_string(ram);
        }
      }
    }
  }
  return value;
}

// Return CPU Process utilization for each process
float LinuxParser::CpuProcessUtilization(int pid) {
  float cpu_usage{0.0};
  // system uptime
  long uptime = LinuxParser::UpTime();
  // process cpu util
  string line, v14, v15, v16, v17, v18, v19, v20, v21, v22;
  int utime, stime, cutime, cstime, starttime;
  std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid) + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i=0; i<14; i++){
      linestream >> v14;
    }
    linestream >> v15 >> v16 >> v17 >> v18 >> v19 >> v20 >> v21 >> v22;
    
    utime = std::stoi(v14);
    stime = std::stoi(v15);
    cutime = std::stoi(v16);
    cstime = std::stoi(v17);
    starttime = std::stoi(v22);

    int total_time = utime + stime;
    total_time += cutime + cstime;
    int hertz = sysconf(_SC_CLK_TCK);
    float sec = uptime - (starttime / hertz);
    cpu_usage = ((total_time / hertz) / sec);
  }
  return cpu_usage;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key, uid{"0"};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> uid) {
        if (key == "Uid:"){
          return uid;
        }
      }
    }
  }
  return uid;
}

// Read and return the user associated with a process
string LinuxParser::User() {
  string line, user{"user"};
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    getline(filestream, line);
    std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream linestream(line);
    linestream >> user;
  }
  return user;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid){
  string line{}, uptime_str{};
  int uptime{0};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++){
      linestream >> uptime_str;
    }
    uptime = std::stoi(uptime_str);
    return (long int)uptime/sysconf(_SC_CLK_TCK);
  }
  return uptime;
}
