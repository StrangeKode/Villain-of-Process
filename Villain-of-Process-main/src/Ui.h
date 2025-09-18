#pragma once
#include "ProcessManager.h"
#include "CpuMonitor.h"

/*
 * Clase: Ui
 * Propósito: Gestiona la interfaz de usuario ncurses y la interacción con el usuario.
 * Miembros:
 *   - sort_mode: Modo de ordenamiento (0: CPU, 1: MEM, 2: DISK).
 * Métodos:
 *   - Ui: Constructor.
 *   - run: Ejecuta el bucle principal de la interfaz.
 */
class Ui {
public:
    Ui();
    void run();
private:
    int sort_mode = 0;
};