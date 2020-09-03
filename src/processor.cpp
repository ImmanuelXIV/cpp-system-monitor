#include "processor.h"
#include "linux_parser.h"
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <iostream>

using std::string;
using std::vector;


vector<int> Processor::Utils_int(){
  vector<int> utils_int{};
  vector<string> utils = LinuxParser::CpuUtilization();
  for (unsigned int i=0; i < utils.size(); i++){
    utils_int.push_back(std::stoi(utils[i]));
  }
  return utils_int;
}

// Return the aggregate CPU utilization
float Processor::Utilization() {
  vector<int> prev = Processor::Utils_int();
  // wait 1 sec, suggestion: e.g. open new terminal to see spike
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  vector<int> now = Processor::Utils_int();

  // prev values
  int prevIdle = prev[LinuxParser::kIdle_] + prev[LinuxParser::kIOwait_];
  int prevNonIdle = prev[LinuxParser::kUser_] + prev[LinuxParser::kNice_] + prev[LinuxParser::kSystem_] + prev[LinuxParser::kIRQ_] + prev[LinuxParser::kSoftIRQ_] + prev[LinuxParser::kSteal_];
  int prevTotal = prevIdle + prevNonIdle;  
  //now values
  int nowIdle = now[LinuxParser::kIdle_] + now[LinuxParser::kIOwait_];
  int nowNonIdle = now[LinuxParser::kUser_] + now[LinuxParser::kNice_] + now[LinuxParser::kSystem_] + now[LinuxParser::kIRQ_] + now[LinuxParser::kSoftIRQ_] + now[LinuxParser::kSteal_];
  int nowTotal = nowIdle + nowNonIdle;
  // differentiate: now - prev values
  int totald = nowTotal - prevTotal;
  int idled = nowIdle - prevIdle;
  float cpu_percentage = (float)(totald - idled)/totald;
  return cpu_percentage;
}