cmake_minimum_required(VERSION 3.20)

project(taxi_app VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-4.0)

add_executable(taxi_app
    src/main.cpp
    src/bodies.cpp
    include/header.hpp
)

set_property(TARGET taxi_app PROPERTY CXX_STANDARD 20)
set_property(TARGET taxi_app PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(taxi_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(taxi_app PRIVATE PkgConfig::GTKMM)