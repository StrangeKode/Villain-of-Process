#pragma once
#include <string>

class Process {
public:
    int pid;
    std::string user;
    std::string name;
    double cpu;
    long mem;

    Process(int pid, const std::string& user, const std::string& name, double cpu, long mem);
};