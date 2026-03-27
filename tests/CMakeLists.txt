cmake_minimum_required(VERSION 3.16)
project(lvgl_gstreamer_webrtc_tests LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)

# ---------------------------------------------------------------------------
# Paths into the lvgl submodule (read-only — nothing is written there)
# ---------------------------------------------------------------------------
set(LVGL_DIR      "${CMAKE_SOURCE_DIR}/../lvgl")
set(LVGL_TEST_DIR "${LVGL_DIR}/tests")

# ---------------------------------------------------------------------------
# lv_conf.h generation — reuse the parent repo's machinery
# ---------------------------------------------------------------------------
find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(LV_CONF_DEFAULTS_PATH "${CMAKE_SOURCE_DIR}/../lv_conf.defaults")
set(LVGL_TEMPLATE_PATH    "${LVGL_DIR}/lv_conf_template.h")
set(GENERATE_SCRIPT_PATH  "${LVGL_DIR}/scripts/generate_lv_conf.py")

set(LV_BUILD_CONF_PATH "${CMAKE_BINARY_DIR}/lv_conf.h"
    CACHE PATH "path to lv_conf.h" FORCE)

execute_process(
    COMMAND ${Python3_EXECUTABLE} "${GENERATE_SCRIPT_PATH}"
            --template "${LVGL_TEMPLATE_PATH}"
            --defaults "${LV_CONF_DEFAULTS_PATH}"
            --config   "${LV_BUILD_CONF_PATH}"
    RESULT_VARIABLE _conf_result
    OUTPUT_VARIABLE _conf_output
    ERROR_VARIABLE  _conf_output)
if(NOT _conf_result EQUAL 0)
    message(FATAL_ERROR "Failed to generate lv_conf.h: ${_conf_output}")
endif()
message(STATUS "Generated ${LV_BUILD_CONF_PATH}")

# Point lvgl's build system at the generated config
set(LV_BUILD_CONF_PATH "${LV_BUILD_CONF_PATH}" CACHE STRING "set test configuration" FORCE)

# Use the test-specific conf header so LV_BUILD_TEST etc. are set correctly
set(LV_BUILD_CONF_PATH "${LVGL_TEST_DIR}/src/lv_test_conf.h"
    CACHE STRING "set test configuration" FORCE)

# ---------------------------------------------------------------------------
# Build options — match OPTIONS_TEST_SYSHEAP from lvgl/tests/CMakeLists.txt
# ---------------------------------------------------------------------------
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)
set(FLAG_CHECK_WHITELIST --coverage -fsanitize=address -Werror)

function(filter_compiler_options lang options_out)
    set(options ${ARGN})
    foreach(option ${options})
        if(option IN_LIST FLAG_CHECK_WHITELIST)
            list(APPEND ${options_out} ${option})
            continue()
        endif()
        string(TOUPPER "FLAG_SUPPORTED_FOR_${lang}_${option}" option_var_name)
        string(REPLACE "-" "_" option_var_name "${option_var_name}")
        string(REPLACE "+" "_" option_var_name "${option_var_name}")
        if(${lang} STREQUAL C)
            check_c_compiler_flag(${option} ${option_var_name})
        elseif(${lang} STREQUAL CXX)
            check_cxx_compiler_flag(${option} ${option_var_name})
        endif()
        if(${option_var_name})
            list(APPEND ${options_out} ${option})
        endif()
    endforeach()
    set(${options_out} ${${options_out}} PARENT_SCOPE)
endfunction()

# Sanitizers are intentionally disabled for the GStreamer WebRTC integration test.
# The pre-built Rust GStreamer plugins (gst-plugins-rs) are not compiled with ASAN,
# so mixing ASAN-instrumented C code with uninstrumented Rust shared libraries
# causes a Rust thread panic / ASAN abort at runtime.
set(SANITIZE_AND_COVERAGE_OPTIONS)

