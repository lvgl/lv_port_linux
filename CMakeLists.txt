cmake_minimum_required(VERSION 3.11)
project(lvgl)

option(WERROR "Treat warnings as errors for LVGL targets" OFF)
option(ASAN "Enable Sanitizer Options" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")

find_package(Python3 REQUIRED COMPONENTS Interpreter)

if(ASAN)
    message(STATUS "Enabling Sanitizer")
    add_compile_options(-fsanitize=address,undefined,null)
    add_link_options(-fsanitize=address,undefined,null,leak)
endif()

set(LV_BUILD_USE_KCONFIG ON CACHE BOOL "use kconfig" FORCE)
set(DOTCONFIG "${CMAKE_CURRENT_LIST_DIR}/.config")
set(DEFCONFIG "${CMAKE_CURRENT_LIST_DIR}/configs/get_started.defconfig")

if(EXISTS "${DOTCONFIG}")
    message(STATUS "Using .config found to build LVGL")
    set(LV_BUILD_DOTCONFIG_PATH "${DOTCONFIG}" CACHE PATH "path to .config" FORCE)
else()
    message(STATUS "No .config found, using ${DEFCONFIG} to build LVGL")
    set(LV_BUILD_DEFCONFIG_PATH "${DEFCONFIG}" CACHE PATH "path to defconfig" FORCE)
endif()

add_subdirectory(lvgl)

file(GLOB LV_LINUX_SRC src/lib/*.c)
set(LV_LINUX_INC src/lib)
set(LV_LINUX_BACKEND_SRC)
set(DISPLAY_BACKENDS src/lib/display_backends)
set(INDEV_BACKENDS src/lib/indev_backends)

function(lv_linux_add_backend_if option dir file)
    if(${option})
        list(APPEND LV_LINUX_BACKEND_SRC ${dir}/${file})
        set(LV_LINUX_BACKEND_SRC ${LV_LINUX_BACKEND_SRC} PARENT_SCOPE)
    endif()
endfunction()

lv_linux_add_backend_if(CONFIG_LV_USE_WAYLAND     ${DISPLAY_BACKENDS} wayland.c)
lv_linux_add_backend_if(CONFIG_LV_USE_LINUX_DRM   ${DISPLAY_BACKENDS} drm.c)
lv_linux_add_backend_if(CONFIG_LV_USE_LINUX_FBDEV ${DISPLAY_BACKENDS} fbdev.c)
lv_linux_add_backend_if(CONFIG_LV_USE_SDL         ${DISPLAY_BACKENDS} sdl.c)
lv_linux_add_backend_if(CONFIG_LV_USE_X11         ${DISPLAY_BACKENDS} x11.c)
lv_linux_add_backend_if(CONFIG_LV_USE_GLFW        ${DISPLAY_BACKENDS} glfw3.c)
lv_linux_add_backend_if(CONFIG_LV_USE_EVDEV       ${INDEV_BACKENDS}   evdev.c)

# Library type is determined by BUILD_SHARED_LIBS (shared if ON, static if OFF)
add_library(lvgl_linux ${LV_LINUX_SRC} ${LV_LINUX_BACKEND_SRC})

set(LIBS lvgl::lvgl)

if(CONFIG_LV_BUILD_DEMOS)
list(APPEND LIBS lvgl::demos)
endif()

if(CONFIG_LV_BUILD_EXAMPLES)
list(APPEND LIBS lvgl::examples)
endif()

target_link_libraries(lvgl_linux PUBLIC ${LIBS})
add_executable(lvglsim src/main.c)
target_link_libraries(lvglsim PUBLIC lvgl_linux)

if(WERROR)
    target_compile_options(lvglsim PRIVATE -Werror)
    target_compile_options(lvgl PRIVATE -Werror)
    target_compile_options(lvgl_linux PRIVATE -Werror)
endif()


# Install the lvgl_linux library and its headers
install(DIRECTORY src/lib/
    DESTINATION include/lvgl
    FILES_MATCHING
    PATTERN "backends*" EXCLUDE
    PATTERN "*.h")

install(TARGETS lvgl_linux
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

add_custom_target(run COMMAND ${EXECUTABLE_OUTPUT_PATH}/lvglsim DEPENDS lvglsim)
