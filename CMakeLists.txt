cmake_minimum_required(VERSION 3.10)
project(lvgl)

# Please set the wanted option to 'ON'. By default, FBDEV is used
# be sure to also enable the selected driver in lv_conf.h

option(LV_USE_WAYLAND "Use the wayland client backend" OFF)
option(LV_USE_SDL "Use the SDL backend" OFF)
option(LV_USE_DRM "Use the libdrm backend" OFF)
option(DEBUG_MODE "Disable optimizations and enable debugging" OFF)

set(CMAKE_C_STANDARD 99) # LVGL officially supports C99 and above
set(CMAKE_CXX_STANDARD 17) #C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (DEBUG_MODE)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")

endif(DEBUG_MODE)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_subdirectory(lvgl)
target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR})

if (LV_USE_DRM)

    include(${CMAKE_CURRENT_LIST_DIR}/lvgl/tests/FindLibDRM.cmake)
    include_directories(${Libdrm_INCLUDE_DIRS})
    add_executable(lvglsim main.c mouse_cursor_icon.c)
    target_link_libraries(lvglsim lvgl lvgl::examples lvgl::demos lvgl::thorvg ${Libdrm_LIBRARIES} m pthread)

elseif (LV_USE_SDL)

    find_package(SDL2)
    find_package(SDL2_image)
    include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
    add_executable(lvglsim main.c mouse_cursor_icon.c)
    target_link_libraries(lvglsim lvgl lvgl::examples lvgl::demos lvgl::thorvg ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m pthread)

elseif (LV_USE_WAYLAND)

    find_package(PkgConfig)
    pkg_check_modules(wayland-client REQUIRED wayland-client)
    pkg_check_modules(wayland-cursor REQUIRED wayland-cursor)
    pkg_check_modules(xkbcommon REQUIRED xkbcommon)

    # Wayland protocols
    set(WAYLAND_PROTOCOLS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/protocols")
    find_program(WAYLAND_SCANNER_EXECUTABLE NAMES wayland-scanner)
    pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols>=1.25)
    pkg_get_variable(WAYLAND_PROTOCOLS_BASE wayland-protocols pkgdatadir)

    macro(wayland_generate protocol_xml_file output_dir target)
        get_filename_component(output_file_base ${protocol_xml_file} NAME_WE)
        set(output_file_noext "${output_dir}/wayland-${output_file_base}-client-protocol")
        add_custom_command(OUTPUT "${output_file_noext}.h"
            COMMAND "${WAYLAND_SCANNER_EXECUTABLE}" client-header "${protocol_xml_file}" "${output_file_noext}.h"
            DEPENDS "${protocol_xml_file}"
            VERBATIM)

        add_custom_command(OUTPUT "${output_file_noext}.c"
            COMMAND "${WAYLAND_SCANNER_EXECUTABLE}" private-code "${protocol_xml_file}" "${output_file_noext}.c"
            DEPENDS "${protocol_xml_file}"
            VERBATIM)

        if(NOT EXISTS ${protocol_xml_file})
            message("Protocol XML file not found: " ${protocol_xml_file})
        else()
            set_property(TARGET ${target} APPEND PROPERTY SOURCES  "${output_file_noext}.h" "${output_file_noext}.c")
        endif()

    endmacro()

    file(MAKE_DIRECTORY ${WAYLAND_PROTOCOLS_DIR})

    add_custom_target(generate_protocols ALL)

    wayland_generate("${WAYLAND_PROTOCOLS_BASE}/stable/xdg-shell/xdg-shell.xml" ${WAYLAND_PROTOCOLS_DIR} generate_protocols)

    target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR} ${WAYLAND_PROTOCOLS_DIR})

    # TODO Add option to still be able to build wl_shell
    add_executable(lvglsim main.c ${WAYLAND_PROTOCOLS_DIR}/wayland-xdg-shell-client-protocol.c mouse_cursor_icon.c)

    target_compile_definitions(lvglsim PRIVATE LV_CONF_INCLUDE_SIMPLE)
    target_link_libraries(lvglsim lvgl lvgl::examples lvgl::demos lvgl::thorvg m wayland-client xkbcommon wayland-cursor)

else()

    # No specific build steps required for FBDEV
    add_executable(lvglsim main.c mouse_cursor_icon.c)
    target_link_libraries(lvglsim lvgl lvgl::examples lvgl::demos lvgl::thorvg m pthread)

endif()

add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/lvglsim DEPENDS lvglsim)
