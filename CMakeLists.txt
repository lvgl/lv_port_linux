cmake_minimum_required(VERSION 3.10)
project(lvgl)

set(CMAKE_C_STANDARD 99)#C99 # lvgl officially support C99 and above
set(CMAKE_CXX_STANDARD 17)#C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(lvgl)
add_executable(main main.c mouse_cursor_icon.c)

find_package(PkgConfig REQUIRED) # We use the platform independent pkg-config to find all the libs
pkg_check_modules(LIBDRM REQUIRED libdrm)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_image REQUIRED SDL2_image)

target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(lvgl PRIVATE ${SDL2_INCLUDE_DIRS})
target_include_directories(lvgl PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})
target_include_directories(lvgl PRIVATE ${LIBDRM_INCLUDE_DIRS})

target_link_libraries(main PRIVATE lvgl) # Add '-static' if you want a standalone binary
target_link_libraries(main PRIVATE lvgl::examples)
target_link_libraries(main PRIVATE lvgl::demos)
target_link_libraries(main PRIVATE lvgl::thorvg)
target_link_libraries(main PRIVATE ${SDL2_LIBRARIES})
target_link_libraries(main PRIVATE ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(main PRIVATE ${LIBDRM_LIBRARIES})
target_link_libraries(main PRIVATE m)
target_link_libraries(main PRIVATE pthread)

add_custom_target(run COMMAND ${EXECUTABLE_OUTPUT_PATH}/main DEPENDS main)
