cmake_minimum_required(VERSION 3.3)
project(spelfysik_slutuppgift)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wpedantic")

set(SOURCE_FILES main.cpp Vector3d.h Vector3f.h Graphics.cpp Graphics.h Simulation.cpp Simulation.h SimulationFloat.cpp SimulationFloat.h icosphere.cpp)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

set(SFML_ROOT "${CMAKE_CURRENT_LIST_DIR}/SFML-2.3.2")
set(EIGEN3_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(SFML 2.3 REQUIRED COMPONENTS system window graphics)
find_package(Eigen3 3.1.2)
if(SFML_FOUND AND EIGEN3_FOUND)
	#include_directories(${SFML_INCLUDE_DIR} ${PROJECT_SOURCE_DIR})
	include_directories(${EIGEN3_INCLUDE_DIR} ${SFML_INCLUDE_DIR})
	target_link_libraries(${CMAKE_PROJECT_NAME} libopengl32.a glu32 ${SFML_LIBRARIES})
else()
	message(WARNING "SFML and/or EIGEN not found")
endif()

