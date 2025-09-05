#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <pwd.h>
#include <ncurses.h>
#include <algorithm>
#include <limits>

struct Process {
    int pid;
    std::string user;
    std::string name;
    double cpu;
    long mem; // en KB
};

// 🔹 Leer UID de un proceso
uid_t read_uid(int pid) {
    std::string path = "/proc/" + std::to_string(pid) + "/status";
    std::ifstream f(path);
    std::string line;
    while (std::getline(f, line)) {
        if (line.rfind("Uid:", 0) == 0) {
            std::istringstream iss(line);
            std::string key;
            uid_t uid;
            iss >> key >> uid;
            return uid;
        }
    }
    return -1;
}

// 🔹 Obtener username a partir de UID
std::string username_from_uid(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    return pw ? pw->pw_name : std::to_string(uid);
}

// 🔹 Leer memoria usada
long read_memory(int pid) {
    std::string path = "/proc/" + std::to_string(pid) + "/statm";
    std::ifstream f(path);
    long pages;
    if (f >> pages) {
        return pages * sysconf(_SC_PAGESIZE) / 1024; // KB
    }
    return 0;
}

// 🔹 Leer procesos
std::vector<Process> get_processes() {
    std::vector<Process> processes;
    DIR *dir = opendir("/proc");
    if (!dir) return processes;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!isdigit(entry->d_name[0])) continue;
        int pid = atoi(entry->d_name);

        std::string commPath = "/proc/" + std::to_string(pid) + "/comm";
        std::ifstream commFile(commPath);
        std::string name;
        if (!(commFile >> name)) continue;

        uid_t uid = read_uid(pid);
        std::string user = username_from_uid(uid);

        long mem = read_memory(pid);

        processes.push_back({pid, user, name, 0.0, mem});
    }
    closedir(dir);

    // 🔹 Ordenar por memoria (default)
    std::sort(processes.begin(), processes.end(),
              [](auto &a, auto &b) { return a.mem > b.mem; });

    return processes;
}

// 🔹 Mostrar barra de CPU por núcleo
void draw_cpu_bars(int row_start) {
    std::ifstream f("/proc/stat");
    std::string line;
    int row = row_start;
    static std::map<std::string, std::pair<unsigned long long,unsigned long long>> prev;

    while (std::getline(f, line)) {
        if (line.rfind("cpu", 0) != 0) break; // solo CPUs
        std::istringstream iss(line);
        std::string label;
        unsigned long long user,nice_,system,idle,iowait,irq,softirq,steal;
        iss >> label >> user >> nice_ >> system >> idle >> iowait >> irq >> softirq >> steal;
        unsigned long long idle_all = idle + iowait;
        unsigned long long non_idle = user + nice_ + system + irq + softirq + steal;
        unsigned long long total = idle_all + non_idle;

        auto& prevvals = prev[label];
        unsigned long long dtotal = total - prevvals.first;
        unsigned long long dbusy  = non_idle - prevvals.second;
        double usage = (dtotal > 0) ? (100.0 * dbusy / dtotal) : 0.0;
        prevvals = {total, non_idle};

        int barlen = 20;
        int filled = (int)(usage/100.0 * barlen);

        // 🔹 Selección de color
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

// 🔹 Mostrar carga promedio
void draw_loadavg(int row) {
    std::ifstream f("/proc/loadavg");
    double l1, l5, l15;
    f >> l1 >> l5 >> l15;
    mvprintw(row, 0, "Load avg: %.2f %.2f %.2f", l1, l5, l15);
}

int main() {
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);   // CPU baja
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);  // CPU media
    init_pair(3, COLOR_RED, COLOR_BLACK);     // CPU alta
    init_pair(4, COLOR_CYAN, COLOR_BLACK);    // Encabezado
    init_pair(5, COLOR_BLACK, COLOR_WHITE);   // Proceso seleccionado

    int selected = 0;

    while (true) {
        clear();

        // CPU
        draw_cpu_bars(0);

        // Load average
        draw_loadavg(5);

        // Procesos
        auto processes = get_processes();
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

        int ch = getch();
        if (ch == 'q') break;
        if (ch == KEY_UP && selected > 0) selected--;
        if (ch == KEY_DOWN && selected < 19) selected++;
        if (ch == 'k' && selected < (int)processes.size()) {
            kill(processes[selected].pid, SIGKILL);
        }
    }

    endwin();
    return 0;
}
