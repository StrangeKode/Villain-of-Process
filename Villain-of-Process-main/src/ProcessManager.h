#pragma once
#include <vector>
#include "Process.h"

/*
 * Clase: ProcessManager
 * Propósito: Gestiona la obtención y procesamiento de la lista de procesos del sistema.
 * Métodos:
 *   - getProcesses: Devuelve la lista de procesos actuales.
 *   - readMemory: Lee la memoria utilizada por un proceso.
 *   - getUsername: Obtiene el nombre de usuario propietario del proceso.
 *   - getUserAndGroup: Obtiene el nombre de usuario y grupo propietario del proceso.
 *   - readCpuUsage: Calcula el uso de CPU por proceso.
 *   - readDiskIO: Lee I/O de disco por proceso.
 *   - readNetIO: Lee I/O de red por proceso (placeholder).
 *   - readGpuUsage: Lee uso de GPU por proceso (placeholder).
 */
class ProcessManager {
public:
    std::vector<Process> getProcesses();
    long readMemory(int pid);
    std::string getUsername(int pid);
    std::pair<std::string, std::string> getUserAndGroup(int pid);
    double readCpuUsage(int pid);
    std::pair<long, long> readDiskIO(int pid);
    std::pair<long, long> readNetIO(int pid);
    double readGpuUsage(int pid);
};