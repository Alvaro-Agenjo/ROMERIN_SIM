cmake_minimum_required(VERSION 3.10)
project(mov_try)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Añadir directorios de inclusión
include_directories(include)

# Archivos fuente
add_executable(mov_try
    movimientos.cpp
    include/RomerinMessage.h
    modulo.h
    # src/modulecontroller.cpp  # Descomenta si lo usas
)

# Librerías necesarias para sockets y utilidades POSIX
target_link_libraries(mov_try
    pthread
)
