# Requiere una versión mínima de CMake
cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(RomerinGen)

# Establecer la versión del estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Añadir el ejecutable (nombre del archivo de salida y el archivo de código fuente principal)
set(SOURCE_ROMERIN
    src/Robot/BrazoRobot/Robot.cpp
    src/Robot/Control/controlPatas.cpp
    src/Robot/Kinematics/romkin.cpp
    src/Robot/RomJoints/modulesimulator.cpp
    src/Robot/RomJoints/RomJointsA.cpp
    src/Robot/RomJoints/RomerinMsgA.cpp
)

add_executable(Romerin ${SOURCE_ROMERIN})


# Si necesitas incluir más archivos fuente o cabeceras
# add_executable(MiEjecutable main.cpp otraFuente.cpp)

# Incluir directorios adicionales donde estén los archivos de cabecera (*.h)
 include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
 include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/Robot")

# Librerías adicionales (si utilizas librerías externas)
 
 find_package(glfw3 3.3 REQUIRED)
 find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network)
 set(libactuator "${CMAKE_CURRENT_SOURCE_DIR}/bin/mujoco_plugin/libactuator.so")
 set(libelasticity "${CMAKE_CURRENT_SOURCE_DIR}/bin/mujoco_plugin/libelasticity.so")
 set(libsdf "${CMAKE_CURRENT_SOURCE_DIR}/bin/mujoco_plugin/libsdf.so")
 set(libsensor "${CMAKE_CURRENT_SOURCE_DIR}/bin/mujoco_plugin/libsensor.so")
 set(libmujoco "${CMAKE_CURRENT_SOURCE_DIR}/lib/libmujoco.so")
 set(libmujoco323 "${CMAKE_CURRENT_SOURCE_DIR}/lib/libmujoco.so.3.2.3")

 target_link_libraries(Romerin 
    "${libactuator}" 
    "${libelasticity}" 
    "${libsdf}" 
    "${libsensor}" 
    "${libmujoco}" 
    "${libmujoco323}"
    glfw
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
