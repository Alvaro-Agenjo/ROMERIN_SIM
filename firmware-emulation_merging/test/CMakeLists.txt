cmake_minimum_required(VERSION 3.5)

project(RomKinTest)
set(CMAKE_CXX_STANDARD 17)

find_package(Eigen3 REQUIRED)


set(INCLUDE_DIRS PUBLIC ${EIGEN3_INCLUDE_DIR})

set(SRC_FILES ../shared/romkin.cpp
              ../shared/romkin.h
              test_rom_kin.cpp
)
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} Eigen3::Eigen)
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})