#include "CpuMonitor.h"
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <ncurses.h>

/*
 * Método: drawCpuBars
 * Propósito: Dibuja barras de uso de CPU por núcleo en la interfaz ncurses.
 * Parámetro: rowStart - fila inicial donde se dibujan las barras.
 * Detalles: Lee /proc/stat, calcula el uso de cada núcleo y muestra una barra gráfica.
 */
void CpuMonitor::drawCpuBars(int rowStart) {
    std::ifstream f("/proc/stat");
    std::string line;
    int row = rowStart;
    // Mapa estático para almacenar valores previos de cada núcleo
    static std::map<std::string, std::pair<unsigned long long,unsigned long long>> prev;

    while (std::getline(f, line)) {
        // Solo procesa líneas que empiezan con "cpu"
        if (line.rfind("cpu", 0) != 0) break;
        std::istringstream iss(line);
        std::string label;
        unsigned long long user,nice_,system,idle,iowait,irq,softirq,steal;
        // Extrae los valores de uso de CPU
        iss >> label >> user >> nice_ >> system >> idle >> iowait >> irq >> softirq >> steal;
        unsigned long long idle_all = idle + iowait;
        unsigned long long non_idle = user + nice_ + system + irq + softirq + steal;
        unsigned long long total = idle_all + non_idle;

        // Calcula la diferencia respecto a la lectura anterior
        auto& prevvals = prev[label];
        unsigned long long dtotal = total - prevvals.first;
        unsigned long long dbusy  = non_idle - prevvals.second;
        double usage = (dtotal > 0) ? (100.0 * dbusy / dtotal) : 0.0;
        prevvals = {total, non_idle};

        // Dibuja la barra de uso de CPU
        int barlen = 20;
        int filled = (int)(usage/100.0 * barlen);

        // Selecciona el color según el porcentaje de uso
        int color = 1;
        if (usage > 80) color = 3;
        else if (usage > 50) color = 2;

        attron(COLOR_PAIR(color));
        std::string bar(filled, '█');
        std::string empty(barlen - filled, ' ');
        mvprintw(row, 0, "%-5s [%s%s] %5.1f%%", label.c_str(), bar.c_str(), empty.c_str(), usage);
        attroff(COLOR_PAIR(color));

        row++;
    }
}

/*
 * Método: drawLoadAvg
 * Propósito: Muestra el promedio de carga del sistema en la interfaz ncurses.
 * Parámetro: row - fila donde se muestra la información.
 * Detalles: Lee /proc/loadavg y muestra los valores.
 */
void CpuMonitor::drawLoadAvg(int row) {
    std::ifstream f("/proc/loadavg");
    double l1, l5, l15;
    f >> l1 >> l5 >> l15;
    mvprintw(row, 0, "Load avg: %.2f %.2f %.2f", l1, l5, l15);
}