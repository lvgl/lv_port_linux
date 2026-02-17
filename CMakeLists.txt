cmake_minimum_required(VERSION 3.11)
project(lvgl)

option(WERROR "Treat warnings as errors for LVGL targets" OFF)
option(ASAN "Enable Sanitizer Options" OFF)

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

if(ASAN)
    message(STATUS "Enabling Sanitizer")
    add_compile_options(-fsanitize=address,undefined,null)
    add_link_options(-fsanitize=address,undefined,null,leak)
endif()

set(CONFIG "default" CACHE STRING "Config name to use. Leave empty for default `lv_conf.defaults`")

set(LV_CONF_DEFAULTS_PATH "${CMAKE_SOURCE_DIR}/lv_conf.defaults")

# If CONFIG is not default, update LV_CONF_DEFAULTS_PATH to the selected config
if(NOT CONFIG STREQUAL "default")
    set(LV_CONF_DEFAULTS_PATH "${CMAKE_SOURCE_DIR}/configs/${CONFIG}.defaults")
endif()

if(NOT EXISTS "${LV_CONF_DEFAULTS_PATH}")
    message(FATAL_ERROR "Config file not found: ${LV_CONF_DEFAULTS_PATH}")
endif()
message(STATUS "Using config: ${LV_CONF_DEFAULTS_PATH}")

set(LV_BUILD_SET_CONFIG_OPTS ON CACHE BOOL
    "create CMAKE variables from lv_conf_internal.h" FORCE)

set(LV_BUILD_CONF_PATH
    "${CMAKE_BINARY_DIR}/lv_conf.h"
    CACHE PATH "path to lv_conf.h" FORCE)

set(LVGL_TEMPLATE_PATH "${CMAKE_SOURCE_DIR}/lvgl/lv_conf_template.h")
set(GENERATE_SCRIPT_PATH "${CMAKE_SOURCE_DIR}/lvgl/scripts/generate_lv_conf.py")

set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${LVGL_TEMPLATE_PATH})
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${LV_CONF_DEFAULTS_PATH})
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${GENERATE_SCRIPT_PATH})

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

if (CONFIG_LV_USE_LINUX_DRM_GBM_BUFFERS OR CONFIG_LV_LINUX_DRM_USE_EGL)

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

if (CONFIG_LV_USE_SDL OR CONFIG_LV_USE_DRAW_SDL)

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
    pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols>=1.20)
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

    # Use public-code for shared libs, private-code for static libs
    if(BUILD_SHARED_LIBS)
        set(WAYLAND_SCANNER_CODE_MODE "public-code")
    else()
        set(WAYLAND_SCANNER_CODE_MODE "private-code")
    endif()

    # Generate xdg-shell protocol (always)
    set(XDG_SHELL_XML "${PROTOCOL_ROOT}/stable/xdg-shell/xdg-shell.xml")
    set(XDG_SHELL_HEADER "${PROTOCOLS_DIR}/wayland_xdg_shell.h")
    set(XDG_SHELL_SOURCE "${PROTOCOLS_DIR}/wayland_xdg_shell.c")
    
    if(NOT EXISTS ${XDG_SHELL_HEADER} OR NOT EXISTS ${XDG_SHELL_SOURCE})
        execute_process(COMMAND wayland-scanner client-header ${XDG_SHELL_XML} ${XDG_SHELL_HEADER})
        execute_process(COMMAND wayland-scanner ${WAYLAND_SCANNER_CODE_MODE} ${XDG_SHELL_XML} ${XDG_SHELL_SOURCE})
    endif()
    list(APPEND WAYLAND_PROTOCOLS_SRC ${XDG_SHELL_SOURCE})
    
    # Generate dmabuf protocol (if config is set)
    if(CONFIG_LV_WAYLAND_USE_G2D)
        set(DMABUF_XML "${PROTOCOL_ROOT}/stable/linux-dmabuf/linux-dmabuf-v1.xml")
        set(DMABUF_HEADER "${PROTOCOLS_DIR}/wayland_linux_dmabuf.h")
        set(DMABUF_SOURCE "${PROTOCOLS_DIR}/wayland_linux_dmabuf.c")
        
        if(NOT EXISTS ${DMABUF_HEADER} OR NOT EXISTS ${DMABUF_SOURCE})
            execute_process(COMMAND wayland-scanner client-header ${DMABUF_XML} ${DMABUF_HEADER})
            execute_process(COMMAND wayland-scanner ${WAYLAND_SCANNER_CODE_MODE} ${DMABUF_XML} ${DMABUF_SOURCE})
        endif()
        list(APPEND WAYLAND_PROTOCOLS_SRC ${DMABUF_SOURCE})
    endif()

    if(CONFIG_LV_WAYLAND_USE_EGL)
        message("Including WAYLAND EGL support")
        pkg_check_modules(WAYLAND_EGL REQUIRED wayland-egl)
        list(APPEND PKG_CONFIG_LIB ${WAYLAND_EGL_LIBRARIES})
        list(APPEND PKG_CONFIG_INC ${WAYLAND_EGL_INCLUDE_DIRS})
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

