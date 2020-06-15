#include <string>
#include <iostream>

#include "format.h"

#define Hours_in_second 3600
#define Minutes_in_second 60
#define Hours(seconds) seconds / Hours_in_second
#define Minutes(seconds) (seconds % Hours_in_second) / 60
#define Format(value) value < 10 ? "0" : ""

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int totalHours = Hours(seconds);
  int totalMinutes = Minutes(seconds - totalHours * Hours_in_second);
  int totalSeconds = seconds - totalHours * Hours_in_second - totalMinutes * Minutes_in_second;
  string result = Format(totalHours) + std::to_string(totalHours) + ":"; 
  result += Format(totalMinutes) + std::to_string(totalMinutes) + ":"; 
  result += Format(totalSeconds) + std::to_string(totalSeconds);
}