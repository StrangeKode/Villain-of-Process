# Villain of Process (VOP)

Un monitor de procesos estilo `htop`, escrito en **C++17** con **ncurses**.  
Incluye:
- ✅ Barras de CPU por núcleo  
- ✅ Carga promedio del sistema  
- ✅ Uso de memoria  
- ✅ Búsqueda de procesos `/`  
- ✅ Renice con tecla `r`  
- ✅ Selección múltiple y kill (`k` / `K`)  
- ✅ Colores (procesos del usuario en verde)  

---

## 🚀 Compilación y uso

### 🔹 Opción 1: Usando **Makefile**
Compilar:
```bash
make
---
Ejecutar:

./vop
---

Limpiar binarios:

make clean
---
### 🔹 Opción 2: Usando **Makefile**
---
Crear carpeta de build:

mkdir build && cd build
---

Generar y compilar:

cmake ..
make
---

Ejecutar:

./vop

---
🖥️ Controles del programa

↑ / ↓: mover selección

espacio: marcar proceso

k: SIGTERM al proceso seleccionado / marcados

K: SIGKILL

c: ordenar por CPU

m: ordenar por memoria

/: buscar proceso por nombre

r: renice (cambia la prioridad)

q: salir
---

📦 Dependencias
---
En Arch/Manjaro:
---
sudo pacman -S base-devel ncurses
---
---
En Ubuntu/Debian:
---
sudo apt install build-essential libncurses5-dev libncursesw5-dev
---
📜 Licencia

GNU 3.0 – libre para usar, modificar y compartir.
---
