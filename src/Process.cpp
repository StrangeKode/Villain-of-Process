#include "Process.h"

/*
 * Implementación del constructor de la clase Process.
 * Inicializa los atributos con los valores proporcionados.
 */
Process::Process(int pid, const std::string& user, const std::string& name, double cpu, long mem)
    : pid(pid), user(user), name(name), cpu(cpu), mem(mem) {}