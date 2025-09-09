#pragma once

/*
 * Clase: CpuMonitor
 * Propósito: Monitorea y muestra gráficamente el uso de CPU y la carga promedio del sistema.
 * Métodos:
 *   - drawCpuBars: Dibuja barras de uso de CPU por núcleo.
 *   - drawLoadAvg: Muestra la carga promedio del sistema.
 */
class CpuMonitor {
public:
    void drawCpuBars(int rowStart);
    void drawLoadAvg(int row);
};