set(BUILD_OPTIONS
    -DLV_TEST_OPTION=5
    -DLVGL_CI_USING_SYS_HEAP
    -Wno-unused-but-set-variable
    ${SANITIZE_AND_COVERAGE_OPTIONS}
)

set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "disable examples" FORCE)
add_definitions(-DREF_IMGS_PATH="ref_imgs/")

filter_compiler_options(C TEST_SANITIZE_LIBS ${SANITIZE_AND_COVERAGE_OPTIONS})

set(COMPILE_OPTIONS ${BUILD_OPTIONS})

filter_compiler_options(C LVGL_C_COMPILE_OPTIONS
    ${COMPILE_OPTIONS}
    -pedantic-errors -Wall -Wformat-security -Wclobbered -Wdeprecated
    -Wdouble-promotion -Wempty-body -Werror -Wextra -Wmaybe-uninitialized
    -Wmissing-prototypes -Wpointer-arith -Wmultichar -Wpedantic -Wreturn-type
    -Wshadow -Wshift-negative-value -Wsizeof-pointer-memaccess -Wtype-limits
    -Wundef -Wuninitialized -Wunreachable-code
    -Werror=float-conversion -Werror=strict-aliasing
    -Wno-double-promotion -Wno-unreachable-code
    -Wno-gnu-zero-variadic-macro-arguments -Wunused-function
    -Wno-overlength-strings
)
filter_compiler_options(C LVGL_TESTFILE_COMPILE_OPTIONS
    ${LVGL_C_COMPILE_OPTIONS} -Wno-missing-prototypes)

# ---------------------------------------------------------------------------
# Pull in the LVGL library (read-only include of the submodule CMakeLists)
# ---------------------------------------------------------------------------
include(CTest)
include("${LVGL_DIR}/CMakeLists.txt")

target_compile_options(lvgl PUBLIC $<$<COMPILE_LANGUAGE:C>:${LVGL_C_COMPILE_OPTIONS}>)
target_compile_definitions(lvgl PUBLIC LV_BUILD_TEST)

if(TARGET lvgl_thorvg)
    filter_compiler_options(CXX LVGL_CXX_COMPILE_OPTIONS
        ${COMPILE_OPTIONS}
        -Wno-shadow -Wno-unused-parameter -Wno-c++11-extensions
        -Wno-missing-prototypes -Wno-deprecated-copy-with-user-provided-dtor
        -Wno-float-conversion -Wno-pedantic
    )
    target_compile_options(lvgl_thorvg PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${LVGL_CXX_COMPILE_OPTIONS}>)
endif()

# ---------------------------------------------------------------------------
# GStreamer — required for this test
# ---------------------------------------------------------------------------
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER      REQUIRED IMPORTED_TARGET gstreamer-1.0)
pkg_check_modules(GSTREAMER_VIDEO REQUIRED IMPORTED_TARGET gstreamer-video-1.0)
pkg_check_modules(GSTREAMER_APP  REQUIRED IMPORTED_TARGET gstreamer-app-1.0)
target_link_libraries(lvgl PRIVATE
    PkgConfig::GSTREAMER
    PkgConfig::GSTREAMER_VIDEO
    PkgConfig::GSTREAMER_APP)

# Propagate the cmake -DCONFIG_LV_USE_GSTREAMER=1 variable as a C preprocessor
# define so that lv_conf_internal.h picks it up and sets LV_USE_GSTREAMER=1.
# Without this, the cmake variable is never seen by the C compiler.
add_compile_definitions(CONFIG_LV_USE_GSTREAMER=1)
add_compile_definitions(REF_IMG_TOLERANCE=10)

# ---------------------------------------------------------------------------
# Optional dependencies (match what the lvgl tests expect; disable cleanly if absent)
# ---------------------------------------------------------------------------
find_package(JPEG REQUIRED)
include_directories(${JPEG_INCLUDE_DIR})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

