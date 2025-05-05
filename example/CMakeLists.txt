# This is used to test the installation process

cmake_minimum_required(VERSION 3.10)
project(lvgl_example)

set(CMAKE_C_STANDARD 99) # LVGL officially supports C99 and above
set(CMAKE_CXX_STANDARD 17) #C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Uncomment if the program needs debugging
set(CMAKE_BUILD_TYPE Debug)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LVGL REQUIRED lvgl)

add_executable(main main.c)
target_include_directories(main PUBLIC ${LVGL_INCLUDE_DIRS})
target_link_libraries(main PUBLIC ${LVGL_LIBRARIES} m)

add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/main DEPENDS main)
