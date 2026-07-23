set(SOURCES lv_demo_truck.c)

add_library(lv_demo_truck_ui ${SOURCES})
target_include_directories(lv_demo_truck_ui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_link_libraries(lv_demo_truck_ui PUBLIC lvgl)
