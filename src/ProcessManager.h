#pragma once
#include <vector>
#include "Process.h"

class ProcessManager {
public:
    std::vector<Process> getProcesses();
    long readMemory(int pid);
    std::string getUsername(int pid);
};