#pragma once
#include <string>

/*
 * Clase: Process
 * Propósito: Representa un proceso individual del sistema.
 * Miembros:
 *   - pid: Identificador del proceso.
 *   - user: Usuario propietario del proceso.
 *   - group: Grupo primario del usuario.
 *   - name: Nombre del proceso.
 *   - cpu_usage: Porcentaje de uso de CPU.
 *   - mem: Memoria utilizada por el proceso en KB.
 *   - disk_read: Bytes leídos del disco.
 *   - disk_write: Bytes escritos al disco.
 *   - net_sent: Bytes enviados por red.
 *   - net_recv: Bytes recibidos por red.
 *   - gpu_usage: Porcentaje de uso de GPU.
 *   - is_service: Indica si el proceso es un servicio.
 */
class Process {
public:
    int pid;
    std::string user;
    std::string group;
    std::string name;
    double cpu_usage;
    long mem;
    long disk_read;
    long disk_write;
    long net_sent;
    long net_recv;
    double gpu_usage;
    bool is_service;

    /*
     * Constructor: Inicializa los atributos del proceso.
     */
    Process(int pid, const std::string& user, const std::string& group, const std::string& name, double cpu_usage, long mem, long disk_read, long disk_write, long net_sent, long net_recv, double gpu_usage, bool is_service);
};