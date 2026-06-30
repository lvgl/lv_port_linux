cmake_minimum_required(VERSION 3.11)
project(lvgl)

option(WERROR "Treat warnings as errors for LVGL targets" OFF)

# Set policy to allow to run the target_link_libraries cmd on targets that are build
# in another directory.
# Currently, the linking is not handled by env_support/cmake/os.cmake
# This means that if a driver is enabled and it requires linking an external library
# it needs to be handled in the top-level project.

cmake_policy(SET CMP0079 NEW)

# Uncomment if the program needs debugging
#set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 99) # LVGL officially supports C99 and above
set(CMAKE_CXX_STANDARD 17) #C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")

option(CONFIG "Config to use leave empty for `lv_conf.defaults`" "default")

set(LV_CONF_DEFAULTS_PATH "${CMAKE_SOURCE_DIR}/lv_conf.defaults")
set(LV_CONF_DEFAULTS_OLD_PATH "${CMAKE_SOURCE_DIR}/lv_conf.defaults.old")

# If CONFIG is not default, copy the selected config to lv_conf.defaults
if(NOT CONFIG STREQUAL "default")
    set(CONFIG_FILE "${CMAKE_SOURCE_DIR}/configs/${CONFIG}.defaults")
    
    # Verify the config file exists
    if(NOT EXISTS "${CONFIG_FILE}")
        message(FATAL_ERROR "Config file not found: ${CONFIG_FILE}")
    endif()
    
    # Backup the current lv_conf.defaults to lv_conf.defaults.old
    if(EXISTS "${LV_CONF_DEFAULTS_PATH}")
        file(RENAME "${LV_CONF_DEFAULTS_PATH}" "${LV_CONF_DEFAULTS_OLD_PATH}")
        message(STATUS "Backed up lv_conf.defaults to lv_conf.defaults.old")
    endif()
    
    # Copy the config file to lv_conf.defaults
    configure_file("${CONFIG_FILE}" "${LV_CONF_DEFAULTS_PATH}" COPYONLY)
    message(STATUS "Copied configs/${CONFIG}.defaults to ${LV_CONF_DEFAULTS_PATH}")
    message(STATUS "Using config: configs/${CONFIG}.defaults")
else()
    if(NOT EXISTS "${LV_CONF_DEFAULTS_PATH}")
        message(FATAL_ERROR "Config file not found: ${LV_CONF_DEFAULTS_PATH}")
    endif()
    message(STATUS "Using default lv_conf.defaults")
    message(STATUS "Using config: ${LV_CONF_DEFAULTS_PATH}")
endif()


set(LV_BUILD_SET_CONFIG_OPTS ON CACHE BOOL
    "create CMAKE variables from lv_conf_internal.h" FORCE)

set(LV_BUILD_CONF_PATH
    "${CMAKE_BINARY_DIR}/lv_conf.h"
    CACHE PATH "path to lv_conf.h" FORCE)

set(LVGL_TEMPLATE_PATH "${CMAKE_SOURCE_DIR}/lvgl/lv_conf_template.h")
set(GENERATE_SCRIPT_PATH "${CMAKE_SOURCE_DIR}/lvgl/scripts/generate_lv_conf.py")

configure_file(${LVGL_TEMPLATE_PATH} ${CMAKE_BINARY_DIR}/lv_conf_template.h
               COPYONLY)
configure_file(${LV_CONF_DEFAULTS_PATH} ${CMAKE_BINARY_DIR}/lv_conf.defaults
               COPYONLY)
configure_file(${GENERATE_SCRIPT_PATH} ${CMAKE_BINARY_DIR}/generate_lv_conf.py
               COPYONLY)

execute_process(
  COMMAND
    ${Python3_EXECUTABLE} ${GENERATE_SCRIPT_PATH} --template
    ${LVGL_TEMPLATE_PATH} --defaults ${LV_CONF_DEFAULTS_PATH} --config
    ${LV_BUILD_CONF_PATH}
  RESULT_VARIABLE config_result
  OUTPUT_VARIABLE config_output
  ERROR_VARIABLE config_output)

if(NOT config_result EQUAL 0)
  message(FATAL_ERROR "Failed to generate lv_conf.h: ${config_output}")
endif()

message(STATUS "${LV_BUILD_CONF_PATH} generated successfully")

add_subdirectory(lvgl)

if (CONFIG_LV_USE_EVDEV)
    list(APPEND LV_LINUX_BACKEND_SRC src/lib/indev_backends/evdev.c)

endif()

if (CONFIG_LV_USE_LINUX_DRM)
    list(APPEND LV_LINUX_BACKEND_SRC src/lib/display_backends/drm.c)
endif()


if (CONFIG_LV_USE_SDL)
    list(APPEND LV_LINUX_BACKEND_SRC src/lib/display_backends/sdl.c)
endif()


if (CONFIG_LV_USE_WAYLAND)
    list(APPEND LV_LINUX_BACKEND_SRC src/lib/display_backends/wayland.c ${WAYLAND_PROTOCOLS_SRC})
endif()

if (CONFIG_LV_USE_X11)
    list(APPEND LV_LINUX_BACKEND_SRC src/lib/display_backends/x11.c)
endif()

if (CONFIG_LV_USE_LINUX_FBDEV)
    list(APPEND LV_LINUX_BACKEND_SRC src/lib/display_backends/fbdev.c)
endif()

if (CONFIG_LV_USE_GLFW)
    list(APPEND LV_LINUX_BACKEND_SRC src/lib/display_backends/glfw3.c)
endif()

file(GLOB LV_LINUX_SRC src/lib/*.c)
set(LV_LINUX_INC src/lib)


add_library(lvgl_linux STATIC ${LV_LINUX_SRC} ${LV_LINUX_BACKEND_SRC})

# If LVGL is configured to use LV_CONF_PATH or Kconfig
# Set the exactly the same definitions on the lvgl_linux target
set_target_properties(lvgl_linux PROPERTIES COMPILE_DEFINITIONS "${LVGL_COMPILER_DEFINES}")
target_include_directories(lvgl_linux PUBLIC
    ${LV_LINUX_INC} ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/lib ${LVGL_CONF_INC_DIR})

add_executable(lvglsim src/main.c ${LV_LINUX_SRC} ${LV_LINUX_BACKEND_SRC})
target_link_libraries(lvglsim lvgl_linux lvgl lvgl::demos lvgl::examples)

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
    ARCHIVE DESTINATION lib
)

add_custom_target(run COMMAND ${EXECUTABLE_OUTPUT_PATH}/lvglsim DEPENDS lvglsim)
