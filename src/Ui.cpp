#include "Ui.h"
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>

/*
 * Constructor de la clase Ui.
 * Inicializa cualquier recurso necesario para la interfaz.
 */
Ui::Ui() {}

/*
 * Método: run
 * Propósito: Ejecuta el bucle principal de la interfaz ncurses.
 * Detalles:
 *   - Inicializa ncurses y los colores.
 *   - Muestra información de CPU y procesos.
 *   - Permite navegar, seleccionar y finalizar procesos.
 *   - Actualiza la pantalla periódicamente.
 */
void Ui::run() {
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
    init_pair(5, COLOR_BLACK, COLOR_WHITE);

    int selected = 0;
    ProcessManager pm;
    CpuMonitor cpu;

    while (true) {
        clear();

        // Dibuja barras de CPU y carga promedio
        cpu.drawCpuBars(0);
        cpu.drawLoadAvg(5);

        // Obtiene y muestra la lista de procesos
        auto processes = pm.getProcesses();
        int row = 7;
        attron(COLOR_PAIR(4));
        mvprintw(row++, 0, "%-6s %-10s %-20s %-8s", "PID", "USER", "NAME", "MEM(KB)");
        attroff(COLOR_PAIR(4));

        for (int i = 0; i < (int)processes.size() && i < 20; i++) {
            auto &p = processes[i];
            if (i == selected) {
                attron(COLOR_PAIR(5));
                mvprintw(row++, 0, "%-6d %-10s %-20s %-8ld", p.pid, p.user.c_str(), p.name.c_str(), p.mem);
                attroff(COLOR_PAIR(5));
            } else {
                mvprintw(row++, 0, "%-6d %-10s %-20s %-8ld", p.pid, p.user.c_str(), p.name.c_str(), p.mem);
            }
        }

        refresh();
        usleep(200000);

        // Maneja la entrada del usuario
        int ch = getch();
        if (ch == 'q') break;
        if (ch == KEY_UP && selected > 0) selected--;
        if (ch == KEY_DOWN && selected < 19) selected++;
        if (ch == 'k' && selected < (int)processes.size()) {
            // Finaliza el proceso seleccionado
            kill(processes[selected].pid, SIGKILL);
        }
    }

    endwin();
}