#include "Process.h"

Process::Process(int pid, const std::string& user, const std::string& name, double cpu, long mem)
    : pid(pid), user(user), name(name), cpu(cpu), mem(mem) {}