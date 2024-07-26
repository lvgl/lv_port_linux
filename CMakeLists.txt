cmake_minimum_required(VERSION 3.10)
project(lvgl LANGUAGES C)

set(CMAKE_C_STANDARD 99)#C99 # lvgl officially support C99 and above
set(CMAKE_CXX_STANDARD 17)#C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(main main.c mouse_cursor_icon.c)

option(USE_FBDEV "Use the framebuffer backend" ON)
option(USE_DRM "Use the DRM backend" OFF)
option(USE_SDL2 "Use the SDL2 backend" OFF)
option(USE_THOR "Link with the ThorVG vector grapics library" OFF)

if(USE_FBDEV)
   add_compile_definitions(LV_USE_LINUX_FBDEV=1)
elseif(USE_DRM)
   add_compile_definitions(LV_USE_LINUX_FBDEV=0)
   add_compile_definitions(LV_USE_LINUX_DRM=1)
   include(${CMAKE_CURRENT_LIST_DIR}/lvgl/tests/FindLibDRM.cmake)
   include_directories(${Libdrm_INCLUDE_DIRS})
   target_link_libraries(main ${Libdrm_LIBRARIES})
elseif(USE_SDL2)
   add_compile_definitions(LV_USE_LINUX_FBDEV=0)
   add_compile_definitions(LV_USE_SDL=1)
   find_package(SDL2)
   target_link_libraries(main ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
   include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})
endif()

if(USE_THOR)
   enable_language(CXX)
   target_link_libraries(main lvgl::thorvg)
   add_compile_definitions(LV_USE_THORVG_INTERNAL=1)
else()
   add_compile_definitions(LV_USE_THORVG_INTERNAL=0)
endif()

add_subdirectory(lvgl EXCLUDE_FROM_ALL)
target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR})

target_link_libraries(main lvgl lvgl::examples lvgl::demos m pthread)
add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/main DEPENDS main)

set_target_properties(main PROPERTIES OUTPUT_NAME lv_port_linux)
install(TARGETS main DESTINATION local/bin)
