#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid(pid) {
    command = LinuxParser::Command(pid);
    user = LinuxParser::User(pid);
}

int Process::Pid() { return Process::pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return 0; }

string Process::Command() { return command; }

string Process::Ram() { return LinuxParser::Ram(pid); }

string Process::User() { return user; }

long int Process::UpTime() { return LinuxParser::UpTime(pid); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }