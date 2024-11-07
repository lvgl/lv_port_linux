cmake_minimum_required(VERSION 3.10)
project(lvgl)

foreach(BACKEND_NAME "SDL" "LINUX_DRM" "LINUX_FBDEV" "X11" "WAYLAND" "OPENGLES")
    execute_process(COMMAND "scripts/backend_conf.sh" ${BACKEND_NAME} OUTPUT_VARIABLE IS_BACKEND_ENABLED)
    set("LV_USE_${BACKEND_NAME}" ${IS_BACKEND_ENABLED})
endforeach()

# Uncomment if the program needs debugging
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")

set(CMAKE_C_STANDARD 99) # LVGL officially supports C99 and above
set(CMAKE_CXX_STANDARD 17) #C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
add_subdirectory(lvgl)

target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)

file(GLOB SIM_SRC src/*.c)

if (LV_USE_LINUX_DRM)

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBDRM REQUIRED libdrm)

    target_include_directories(lvgl PUBLIC ${LIBDRM_INCLUDE_DIRS})
    add_executable(lvglsim ${SIM_SRC} src/backends/drm.c)
    target_link_libraries(lvglsim lvgl lvgl::examples lvgl::demos lvgl::thorvg ${LIBDRM_LIBRARIES} m pthread)

elseif (LV_USE_SDL)

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_image REQUIRED SDL2_image)

    target_include_directories(lvgl PRIVATE ${SDL2_INCLUDE_DIRS})
    target_include_directories(lvgl PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})
    add_executable(lvglsim ${SIM_SRC} src/backends/sdl.c)
    target_link_libraries(lvglsim lvgl lvgl::examples lvgl::demos lvgl::thorvg ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} m pthread)

elseif (LV_USE_WAYLAND)

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(wayland-client REQUIRED wayland-client)
    pkg_check_modules(wayland-cursor REQUIRED wayland-cursor)
    pkg_check_modules(xkbcommon REQUIRED xkbcommon)

    # Wayland protocols
    pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols>=1.25)
    pkg_get_variable(WAYLAND_PROTOCOLS_BASE wayland-protocols pkgdatadir)

    execute_process(COMMAND "scripts/gen_wl_protocols.sh" OUTPUT_VARIABLE WAYLAND_PROTOCOLS_SRC)

    target_include_directories(lvgl PRIVATE ${PROJECT_SOURCE_DIR}
                                            "${PROJECT_SOURCE_DIR}/wl_protocols")
    add_executable(lvglsim ${SIM_SRC} ${WAYLAND_PROTOCOLS_SRC} src/backends/wayland.c)
    target_compile_definitions(lvglsim PRIVATE LV_CONF_INCLUDE_SIMPLE)
    target_link_libraries(lvglsim lvgl lvgl::examples lvgl::demos lvgl::thorvg m
        wayland-client wayland-cursor xkbcommon)

elseif (LV_USE_LINUX_FBDEV)

    # No specific build steps required for FBDEV
    add_executable(lvglsim ${SIM_SRC} src/backends/fbdev.c)
    target_link_libraries(lvglsim lvgl lvgl::examples lvgl::demos lvgl::thorvg m pthread)

elseif (LV_USE_OPENGLES)

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW3 REQUIRED glfw3)
    pkg_check_modules(GLEW REQUIRED glew)

    add_executable(lvglsim ${SIM_SRC} src/backends/glfw3.c)
    target_link_libraries(lvglsim lvgl lvgl::examples lvgl::demos lvgl::thorvg
        m pthread ${GLFW3_LIBRARIES} ${GLEW_LIBRARIES})

endif()

add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/lvglsim DEPENDS lvglsim)
