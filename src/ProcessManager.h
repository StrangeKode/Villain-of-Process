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
 */
class ProcessManager {
public:
    std::vector<Process> getProcesses();
    long readMemory(int pid);
    std::string getUsername(int pid);
};