pkg_check_modules(WebP libwebp)
if(WebP_FOUND)
    include_directories(${WebP_INCLUDEDIR})
else()
    add_definitions(-DLV_USE_LIBWEBP=0)
endif()

pkg_check_modules(FFMPEG IMPORTED_TARGET libavformat libavcodec libswscale libavutil)
if(NOT FFMPEG_FOUND)
    add_definitions(-DLV_USE_FFMPEG=0)
endif()

pkg_check_modules(wayland_client wayland-client)
if(wayland_client_FOUND)
    pkg_check_modules(WAYLAND REQUIRED wayland-cursor wayland-client wayland-egl xkbcommon)
    link_libraries(${WAYLAND_LIBRARIES})
    include_directories("${CMAKE_BINARY_DIR}/wayland_protocols")
    set(WAYLAND_XDG_SHELL_C "${CMAKE_BINARY_DIR}/wayland_protocols/wayland_xdg_shell.c")
    if(NOT EXISTS "${WAYLAND_XDG_SHELL_C}")
        message(FATAL_ERROR
            "Missing ${WAYLAND_XDG_SHELL_C}. "
            "Run: wayland-scanner private-code "
            "/usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml "
            "${WAYLAND_XDG_SHELL_C}")
    endif()
else()
    add_definitions(-DLV_USE_WAYLAND=0)
    set(WAYLAND_XDG_SHELL_C "")
endif()

pkg_check_modules(xkbcommon xkbcommon)
if(NOT xkbcommon_FOUND)
    add_definitions(-DLV_LIBINPUT_XKB=0)
endif()

find_package(Libinput OPTIONAL_COMPONENTS)
if(NOT LIBINPUT_FOUND)
    add_definitions(-DLV_USE_LIBINPUT=0)
endif()

include("${LVGL_TEST_DIR}/FindLibDRM.cmake")
if(Libdrm_FOUND)
    include_directories(${Libdrm_INCLUDE_DIRS})
else()
    add_definitions(-DLV_USE_LINUX_DRM=0)
endif()

if($ENV{NON_AMD64_BUILD})
    add_definitions(-DLV_USE_OPENGLES=0)
else()
    find_package(OpenGL QUIET)
    find_package(glfw3  QUIET)
    if(OpenGL_FOUND AND glfw3_FOUND)
        include_directories(${OPENGL_INCLUDE_DIR})
    else()
        add_definitions(-DLV_USE_OPENGLES=0)
    endif()
endif()

# ---------------------------------------------------------------------------
# test_common static library — same asset list as lvgl/tests/CMakeLists.txt
# ---------------------------------------------------------------------------
set(TEST_INCLUDE_DIRS
    $<BUILD_INTERFACE:${LVGL_TEST_DIR}/src>
    $<BUILD_INTERFACE:${LVGL_TEST_DIR}/unity>
    $<BUILD_INTERFACE:${LVGL_TEST_DIR}>
    $<BUILD_INTERFACE:${LVGL_CONF_DIR}>
)

add_library(test_common STATIC
    "${LVGL_TEST_DIR}/src/lv_test_init.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_animimg001.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_animimg002.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_animimg003.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_img_cogwheel_i4.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_img_cogwheel_a8.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_img_cogwheel_rgb565.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_img_cogwheel_rgb565a8.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_img_cogwheel_xrgb8888.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_img_cogwheel_argb8888.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_img_svg.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_font_1.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_font_2.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_font_3.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_font_montserrat_ascii_1bpp.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_font_montserrat_ascii_2bpp.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_font_montserrat_ascii_4bpp.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_font_montserrat_ascii_8bpp.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_font_montserrat_ascii_3bpp_compressed.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_font_1_bin.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_font_2_bin.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_font_3_bin.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_img_caret_down.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_arc_bg.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_img_bulb_gif.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_img_lvgl_logo_png.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_img_lvgl_logo_jpg.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_img_emoji_F617.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_ubuntu_font.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_kern_one_otf.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_imagebutton_left.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_imagebutton_mid.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_imagebutton_right.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_music_button_play.c"
    "${LVGL_TEST_DIR}/src/test_assets/test_lottie_approve.c"
    "${LVGL_TEST_DIR}/unity/unity.c"
)
target_include_directories(test_common PUBLIC ${TEST_INCLUDE_DIRS})
target_compile_options(test_common PUBLIC ${LVGL_TESTFILE_COMPILE_OPTIONS})
target_compile_definitions(test_common PRIVATE LV_BUILD_TEST ${LVGL_COMPILER_DEFINES})

