
# Requiere una versión mínima de CMake
cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(RomSteps)

# Establecer la versión del estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Añadir el ejecutable (nombre del archivo de salida y el archivo de código fuente principal)
set(SOURCE_ROMERIN
    
)

add_executable(RomSteps Trayectoria.cpp ${SOURCE_ROMERIN})


# Si necesitas incluir más archivos fuente o cabeceras
# add_executable(MiEjecutable main.cpp otraFuente.cpp)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Incluir directorios adicionales donde estén los archivos de cabecera (*.h)
 include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
 include_directories("${CMAKE_CURRENT_SOURCE_DIR}/shared")

# Librerías adicionales (si utilizas librerías externas)
 
 find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network )
 
 target_link_libraries(RomSteps
    Qt5::Core
    Qt5::Widgets
    Qt5::Network)


# Configuración específica para Visual Studio
if (MSVC)
    # Opciones del compilador para Visual Studio
    target_compile_options(Romerin PRIVATE /W4)  # Nivel de advertencias más alto

    # Otras configuraciones personalizadas, como cambiar el tipo de compilación a MT estático
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
