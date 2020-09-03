#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Constructor
Process::Process(int pid) : pid_(pid) {};

// Return this process's ID
int Process::Pid() const { 
  return pid_;
}

// Return this process's CPU utilization
float Process::CpuUtilization() const { 
  return LinuxParser::CpuProcessUtilization(Pid()); 
}

// Return the command that generated this process
string Process::Command() {
  return LinuxParser::Command(Process::Pid());
}

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Process::Pid()); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Process::Pid()); }

// Overload the "greater than" comparison operator for Process objects
bool Process::operator>(Process const& a) const { 
  return this->CpuUtilization() > a.CpuUtilization() ? true : false;
}