#include "ProcessManager.h"
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <pwd.h>
#include <unistd.h>
#include <algorithm>

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
 * Método: getProcesses
 * Propósito: Obtiene la lista de procesos activos en el sistema.
 * Retorno: Vector de objetos Process con la información de cada proceso.
 * Detalles: Lee el directorio /proc, obtiene información relevante y la almacena en objetos Process.
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

        std::string user = getUsername(pid);
        long mem = readMemory(pid);

        processes.push_back(Process(pid, user, name, 0.0, mem));
    }
    closedir(dir);

    // Ordena los procesos por uso de memoria descendente
    std::sort(processes.begin(), processes.end(),
        [](auto &a, auto &b) { return a.mem > b.mem; });

    return processes;
}