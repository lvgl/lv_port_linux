cmake_minimum_required(VERSION 3.10)
project(lvgl)

# Set policy to allow to run the target_link_libraries cmd on targets that are build
# in another directory.
# Currently, the linking is not handled by env_support/cmake/os.cmake
# This means that if a driver is enabled and it requires linking an external library
# it needs to be handled in the top-level project.

cmake_policy(SET CMP0079 NEW)

# Uncomment if the program needs debugging
#set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_STANDARD 99) # LVGL officially supports C99 and above
set(CMAKE_CXX_STANDARD 17) #C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")

set(LV_BUILD_SET_CONFIG_OPTS ON CACHE BOOL
    "create CMAKE variables from lv_conf_internal.h" FORCE)

add_subdirectory(lvgl)

if (CONFIG_LV_USE_EVDEV)
    message("Including EVDEV support")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(EVDEV REQUIRED libevdev)

    list(APPEND PKG_CONFIG_LIB ${EVDEV_LIBRARIES})
    list(APPEND PKG_CONFIG_INC ${EVDEV_INCLUDE_DIRS})
    list(APPEND LV_LINUX_BACKEND_SRC src/lib/indev_backends/evdev.c)

endif()

if (CONFIG_LV_USE_LINUX_DRM)

    message("Including DRM support")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBDRM REQUIRED libdrm)

    list(APPEND PKG_CONFIG_LIB ${LIBDRM_LIBRARIES})
    list(APPEND PKG_CONFIG_INC ${LIBDRM_INCLUDE_DIRS})
    list(APPEND LV_LINUX_BACKEND_SRC src/lib/display_backends/drm.c)

endif()

if (CONFIG_LV_USE_LINUX_DRM_GBM_BUFFERS)

    message("Including GBM support")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBGBM REQUIRED gbm)

    list(APPEND PKG_CONFIG_LIB ${LIBGBM_LIBRARIES})
    list(APPEND PKG_CONFIG_INC ${LIBGBM_INCLUDE_DIRS})

endif()

if (CONFIG_LV_USE_LIBINPUT)
    message("Including libinput support")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBINPUT REQUIRED libinput)

    list(APPEND PKG_CONFIG_LIB ${LIBINPUT_LIBRARIES})
    list(APPEND PKG_CONFIG_INC ${LIBINPUT_INCLUDE_DIRS})

endif()

if (CONFIG_LV_USE_FREETYPE)
    message("Including Freetype support")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBFREETYPE REQUIRED freetype2)

    list(APPEND PKG_CONFIG_LIB ${LIBFREETYPE_LIBRARIES})
    list(APPEND PKG_CONFIG_INC ${LIBFREETYPE_INCLUDE_DIRS})

endif()