# Wayland: use PUBLIC so the generated source is propagated into each test
# executable that links test_common — this avoids a static-lib link-order issue
# where lv_wayland_xdg_shell.c.o (inside liblvgl.a) references these symbols
# but they were only compiled into test_common, not the final executable.
if(wayland_client_FOUND AND WAYLAND_XDG_SHELL_C)
    target_sources(test_common PUBLIC "${WAYLAND_XDG_SHELL_C}")
endif()

# ---------------------------------------------------------------------------
# Unity test runner generation + test executable
# ---------------------------------------------------------------------------
find_package(Ruby REQUIRED)
set(generate_test_runner_rb     "${LVGL_TEST_DIR}/unity/generate_test_runner.rb")
set(generate_test_runner_config "${LVGL_TEST_DIR}/config.yml")

set(TEST_CASE_SRC "${CMAKE_SOURCE_DIR}/src/test_gstreamer_webrtc.c")
set(TEST_NAME "test_gstreamer_webrtc")
set(TEST_RUNNER "${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}_Runner.c")

add_custom_command(
    OUTPUT  "${TEST_RUNNER}"
    COMMAND ${RUBY_EXECUTABLE} "${generate_test_runner_rb}"
            "${TEST_CASE_SRC}" "${TEST_RUNNER}"
            "${generate_test_runner_config}"
    DEPENDS "${generate_test_runner_rb}" "${TEST_CASE_SRC}"
            "${generate_test_runner_config}"
)

add_executable(${TEST_NAME}
    "${TEST_CASE_SRC}"
    "${TEST_RUNNER}"
)

target_link_libraries(${TEST_NAME} PRIVATE
    test_common
    lvgl_demos
    lvgl
    $<$<TARGET_EXISTS:lvgl_thorvg>:lvgl_thorvg>
    ${PNG_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${WebP_LINK_LIBRARIES}
    m pthread
    ${TEST_SANITIZE_LIBS}
)
if(Libdrm_FOUND)
    target_link_libraries(${TEST_NAME} PRIVATE ${LIBDRM_LIBRARIES})
endif()
if(LIBINPUT_FOUND)
    target_link_libraries(${TEST_NAME} PRIVATE ${LIBINPUT_LIBRARIES})
endif()
if(FFMPEG_FOUND)
    target_link_libraries(${TEST_NAME} PRIVATE PkgConfig::FFMPEG)
endif()
if(NOT $ENV{NON_AMD64_BUILD})
    if(OpenGL_FOUND AND glfw3_FOUND)
        target_link_libraries(${TEST_NAME} PRIVATE ${OPENGL_LIBRARIES} glfw)
    endif()
endif()

target_include_directories(${TEST_NAME} PUBLIC ${TEST_INCLUDE_DIRS})
target_compile_options(${TEST_NAME} PUBLIC ${LVGL_TESTFILE_COMPILE_OPTIONS})

add_test(
    NAME    ${TEST_NAME}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMAND ${TEST_NAME}
)
# Hard timeout: 15 s stream-wait + 5 s screenshot + 10 s teardown headroom.
# Prevents the test from hanging indefinitely if GStreamer pipeline teardown blocks.
set_tests_properties(${TEST_NAME} PROPERTIES TIMEOUT 60)
