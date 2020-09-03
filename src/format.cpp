#include <string>
#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int time, hours, mins, secs;
  hours = seconds/3600;
  time = seconds%3600;
  mins = time/60;
  time = time%60;
  secs = time;
  return std::to_string(hours) + ":" + std::to_string(mins) + ":" + std::to_string(secs);
}