#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

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
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  std::ifstream stream (kProcDirectory + kMeminfoFilename);
  float totalMen, totalFree;
  if(stream.is_open()) {
    string line, key, value;
    int items=0;
    while(std::getline(stream, line)) {
      std::istringstream lineStream(line);
      while (lineStream >> key >> value) {
        if (key == MEM_TOTAL) {
          totalMen = std::stoi(value);
          items++;
        }
        if (key == MEM_FREE) {
          totalFree = std::stoi(value);
          items++;
        }
      }
       if(items >= 2) {
          break;
        }
    }
  }
  return (totalMen - totalFree) /totalMen; 
}

// TODO: Read and return the system uptime
// /proc/uptime 
// 595.20 991.71
long LinuxParser::UpTime() { 
  long result;
  std::ifstream stream (kProcDirectory + kUptimeFilename);
  if(stream.is_open()){
    string line;
    std::getline(stream, line);
    std::istringstream streamLine(line);
    string uptime;

    streamLine >> uptime;
    result = std::stol(uptime);
  }  
  return result; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return LinuxParser::CalculeteJiffies(kUser_, kGuest_);
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  long result{0};

  std::ifstream stream (kProcDirectory + to_string(pid) + kStatFilename);
  if(stream.is_open()){
    string line;
    string token;
    std::getline(stream, line);
    std::istringstream streamLine(line);
    int position{0};
    for (string token; streamLine >> token; position ++) {
      if (position > 12 && position < 17) {
        result += std::stol(token);
      }
    }
  }  
  return result;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies(); 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  return LinuxParser::CalculeteJiffies(kIdle_, kIOwait_); 
}

long LinuxParser::CalculeteJiffies(int iniPos, int endPos){
  long result{0};
  vector<string> cpuUtilization = LinuxParser::CpuUtilization();
  for(int index = iniPos; index < endPos; index++) {
    result += std::stol(cpuUtilization[index]);
  }
  return result;
}

// TODO: Read and return CPU utilization
// cpu  18160 1444 8588 118008 1523 0 144 0 0 0
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> result;

  std::ifstream stream (kProcDirectory + kStatFilename);
  if(stream.is_open()){
    string line;
    string token;
    std::getline(stream, line);
    std::istringstream streamLine(line);
    
    while(std::getline(streamLine, token, ' ')) {
      if(token != "" && token != CPU) {
        result.push_back(token);
      }
    }
  }  
  return result; 
}

// TODO: Read and return the total number of processes
// S 10845
int LinuxParser::TotalProcesses() { 
  return LinuxParser::GetKeyInStat(PROCESSES); 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  return LinuxParser::GetKeyInStat(PROCS_RUNNING); 
}

int LinuxParser::GetKeyInStat(string key) {
   int result {0};
  std::ifstream stream (kProcDirectory + kStatFilename);
  if(stream.is_open()){
    string line, token, value;
    while(std::getline(stream, line)) {
      std::istringstream lineStream(line);
      lineStream >> token >> value;
      if (token == key) {
        result = std::stoi(value);
        break;
      }
    }
  }
  return result; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string result {""};
   std::ifstream stream (kProcDirectory + to_string(pid) + kCmdlineFilename);
   if(stream.is_open()) {
     std::getline(stream, result);
   }
  return result; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
        return std::to_string(std::stof(LinuxParser::GetValueStatus(pid, VM_SIZE)) / 1000); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { return LinuxParser::GetValueStatus(pid, UID); }

string LinuxParser::GetValueStatus(int pid, string key) {
   string result {"0"};
   std::ifstream stream (kProcDirectory + to_string(pid) + kStatusFilename);
   if(stream.is_open()) {
     string line, token, value;
     std::getline(stream, line);
      
      while(std::getline(stream, line)) {
      std::istringstream lineStream(line);
      lineStream >> token >> value;
      if (token == key) {
        result = value;
        break;
      }
    }
   }
  return result; 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string result {"None"}, uid;
  uid = LinuxParser::Uid(pid);

  if (uid != "" && uid !="0") {
    std::ifstream stream (kPasswordPath);
    if(stream.is_open()){
      string line, token, value;
      while(std::getline(stream, line)){
        std::istringstream strToToken(line);
        strToToken >> token >> value >> value;
        if(value == uid) {
          result = token;
          break;
        }
      }

    }

  }

  return result; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  long result {0};
  
  std::ifstream stream (kProcDirectory + kStatFilename);
  if(stream.is_open()){
    string line, token;
    std::getline(stream, line);
    std::istringstream tokens (line);

    for(int index=0; std::getline(tokens, token, ' '); index++) {
      if (index == UP_TIME_POS) {
        result = std::stol(token);
        break;
      }
    }
  }
  return result; 
}