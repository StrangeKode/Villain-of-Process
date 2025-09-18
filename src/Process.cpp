#include "Process.h"

/*
 * Implementación del constructor de la clase Process.
 * Inicializa los atributos con los valores proporcionados.
 */
Process::Process(int pid, const std::string& user, const std::string& group, const std::string& name, double cpu_usage, long mem, long disk_read, long disk_write, long net_sent, long net_recv, double gpu_usage, bool is_service)
    : pid(pid), user(user), group(group), name(name), cpu_usage(cpu_usage), mem(mem), disk_read(disk_read), disk_write(disk_write), net_sent(net_sent), net_recv(net_recv), gpu_usage(gpu_usage), is_service(is_service) {}