if (CONFIG_LV_USE_GLFW)

    message("Including GLFW support")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GLFW3 REQUIRED glfw3)
    pkg_check_modules(GLEW REQUIRED glew)

    list(APPEND PKG_CONFIG_LIB ${GLFW3_LIBRARIES})
    list(APPEND PKG_CONFIG_LIB ${GLEW_LIBRARIES})
    list(APPEND LV_LINUX_BACKEND_SRC src/lib/display_backends/glfw3.c)

endif()

if (CONFIG_LV_USE_DRAW_G2D)
    message("Including G2D support")
    find_library(G2D_LIBRARY NAMES g2d)
    list(APPEND PKG_CONFIG_LIB ${G2D_LIBRARY})
endif()

if (CONFIG_LV_USE_GLTF)
    message(STATUS "Including with GLTF support")
    include(FetchContent)
  
    FetchContent_Declare(
      fastgltf
      GIT_REPOSITORY https://github.com/spnda/fastgltf
      GIT_TAG f04052ebd8d157c7b6e8fc3dd9f1ed83df99f3e5)
  
    set(FASTGLTF_ENABLE_DEPRECATED_EXT
        ON
        CACHE BOOL "" FORCE)
  
    set(FASTGLTF_DIFFUSE_TRANSMISSION_SUPPORT
        ON
        CACHE BOOL "" FORCE)
  
    FetchContent_MakeAvailable(fastgltf)
  
    set(WEBP_BUILD_ANIM_UTILS
        OFF
        CACHE BOOL "" FORCE)
    set(WEBP_BUILD_CWEBP
        OFF
        CACHE BOOL "" FORCE)
    set(WEBP_BUILD_DWEBP
        OFF
        CACHE BOOL "" FORCE)
    set(WEBP_BUILD_GIF2WEBP
        OFF
        CACHE BOOL "" FORCE)
    set(WEBP_BUILD_IMG2WEBP
        OFF
        CACHE BOOL "" FORCE)
    set(WEBP_BUILD_VWEBP
        OFF
        CACHE BOOL "" FORCE)
    set(WEBP_BUILD_WEBPINFO
        OFF
        CACHE BOOL "" FORCE)
    set(WEBP_BUILD_WEBPMUX
        OFF
        CACHE BOOL "" FORCE)
    set(WEBP_BUILD_EXTRAS
        OFF
        CACHE BOOL "" FORCE)
  
    FetchContent_Declare(
      webp
      GIT_REPOSITORY https://github.com/webmproject/libwebp
      GIT_TAG fa6f56496a442eed59b103250021e4b14ebf1427)
  
    FetchContent_MakeAvailable(webp)
  
    list(APPEND PKG_CONFIG_LIB fastgltf webp)
