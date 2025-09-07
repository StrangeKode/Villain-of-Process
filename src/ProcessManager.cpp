#include "ProcessManager.h"
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <pwd.h>
#include <unistd.h>
#include <algorithm>

long ProcessManager::readMemory(int pid) {
    std::string path = "/proc/" + std::to_string(pid) + "/statm";
    std::ifstream f(path);
    long pages;
    if (f >> pages) {
        return pages * sysconf(_SC_PAGESIZE) / 1024;
    }
    return 0;
}

std::string ProcessManager::getUsername(int pid) {
    std::string path = "/proc/" + std::to_string(pid) + "/status";
    std::ifstream f(path);
    std::string line;
    uid_t uid = -1;
    while (std::getline(f, line)) {
        if (line.rfind("Uid:", 0) == 0) {
            std::istringstream iss(line);
            std::string key;
            iss >> key >> uid;
            break;
        }
    }
    struct passwd *pw = getpwuid(uid);
    return pw ? pw->pw_name : std::to_string(uid);
}

std::vector<Process> ProcessManager::getProcesses() {
    std::vector<Process> processes;
    DIR *dir = opendir("/proc");
    if (!dir) return processes;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!isdigit(entry->d_name[0])) continue;
        int pid = atoi(entry->d_name);

        std::string commPath = "/proc/" + std::to_string(pid) + "/comm";
        std::ifstream commFile(commPath);
        std::string name;
        if (!(commFile >> name)) continue;

        std::string user = getUsername(pid);
        long mem = readMemory(pid);

        processes.push_back(Process(pid, user, name, 0.0, mem));
    }
    closedir(dir);

    std::sort(processes.begin(), processes.end(),
        [](auto &a, auto &b) { return a.mem > b.mem; });

    return processes;
}