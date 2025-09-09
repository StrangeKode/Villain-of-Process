#pragma once
#include <string>

/*
 * Clase: Process
 * Propósito: Representa un proceso individual del sistema.
 * Miembros:
 *   - pid: Identificador del proceso.
 *   - user: Usuario propietario del proceso.
 *   - name: Nombre del proceso.
 *   - cpu: Porcentaje de uso de CPU (puede ser extendido).
 *   - mem: Memoria utilizada por el proceso en KB.
 */
class Process {
public:
    int pid;
    std::string user;
    std::string name;
    double cpu;
    long mem;

    /*
     * Constructor: Inicializa los atributos del proceso.
     */
    Process(int pid, const std::string& user, const std::string& name, double cpu, long mem);
};