if (CONFIG_LV_USE_SDL)

    message("Including SDL2 support")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

    list(APPEND PKG_CONFIG_LIB ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
    list(APPEND PKG_CONFIG_INC ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

    list(APPEND LV_LINUX_BACKEND_SRC src/lib/display_backends/sdl.c)
endif()


if (CONFIG_LV_USE_WAYLAND)

    message("Including Wayland support")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(WAYLAND_CLIENT REQUIRED wayland-client)
    pkg_check_modules(WAYLAND_CURSOR REQUIRED wayland-cursor)
    pkg_check_modules(XKBCOMMON REQUIRED xkbcommon)

    list(APPEND PKG_CONFIG_LIB ${WAYLAND_CLIENT_LIBRARIES})
    list(APPEND PKG_CONFIG_LIB ${WAYLAND_CURSOR_LIBRARIES})
    list(APPEND PKG_CONFIG_LIB ${XKBCOMMON_LIBRARIES})

    # Wayland protocols
    pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols>=1.25)
    pkg_get_variable(WAYLAND_PROTOCOLS_BASE wayland-protocols pkgdatadir)

    if(DEFINED ENV{SDKTARGETSYSROOT} AND DEFINED ENV{SYSROOT})
        message(FATAL_ERROR "Both SDKTARGETSYSROOT and SYSROOT are set. Please set only one.")
    endif()
    
    if(DEFINED ENV{SDKTARGETSYSROOT})
        set(PROTOCOL_ROOT "$ENV{SDKTARGETSYSROOT}/usr/share/wayland-protocols")
    elseif(DEFINED ENV{SYSROOT})
        set(PROTOCOL_ROOT "$ENV{SYSROOT}/usr/share/wayland-protocols")
    else()
        set(PROTOCOL_ROOT "/usr/share/wayland-protocols")
    endif()
    
    if(NOT EXISTS ${PROTOCOL_ROOT})
        message(FATAL_ERROR "Wayland protocols not found at ${PROTOCOL_ROOT}")
    endif()
    
    set(PROTOCOLS_DIR "${CMAKE_CURRENT_BINARY_DIR}/protocols")
    file(MAKE_DIRECTORY ${PROTOCOLS_DIR})
    
    set(WAYLAND_PROTOCOLS_SRC "")
    
    # Generate xdg-shell protocol (always)
    set(XDG_SHELL_XML "${PROTOCOL_ROOT}/stable/xdg-shell/xdg-shell.xml")
    set(XDG_SHELL_HEADER "${PROTOCOLS_DIR}/wayland_xdg_shell.h")
    set(XDG_SHELL_SOURCE "${PROTOCOLS_DIR}/wayland_xdg_shell.c")
    
    if(NOT EXISTS ${XDG_SHELL_HEADER} OR NOT EXISTS ${XDG_SHELL_SOURCE})
        execute_process(COMMAND wayland-scanner client-header ${XDG_SHELL_XML} ${XDG_SHELL_HEADER})
        execute_process(COMMAND wayland-scanner private-code ${XDG_SHELL_XML} ${XDG_SHELL_SOURCE})
    endif()
    list(APPEND WAYLAND_PROTOCOLS_SRC ${XDG_SHELL_SOURCE})
    
    # Generate dmabuf protocol (if config is set)
    if(CONFIG_LV_WAYLAND_USE_DMABUF)
        set(DMABUF_XML "${PROTOCOL_ROOT}/stable/linux-dmabuf/linux-dmabuf-v1.xml")
        set(DMABUF_HEADER "${PROTOCOLS_DIR}/wayland_linux_dmabuf.h")
        set(DMABUF_SOURCE "${PROTOCOLS_DIR}/wayland_linux_dmabuf.c")
        
        if(NOT EXISTS ${DMABUF_HEADER} OR NOT EXISTS ${DMABUF_SOURCE})
            execute_process(COMMAND wayland-scanner client-header ${DMABUF_XML} ${DMABUF_HEADER})
            execute_process(COMMAND wayland-scanner private-code ${DMABUF_XML} ${DMABUF_SOURCE})
        endif()
        list(APPEND WAYLAND_PROTOCOLS_SRC ${DMABUF_SOURCE})
    endif()
    
    list(APPEND PKG_CONFIG_INC ${PROTOCOLS_DIR})
    list(APPEND LV_LINUX_BACKEND_SRC src/lib/display_backends/wayland.c ${WAYLAND_PROTOCOLS_SRC})

endif()

if (CONFIG_LV_USE_X11)

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(X11 REQUIRED x11)

    message("Including X11 support")

    list(APPEND PKG_CONFIG_INC ${X11_INCLUDE_DIRS})
    list(APPEND PKG_CONFIG_LIB ${X11_LIBRARIES})
    list(APPEND LV_LINUX_BACKEND_SRC src/lib/display_backends/x11.c)

endif()

if (CONFIG_LV_USE_LINUX_FBDEV)

    # FBDEV has no dependencies
    message("Including FBDEV support")
    list(APPEND LV_LINUX_BACKEND_SRC src/lib/display_backends/fbdev.c)

endif()

if (CONFIG_LV_USE_OPENGLES)

    message("Including OPENGLES support")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW3 REQUIRED glfw3)
    pkg_check_modules(GLEW REQUIRED glew)

    list(APPEND PKG_CONFIG_LIB ${GLFW3_LIBRARIES})
    list(APPEND PKG_CONFIG_LIB ${GLEW_LIBRARIES})
    list(APPEND LV_LINUX_BACKEND_SRC src/lib/display_backends/glfw3.c)

endif()

file(GLOB LV_LINUX_SRC src/lib/*.c)
set(LV_LINUX_INC src/lib)

target_include_directories(lvgl PUBLIC ${PKG_CONFIG_INC})

add_library(lvgl_linux STATIC ${LV_LINUX_SRC} ${LV_LINUX_BACKEND_SRC})

# If LVGL is configured to use LV_CONF_PATH or Kconfig
# Set the exactly the same definitions on the lvgl_linux target
set_target_properties(lvgl_linux PROPERTIES COMPILE_DEFINITIONS "${LVGL_COMPILER_DEFINES}")
target_include_directories(lvgl_linux PUBLIC
    ${LV_LINUX_INC} ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/lib ${LVGL_CONF_INC_DIR})

# Link LVGL with external dependencies - Modern CMake/CMP0079 allows this
target_link_libraries(lvgl PUBLIC ${PKG_CONFIG_LIB} m pthread)

add_executable(lvglsim src/main.c ${LV_LINUX_SRC} ${LV_LINUX_BACKEND_SRC})
target_link_libraries(lvglsim lvgl_linux lvgl)

if (CONFIG_LV_USE_DRAW_G2D)
    target_link_libraries(lvglsim g2d)
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
