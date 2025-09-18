#include "Ui.h"
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>
#include <map>
#include <vector>
#include <algorithm>

struct UserStats {
    double total_cpu = 0;
    long total_mem = 0;
    long total_disk = 0;
    int process_count = 0;
};

// Helper function to format memory
std::string formatMemory(long kb) {
    if (kb >= 1024 * 1024) {
        return std::to_string(kb / (1024 * 1024)) + "GB";
    } else if (kb >= 1024) {
        return std::to_string(kb / 1024) + "MB";
    } else {
        return std::to_string(kb) + "KB";
    }
}

// Helper function to format disk
std::string formatDisk(long mb) {
    if (mb >= 1024) {
        return std::to_string(mb / 1024) + "GB";
    } else {
        return std::to_string(mb) + "MB";
    }
}

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
 *   - Muestra información de CPU, carga promedio, lista de procesos con métricas de rendimiento, grupos y servicios.
 *   - Agrega resumen de consumo por usuario.
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
    init_pair(1, COLOR_GREEN, COLOR_BLACK);    // Low CPU
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);   // Medium CPU
    init_pair(3, COLOR_RED, COLOR_BLACK);      // High CPU
    init_pair(4, COLOR_CYAN, COLOR_BLACK);     // Headers
    init_pair(5, COLOR_BLACK, COLOR_WHITE);    // Selected
    init_pair(6, COLOR_BLUE, COLOR_BLACK);     // Services
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);  // High memory
    init_pair(8, COLOR_WHITE, COLOR_BLUE);     // Summary background

    int selected = 0;
    ProcessManager pm;
    CpuMonitor cpu;

    while (true) {
        clear();

        // Draw border
        box(stdscr, 0, 0);
        mvprintw(0, 2, " Monitor de Procesos Linux ");

        // Dibuja barras de CPU y carga promedio
        mvprintw(1, 2, "Uso de CPU:");
        cpu.drawCpuBars(2);
        cpu.drawLoadAvg(7);
        mvhline(9, 1, ACS_HLINE, COLS - 2);

        // Obtiene y muestra la lista de procesos
        auto processes = pm.getProcesses();
        // Sort processes based on sort_mode
        if (sort_mode == 0) {
            std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) { return a.cpu_usage > b.cpu_usage; });
        } else if (sort_mode == 1) {
            std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) { return a.mem > b.mem; });
        } else if (sort_mode == 2) {
            std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) { return (a.disk_read + a.disk_write) > (b.disk_read + b.disk_write); });
        }
        std::map<std::string, UserStats> userStats;
        for (const auto& p : processes) {
            userStats[p.user].total_cpu += p.cpu_usage;
            userStats[p.user].total_mem += p.mem;
            userStats[p.user].total_disk += (p.disk_read + p.disk_write) / 1024 / 1024;
            userStats[p.user].process_count++;
        }
        std::vector<std::pair<std::string, UserStats>> sortedUsers(userStats.begin(), userStats.end());
        std::sort(sortedUsers.begin(), sortedUsers.end(), [](const auto& a, const auto& b) { return a.second.total_cpu > b.second.total_cpu; });

        int row = 10;
        mvprintw(row++, 2, "Procesos:");
        mvhline(row, 1, ACS_HLINE, COLS - 2);
        row++;
        attron(COLOR_PAIR(4));
        mvprintw(row++, 2, "%-6s %-10s %-10s %-15s %-8s %-6s %-10s %-6s %-10s %-3s", "PID", "USUARIO", "GRUPO", "NOMBRE", "MEM", "CPU%", "DISK", "GPU%", "NET", "SVC");
        attroff(COLOR_PAIR(4));
        mvhline(row, 1, ACS_HLINE, COLS - 2);
        row++;

        for (int i = 0; i < (int)processes.size() && i < 20; i++) {
            auto &p = processes[i];
            double disk_mb = (p.disk_read + p.disk_write) / 1024.0 / 1024.0;
            std::string mem_str = formatMemory(p.mem);
            std::string disk_str = formatDisk((long)disk_mb);
            int color = 1; // default green
            if (p.is_service) color = 6; // blue for services
            else if (p.cpu_usage > 50) color = 3; // red for high CPU
            else if (p.cpu_usage > 20) color = 2; // yellow for medium
            if (i == selected) color = 5; // selected
            std::string net_str = formatDisk((long)(p.net_sent + p.net_recv) / 1024.0 / 1024.0); // in MB
            attron(COLOR_PAIR(color));
            mvprintw(row++, 2, "%-6d %-10s %-10s %-15s %-8s %-5.1f %-9s %-5.1f %-9s %-3s", p.pid, p.user.c_str(), p.group.c_str(), p.name.c_str(), mem_str.c_str(), p.cpu_usage, disk_str.c_str(), p.gpu_usage, net_str.c_str(), p.is_service ? "S" : "N");
            attroff(COLOR_PAIR(color));
        }

        // Summary
        row += 2;
        mvhline(row, 1, ACS_HLINE, COLS - 2);
        row++;
        mvprintw(row++, 2, "Resumen de Consumo por Usuario (Top por CPU):");
        for (int i = 0; i < (int)sortedUsers.size() && i < 5; i++) {
            const auto& [user, stats] = sortedUsers[i];
            std::string mem_str = formatMemory(stats.total_mem);
            std::string disk_str = formatDisk(stats.total_disk);
            mvprintw(row++, 2, "%-10s CPU: %-5.1f%% MEM: %-8s DISK: %-8s PROC: %d", user.c_str(), stats.total_cpu, mem_str.c_str(), disk_str.c_str(), stats.process_count);
        }
        row += 2;
        mvprintw(row++, 2, "Controles: ARRIBA/ABAJO seleccionar, K matar, C/M/D ordenar por CPU/MEM/DISCO, Q salir");

        refresh();
        usleep(200000);

        // Maneja la entrada del usuario
        int ch = getch();
        if (ch == 'q') break;
        if (ch == KEY_UP && selected > 0) selected--;
        if (ch == KEY_DOWN && selected < 19 && selected < (int)processes.size() - 1) selected++;
        if (ch == 'k' && selected < (int)processes.size()) {
            // Finaliza el proceso seleccionado
            kill(processes[selected].pid, SIGKILL);
        }
        if (ch == 'c') sort_mode = 0; // sort by CPU
        if (ch == 'm') sort_mode = 1; // sort by MEM
        if (ch == 'd') sort_mode = 2; // sort by DISK
    }

    endwin();
}