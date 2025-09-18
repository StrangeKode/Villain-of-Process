#include "ProcessManager.h"
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <algorithm>
#include <map>
#include <chrono>
#include <utility>

/*
 * Método: readMemory
 * Propósito: Lee la memoria utilizada por el proceso especificado.
 * Parámetro: pid - Identificador del proceso.
 * Retorno: Memoria utilizada en KB.
 */
long ProcessManager::readMemory(int pid) {
    std::string path = "/proc/" + std::to_string(pid) + "/statm";
    std::ifstream f(path);
    long pages;
    if (f >> pages) {
        return pages * sysconf(_SC_PAGESIZE) / 1024;
    }
    return 0;
}

/*
 * Método: getUsername
 * Propósito: Obtiene el nombre de usuario propietario del proceso.
 * Parámetro: pid - Identificador del proceso.
 * Retorno: Nombre de usuario.
 */
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

/*
 * Método: getUserAndGroup
 * Propósito: Obtiene el nombre de usuario y grupo propietario del proceso.
 * Parámetro: pid - Identificador del proceso.
 * Retorno: Par de (usuario, grupo).
 */
std::pair<std::string, std::string> ProcessManager::getUserAndGroup(int pid) {
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
    std::string user = pw ? pw->pw_name : std::to_string(uid);
    std::string group = "unknown";
    if (pw) {
        struct group *gr = getgrgid(pw->pw_gid);
        if (gr) group = gr->gr_name;
    }
    return {user, group};
}

/*
 * Método: getProcesses
 * Propósito: Obtiene la lista de procesos activos en el sistema con métricas de rendimiento.
 * Retorno: Vector de objetos Process con la información de cada proceso.
 * Detalles: Lee el directorio /proc, obtiene información relevante incluyendo uso de CPU, memoria, I/O de disco, etc., y la almacena en objetos Process.
 */
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

        std::string statPath = "/proc/" + std::to_string(pid) + "/stat";
        std::ifstream statFile(statPath);
        std::string statLine;
        if (!std::getline(statFile, statLine)) continue;
        std::istringstream iss(statLine);
        int pid_stat, ppid;
        std::string comm, state;
        iss >> pid_stat >> comm >> state >> ppid;
        bool is_service = (ppid == 1);

        auto [user, group] = getUserAndGroup(pid);
        long mem = readMemory(pid);
        double cpu_usage = readCpuUsage(pid);
        auto [disk_read, disk_write] = readDiskIO(pid);
        auto [net_sent, net_recv] = readNetIO(pid);
        double gpu_usage = readGpuUsage(pid);

        processes.push_back(Process(pid, user, group, name, cpu_usage, mem, disk_read, disk_write, net_sent, net_recv, gpu_usage, is_service));
    }
    closedir(dir);

    return processes;
}

/*
 * Método: readCpuUsage
 * Propósito: Calcula el porcentaje de uso de CPU por proceso.
 * Parámetro: pid - Identificador del proceso.
 * Retorno: Porcentaje de uso de CPU.
 */
double ProcessManager::readCpuUsage(int pid) {
    static std::map<int, std::pair<unsigned long long, std::chrono::steady_clock::time_point>> prev;
    std::string path = "/proc/" + std::to_string(pid) + "/stat";
    std::ifstream f(path);
    std::string line;
    if (!std::getline(f, line)) return 0.0;
    std::istringstream iss(line);
    int pid_stat;
    std::string comm, state;
    int ppid, pgrp, session, tty_nr, tpgid, flags, minflt, cminflt, majflt, cmajflt;
    unsigned long long utime, stime;
    iss >> pid_stat >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >> stime;
    unsigned long long cpu_time = utime + stime;
    auto now = std::chrono::steady_clock::now();
    auto& prev_data = prev[pid];
    if (prev_data.second.time_since_epoch().count() == 0) {
        prev_data = {cpu_time, now};
        return 0.0;
    }
    auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - prev_data.second).count() / 1000.0;
    if (time_diff == 0) return 0.0;
    unsigned long long cpu_diff = cpu_time - prev_data.first;
    double usage = (cpu_diff / (double)sysconf(_SC_CLK_TCK)) / time_diff * 100.0;
    prev_data = {cpu_time, now};
    return usage;
}

/*
 * Método: readDiskIO
 * Propósito: Lee los bytes leídos y escritos al disco por el proceso.
 * Parámetro: pid - Identificador del proceso.
 * Retorno: Par de (read_bytes, write_bytes).
 */
std::pair<long, long> ProcessManager::readDiskIO(int pid) {
    std::string path = "/proc/" + std::to_string(pid) + "/io";
    std::ifstream f(path);
    std::string line;
    long read_bytes = 0, write_bytes = 0;
    while (std::getline(f, line)) {
        if (line.rfind("read_bytes:", 0) == 0) {
            std::istringstream iss(line);
            std::string key;
            iss >> key >> read_bytes;
        } else if (line.rfind("write_bytes:", 0) == 0) {
            std::istringstream iss(line);
            std::string key;
            iss >> key >> write_bytes;
        }
    }
    return {read_bytes, write_bytes};
}

/*
 * Método: readNetIO
 * Propósito: Lee los bytes enviados y recibidos por red (placeholder, no implementado).
 * Parámetro: pid - Identificador del proceso.
 * Retorno: Par de (sent_bytes, recv_bytes).
 */
std::pair<long, long> ProcessManager::readNetIO(int pid) {
    // Placeholder: no fácil de obtener per-proceso en Linux sin herramientas adicionales
    return {0, 0};
}

/*
 * Método: readGpuUsage
 * Propósito: Lee el porcentaje de uso de GPU (placeholder, no implementado).
 * Parámetro: pid - Identificador del proceso.
 * Retorno: Porcentaje de uso de GPU.
 */
double ProcessManager::readGpuUsage(int pid) {
    // Placeholder: requiere herramientas específicas de GPU
    return 0.0;
}