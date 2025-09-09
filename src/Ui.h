#pragma once
#include "ProcessManager.h"
#include "CpuMonitor.h"

/*
 * Clase: Ui
 * Propósito: Gestiona la interfaz de usuario ncurses y la interacción con el usuario.
 * Métodos:
 *   - Ui: Constructor.
 *   - run: Ejecuta el bucle principal de la interfaz.
 */
class Ui {
public:
    Ui();
    void run();
};