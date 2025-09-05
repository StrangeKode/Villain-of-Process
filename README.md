
# VOP 
## Villain Of Process
![](https://raw.githubusercontent.com/StrangeKode/Villain-of-Process/refs/heads/main/logo.png)

VOP (**Villain of Process**) es un monitor de procesos minimalista para Linux, inspirado en `htop` pero mucho más ligero. Su objetivo es ofrecer una forma rápida y simple de visualizar el uso de CPU y los procesos activos en tu sistema, sin dependencias pesadas ni interfaces recargadas.


## Características
- 📊 **Monitoreo en tiempo real** del uso de CPU por núcleo.
- 🔎 **Listado de procesos activos** con PID, usuario y nombre.
- 🪶 **Interfaz minimalista** basada en `ncurses`.
- ⚡ **Rendimiento ligero**, ideal para servidores o equipos con pocos recursos.
- 🛠 **Compilación sencilla** con CMake.
                
----
## 📋 Dependencias

Para compilar VOP necesitas:
- **CMake** (>= 3.10)
- **g++** o cualquier compilador compatible con C++11 o superior
- **ncurses**

### 🔹 Instalación de dependencias
#### Ubuntu / Debian / Linux Mint
```bash 
sudo apt update
```
```bash 
sudo apt install build-essential cmake libncurses5-dev   libncursesw5-dev git
```

#### Arch Linux / Manjaro
```bash
sudo pacman -S base-devel cmake ncurses git
```

### Fedora / CentOS / RHEL
```bash 
sudo dnf install @development-tools cmake ncurses-devel git
```

#### OpenSUSE
```bash 
sudo zypper install -t pattern devel_C_C++
```
```bash 
sudo zypper install cmake ncurses-devel git
```

## ⚙️ Compilación e instalación
Clonamos el directorio
```bash
git clone https://github.com/StrangeKode/Villain-of-Process
```

Accedemos a la raiz
```bash
cd Villain-of-Process
```

Creamos los build
```bash 
mkdir build && cd build
```
```bash
cmake ..
```
```bash
make
```
```bash
./vop
```

## 🔧 Instalación en el sistema
Si queremos instalar vop en el PATH de nuestro sistema para usarlo facilmente podemos hacer

```bash
sudo make install
```

## 🚀 Usabilidad

Ejecuta el programa:

```bash
vop
```


Verás:

- Uso de CPU por núcleo (gráfico con barras ASCII).

Lista de procesos con:

- PID

- Usuario

- Nombre del proceso

## 🛣 Roadmap

-  Mostrar uso de memoria RAM.

- Agregar opción de filtrar procesos.

- Función para terminar procesos desde la interfaz.

- Versión con colores personalizables.

## 🤝 Contribuciones

¡Las contribuciones son bienvenidas!
Haz un fork del proyecto, crea una rama (feature/nueva-funcion) y abre un Pull Request 🚀.

## 📜 Licencia

Este proyecto se distribuye bajo la licencia GLP.
Puedes usarlo, modificarlo y compartirlo libremente.