endif()

if (CONFIG_LV_USE_GSTREAMER)

    message("Including GSTREAMER support")
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
    pkg_check_modules(GSTREAMER_VIDEO REQUIRED gstreamer-video-1.0)
    pkg_check_modules(GSTREAMER_APP REQUIRED gstreamer-app-1.0)

    list(APPEND PKG_CONFIG_LIB ${GSTREAMER_VIDEO_LIBRARIES} ${GSTREAMER_APP_LIBRARIES})
    list(APPEND PKG_CONFIG_INC ${GSTREAMER_VIDEO_INCLUDE_DIRS} ${GSTREAMER_APP_INCLUDE_DIRS})
endif()

if (CONFIG_LV_USE_FFMPEG)

    message("Including FFMPEG support")
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(AVFORMAT REQUIRED libavformat)
    pkg_check_modules(AVCODEC  REQUIRED libavcodec)
    pkg_check_modules(AVUTIL   REQUIRED libavutil)
    pkg_check_modules(SWSCALE  REQUIRED libswscale)

    list(APPEND PKG_CONFIG_LIB
        ${AVFORMAT_LIBRARIES}
        ${AVCODEC_LIBRARIES}
        ${AVUTIL_LIBRARIES}
        ${SWSCALE_LIBRARIES}
    )

    list(APPEND PKG_CONFIG_INC
        ${AVFORMAT_INCLUDE_DIRS}
        ${AVCODEC_INCLUDE_DIRS}
        ${AVUTIL_INCLUDE_DIRS}
        ${SWSCALE_INCLUDE_DIRS}
    )

endif()

# Check for external demos
if(CONFIG_LV_USE_DEMO_FLEX_LAYOUT
   OR CONFIG_LV_USE_DEMO_MULTILANG
   OR CONFIG_LV_USE_DEMO_TRANSFORM
   OR CONFIG_LV_USE_DEMO_SCROLL
   OR CONFIG_LV_USE_DEMO_EBIKE
   OR CONFIG_LV_USE_DEMO_HIGH_RES
   OR CONFIG_LV_USE_DEMO_SMARTWATCH)

    set(USE_EXTERNAL_DEMOS ON)
else()
    set(USE_EXTERNAL_DEMOS OFF)
endif()

if(USE_EXTERNAL_DEMOS)
    message("Including external demos")
    include(FetchContent)
    FetchContent_Declare(lv_demos_ext GIT_REPOSITORY https://github.com/lvgl/lv_demos)
    FetchContent_MakeAvailable(lv_demos_ext)
    list(APPEND PKG_CONFIG_LIB lv_demos_ext)
endif()


file(GLOB LV_LINUX_SRC src/lib/*.c)
set(LV_LINUX_INC src/lib)

target_include_directories(lvgl PUBLIC ${PKG_CONFIG_INC})

# Library type is determined by BUILD_SHARED_LIBS (shared if ON, static if OFF)
add_library(lvgl_linux ${LV_LINUX_SRC} ${LV_LINUX_BACKEND_SRC})
target_include_directories(lvgl_linux PUBLIC ${PKG_CONFIG_INC})

# If LVGL is configured to use LV_CONF_PATH or Kconfig
# Set the exactly the same definitions on the lvgl_linux target
set_target_properties(lvgl_linux PROPERTIES COMPILE_DEFINITIONS "${LVGL_COMPILER_DEFINES}")
target_include_directories(lvgl_linux PUBLIC
    ${LV_LINUX_INC} ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/lib ${LVGL_CONF_INC_DIR})

# Link LVGL with external dependencies - Modern CMake/CMP0079 allows this
target_link_libraries(lvgl PUBLIC ${PKG_CONFIG_LIB} m pthread rt)

add_executable(lvglsim src/main.c)

# Repeat lvgl_linux to resolve circular dependency with lvgl
target_link_libraries(lvglsim lvgl_linux lvgl lvgl_linux)

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
