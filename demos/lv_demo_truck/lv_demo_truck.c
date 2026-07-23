/**
 * @file lv_demo_truck.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_truck.h"

#if LV_USE_GLTF

#include <stdio.h>

/*********************
 *      DEFINES
 *********************/

#define PI 3.14159265358979323846
#define DEG_TO_RAD   0.01745329238f
#define SLIDER_COLOR    lv_color_hex(0x26A69A)
#define PLAY_BTN_COLOR  lv_color_hex(0x00C8535)
#define PAUSE_BTN_COLOR lv_color_hex(0xF9A825)
#define LV_TRUCK_DEMO_CHECKBOX_HEIGHT 22

#ifndef LV_DEMO_GLTF_CAMERA_NUM
    #define LV_DEMO_GLTF_CAMERA_NUM LV_GLTF_DEFAULT_CAMERA
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_point_t last_pos;
    bool is_dragging;
    float sensitivity;
} mouse_event_data_t;

typedef struct {
    lv_obj_t * viewer;
    lv_obj_t * label;
} play_pause_event_data_t;

typedef void (*lv_gltf_set_float_fn)(lv_obj_t *, float);
typedef void (*lv_gltf_set_int_fn)(lv_obj_t *, uint32_t);

typedef union {
    void * ptr;
    lv_gltf_set_float_fn cb;
} lv_gltf_set_float_fn_union_t;

typedef union {
    void * ptr;
    lv_gltf_set_int_fn fn;
} lv_gltf_set_int_fn_union_t;

typedef enum { LV_TRUCK_DOOR = 1, LV_TRUCK_WINDOW = 2, LV_TRUCK_TRUNKHOOD = 3, LV_TRUCK_SUNROOF = 4} lv_demo_truck_hatch_type;
typedef enum { LV_HATCH_UNSET = 0, LV_HATCH_OPEN = 1, LV_HATCH_CLOSED = 2} lv_demo_truck_hatch_state;
typedef enum { LV_TIRE_UNSET = 0, LV_TIRE_CLEAN = 1, LV_TIRE_DIRTY = 2} lv_demo_truck_tire_type;
typedef enum { LV_SPEED_UNSET = 0, LV_SPEED_MPH = 1, LV_SPEED_KMPH = 2} lv_demo_truck_speed_type;
typedef enum { LV_BLINKER_UNSET = 0, LV_BLINKER_NONE = 1, LV_BLINKER_LEFT = 2, LV_BLINKER_RIGHT = 3, LV_BLINKER_HAZARD = 4} lv_demo_truck_blinker_setting;
typedef enum { LV_WIPERS_UNSET = 0, LV_WIPERS_OFF = 1, LV_WIPERS_INT = 2, LV_WIPERS_LOW = 3, LV_WIPERS_HIGH = 4} lv_demo_truck_wipers_setting;
typedef enum { LV_HEADLIGHTS_UNSET = 0, LV_HEADLIGHTS_OFF = 1, LV_HEADLIGHTS_LOW = 2, LV_HEADLIGHTS_HIGH = 3} lv_demo_truck_headlights_setting;
typedef enum { LV_CAMERA_UNSET = 0, LV_CAMERA_INTERIOR = 1, LV_CAMERA_EXTERIOR = 2} lv_demo_truck_camera_setting;

typedef struct lv_demo_foldout_ {
    lv_obj_t * contents;
    lv_obj_t * title;
    lv_obj_t * title_button;
    bool contents_visible;
} lv_demo_foldout_t;

typedef struct lv_demo_truck_hatch_ {
    lv_anim_t   anim_template;
    lv_anim_t * running_anim;
    lv_gltf_model_node_t * node;
    float open_degrees;
    float closed_degrees;
    int32_t last_set_value;
    int32_t goal_value;
    uint32_t length_ms;
    lv_demo_truck_hatch_type hatch_type;
    lv_demo_truck_hatch_state hatch_state;
    lv_obj_t * checkbox;
} lv_demo_truck_hatch_t;

typedef struct lv_demo_truck_tire_ {
    lv_anim_t   anim_template;
    lv_anim_t * running_anim;
    lv_gltf_model_node_t * node_steering;
    lv_gltf_model_node_t * node_spin;
    lv_gltf_model_node_t * node_tire_type1;
    lv_gltf_model_node_t * node_tire_type2;
    float actual_spin_angle;
    float goal_spin_rate;
    float last_spin_rate;
    lv_demo_truck_tire_type tire_type;
} lv_demo_truck_tire_t;

typedef struct lv_demo_truck_tireset_controller_ {
    lv_demo_truck_tire_t * tire_FD_spin;
    lv_demo_truck_tire_t * tire_FP_spin;
    lv_demo_truck_tire_t * tire_BDP_spin;
    float goal_speed_ratio;
    float tach_offset;
    float goal_spin_rate;
    float last_spin_rate;
    float goal_steer_ratio;
    float goal_steer_angle;
    float last_steer_angle;
    lv_demo_truck_tire_type tire_type;
    lv_demo_truck_speed_type speed_type;
    lv_obj_t * checkbox_mph;
    lv_obj_t * checkbox_kmph;
    lv_obj_t * label_speed;
    lv_obj_t * label_speed_type;
    lv_obj_t * slider_speed;
    lv_obj_t * slider_steering;
    bool tires_dirty;
} lv_demo_truck_tireset_controller_t;

typedef struct lv_demo_truck_paintset_ {
    lv_gltf_model_node_t * body_node;
    lv_gltf_model_node_t * hood_node;
    lv_gltf_model_node_t * door_FD_node;
    lv_gltf_model_node_t * door_FP_node;
    lv_gltf_model_node_t * door_BD_node;
    lv_gltf_model_node_t * door_BP_node;
    lv_gltf_model_node_t * tailgate_node;
    lv_obj_t * button;
} lv_demo_truck_paintset_t;

typedef struct lv_demo_truck_lightset_controller_ {
    lv_anim_t   anim_template;
    lv_anim_t * running_anim;
    lv_gltf_model_node_t * node_left_blinker_on;
    lv_gltf_model_node_t * node_left_blinker_off;
    lv_gltf_model_node_t * node_right_blinker_on;
    lv_gltf_model_node_t * node_right_blinker_off;
    lv_gltf_model_node_t * node_brakes_on;
    lv_gltf_model_node_t * node_brakes_off;
    lv_gltf_model_node_t * node_lights_low_on;
    lv_gltf_model_node_t * node_lights_low_off;
    lv_gltf_model_node_t * node_lights_high_on;
    lv_gltf_model_node_t * node_lights_high_off;
    float actual_blink_cycle;
    bool brakes_active;
    float blinker_set_angle;
    float blinker_ext_angle;
    lv_demo_truck_blinker_setting blinker_setting;
    lv_demo_truck_headlights_setting headlights_setting;
    lv_demo_truck_headlights_setting last_applied_headlights_setting;
    lv_obj_t * checkbox_headlights_low;
    lv_obj_t * checkbox_headlights_high;
} lv_demo_truck_lightset_controller_t;

typedef struct lv_demo_truck_wipers_controller_ {
    lv_anim_t   anim_template;
    lv_anim_t * running_anim;
    lv_gltf_model_node_t * node_left_wiper;
    lv_gltf_model_node_t * node_right_wiper;
    float actual_blink_cycle;
    bool brakes_active;
    uint32_t last_anim_value;
    lv_demo_truck_wipers_setting wipers_setting;
    lv_demo_truck_wipers_setting next_wipers_setting;
    lv_obj_t * btn_wipers_low;
    lv_obj_t * btn_wipers_med;
    lv_obj_t * btn_wipers_high;
} lv_demo_truck_wipers_controller_t;

typedef struct lv_demo_truck_interior_controller_ {
    lv_anim_t   anim_template;
    lv_anim_t * running_anim;
    lv_gltf_model_node_t * node_speedometer_needle;
    lv_gltf_model_node_t * node_tachometer_needle;
    lv_gltf_model_node_t * node_left_turn_indicator;
    lv_gltf_model_node_t * node_right_turn_indicator;
    lv_gltf_model_node_t * node_lights_low_indicator_on;
    lv_gltf_model_node_t * node_lights_high_indicator_on;
    lv_gltf_model_node_t * node_steering_wheel;
} lv_demo_truck_interior_controller_t;

typedef struct lv_demo_truck_camera_controller_ {
    lv_anim_t   anim_template;
    lv_anim_t * running_anim;
    lv_gltf_model_node_t * node_temp_placeholder;

    lv_obj_t * viewer;

    lv_obj_t * checkbox_camera_interior;
    lv_obj_t * checkbox_camera_exterior;
    lv_obj_t * checkbox_camera_free;

    lv_demo_truck_camera_setting next_camera_setting;
    lv_demo_truck_camera_setting camera_setting;
} lv_demo_truck_camera_controller_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void hatch_open_close_complete_anim_cb(lv_anim_t * viewer);
static void hatch_update_checkbox(lv_demo_truck_hatch_t * hatch);
static void hatch_open_close_on_x_anim_cb(lv_anim_t * viewer, int32_t anim_value);
static void hatch_open_close_on_y_anim_cb(lv_anim_t * viewer, int32_t anim_value);
static void hatch_open_close_on_z_anim_cb(lv_anim_t * viewer, int32_t anim_value);
static void hatch_open_close_slide_z_anim_cb(lv_anim_t * obj, int32_t anim_value);
static void tire_spin_on_z_anim_cb(lv_anim_t * obj, int32_t anim_value);
static void blinker_lights_anim_cb(lv_anim_t * obj, int32_t anim_value);
static void interior_update_anim_cb(lv_anim_t * obj, int32_t anim_value);
static void camera_update_anim_cb(lv_anim_t * obj, int32_t anim_value);
static void wipers_anim_cb(lv_anim_t * obj, int32_t anim_value);

static void show_foldout(lv_demo_foldout_t * foldout);
static void hide_foldout(lv_demo_foldout_t * foldout);

static void hide_node(lv_gltf_model_node_t * node);
static void show_node(lv_gltf_model_node_t * node);
static void hide_all_paintsets(void);
static void show_paintset(lv_demo_truck_paintset_t * paintset);
static void hide_paintset(lv_demo_truck_paintset_t * paintset);
static void apply_dirt(bool truck_is_dirty);
static void enable_antialiasing(bool use_antialiasing);
static void select_paintset_A(void);
static void select_paintset_B(void);
static void select_paintset_C(void);

static void set_tire_type(lv_demo_truck_tire_t * tire, lv_demo_truck_tire_type tire_type);
static void set_tireset_type(lv_demo_truck_tireset_controller_t * tireset, lv_demo_truck_tire_type tire_type);
static void set_tireset_speed_type(lv_demo_truck_tireset_controller_t * tireset, lv_demo_truck_speed_type speed_type);
static void set_tireset_speed_ratio(lv_demo_truck_tireset_controller_t * tireset, float max_speed_ratio);
static void set_tireset_steer_ratio(lv_demo_truck_tireset_controller_t * tireset, float max_steer_ratio);
static void set_lightset_blinker_type(lv_demo_truck_lightset_controller_t * lights,
                                      lv_demo_truck_blinker_setting blinker_type);
static void set_lightset_headlight_type(lv_demo_truck_lightset_controller_t * lights,
                                        lv_demo_truck_headlights_setting headlights_type);
static void set_wiper_speed(lv_demo_truck_wipers_controller_t * wipers, lv_demo_truck_wipers_setting wipers_setting);
static void set_camera_num(lv_demo_truck_camera_controller_t * cameras, lv_demo_truck_camera_setting camera_type);

static void open_hatch(lv_demo_truck_hatch_t * hatch);
static void close_hatch(lv_demo_truck_hatch_t * hatch);
static void open_all_doors(void);
static void close_all_doors(void);
static void open_all_windows(void);
static void close_all_windows(void);

static void init_anim_controllers(lv_obj_t * viewer);
static void init_paintset_controllers(lv_obj_t * viewer);
static void init_tire_controllers(lv_obj_t * viewer);
static void init_lights_controller(lv_obj_t * viewer);
static void init_wipers_controller(lv_obj_t * viewer);
static void init_interior_controller(lv_obj_t * viewer);
static void init_camera_controller(lv_obj_t * viewer);
static void init_subjects(lv_obj_t * viewer);
static void init_checkbox_states(void);

static void create_about_panel(lv_obj_t * panel, lv_obj_t * viewer);
static void create_doors_panel(lv_obj_t * panel, lv_obj_t * viewer);
static void create_windows_panel(lv_obj_t * panel, lv_obj_t * viewer);
static void create_paint_panel(lv_obj_t * panel, lv_obj_t * viewer);
static void create_speed_panel(lv_obj_t * panel, lv_obj_t * viewer);
static void create_steering_panel(lv_obj_t * panel, lv_obj_t * viewer);
static void create_headlights_panel(lv_obj_t * panel, lv_obj_t * viewer);
static void create_wipers_panel(lv_obj_t * panel, lv_obj_t * viewer);
static void create_control_panel(lv_obj_t * viewer);
static void create_camera_panel(lv_obj_t * panel, lv_obj_t * viewer);
static void create_options_panel(lv_obj_t * parent, lv_obj_t * viewer);

static void on_mouse_event(lv_event_t * e);
static void show_foldout_event_cb(lv_event_t * e);
static void hide_foldout_event_cb(lv_event_t * e);
static void open_all_doors_event_handler(lv_event_t * e);
static void close_all_doors_event_handler(lv_event_t * e);
static void toggle_door_FD_event_handler(lv_event_t * e);
static void toggle_door_FP_event_handler(lv_event_t * e);
static void toggle_door_BD_event_handler(lv_event_t * e);
static void toggle_door_BP_event_handler(lv_event_t * e);
static void toggle_hood_event_handler(lv_event_t * e);
static void toggle_tailgate_event_handler(lv_event_t * e);
static void open_all_windows_event_handler(lv_event_t * e);
static void close_all_windows_event_handler(lv_event_t * e);
static void toggle_window_FD_event_handler(lv_event_t * e);
static void toggle_window_FP_event_handler(lv_event_t * e);
static void toggle_window_BD_event_handler(lv_event_t * e);
static void toggle_window_BP_event_handler(lv_event_t * e);
static void toggle_sunroof_event_handler(lv_event_t * e);
static void select_paintset_A_event_handler(lv_event_t * e);
static void select_paintset_B_event_handler(lv_event_t * e);
static void select_paintset_C_event_handler(lv_event_t * e);
static void toggle_mud_event_handler(lv_event_t * e);
static void select_speed_event_handler(lv_event_t * e);
static void select_mph_event_handler(lv_event_t * e);
static void select_kmph_event_handler(lv_event_t * e);
static void select_steering_event_handler(lv_event_t * e);
static void select_left_blinker_event_handler(lv_event_t * e);
static void select_right_blinker_event_handler(lv_event_t * e);
static void select_hazard_blinker_event_handler(lv_event_t * e);
static void select_wipers_low_event_handler(lv_event_t * e);
static void select_wipers_med_event_handler(lv_event_t * e);
static void select_wipers_high_event_handler(lv_event_t * e);
static void select_camera_interior_event_handler(lv_event_t * e);
static void select_camera_exterior_event_handler(lv_event_t * e);
static void select_camera_free_event_handler(lv_event_t * e);
static void toggle_antialias_event_handler(lv_event_t * e);
static void brakes_press_event_handler(lv_event_t * e);
static void brakes_pressing_event_handler(lv_event_t * e);
static void brakes_released_event_handler(lv_event_t * e);

static lv_obj_t * add_row(lv_obj_t * parent);
static lv_obj_t * add_sep(lv_obj_t * parent);
static lv_obj_t * add_button_to_row(lv_obj_t * row, lv_color_t color);
static lv_obj_t * add_labeled_event_button_to_row(lv_obj_t * row, lv_color_t color, const char * label,
                                                  lv_event_cb_t event_cb);
static lv_obj_t * add_slider_to_row(lv_obj_t * row, lv_color_t color);
static lv_obj_t * add_checkbox_to_row(lv_obj_t * row, lv_color_t color, lv_event_cb_t event_cb);
static lv_obj_t * add_labeled_checkbox_row(lv_obj_t * panel, const char * label, lv_event_cb_t event_cb);
static lv_obj_t * add_title_to_row(lv_obj_t * row, const char * title);
static lv_obj_t * add_foldout_header(lv_obj_t * parent, const char * title);

static void animation_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void animation_speed_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void style_slider(lv_obj_t * slider, lv_color_t accent_color);
static void style_toggle_button(lv_obj_t * btn, lv_color_t accent_color);
static void style_control_panel(lv_obj_t * panel);
static void style_checkbox(lv_obj_t * checkbox, lv_color_t accent_color);

static double distance_per_revolution(double tire_radius);
static double revolution_rate(double tire_radius, double travel_rate_kmh);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_subject_t yaw_subject;
static lv_subject_t pitch_subject;
static lv_subject_t animation_subject;
static lv_subject_t animation_speed_subject;

static lv_gltf_set_float_fn_union_t pitch_fn = { .cb = lv_gltf_set_pitch };
static lv_gltf_set_float_fn_union_t yaw_fn = { .cb = lv_gltf_set_yaw };

static lv_demo_truck_hatch_t * door_FD_open_close;
static lv_demo_truck_hatch_t * door_FP_open_close;
static lv_demo_truck_hatch_t * door_BD_open_close;
static lv_demo_truck_hatch_t * door_BP_open_close;
static lv_demo_truck_hatch_t * hood_open_close;
static lv_demo_truck_hatch_t * tailgate_open_close;
static lv_demo_truck_hatch_t * window_FD_open_close;
static lv_demo_truck_hatch_t * window_FP_open_close;
static lv_demo_truck_hatch_t * window_BD_open_close;
static lv_demo_truck_hatch_t * window_BP_open_close;
static lv_demo_truck_hatch_t * sunroof_open_close;

static lv_demo_truck_paintset_t * paintset_red;
static lv_demo_truck_paintset_t * paintset_gray;
static lv_demo_truck_paintset_t * paintset_green;

static lv_demo_truck_tireset_controller_t * tireset_controller;
static lv_demo_truck_lightset_controller_t * lights_controller;
static lv_demo_truck_wipers_controller_t * wipers_controller;
static lv_demo_truck_interior_controller_t * interior_controller;
static lv_demo_truck_camera_controller_t * camera_controller;

static lv_gltf_model_node_t * node_dirty_overlay_1;
static lv_gltf_model_node_t * node_dirty_overlay_2;
static lv_gltf_model_node_t * node_dirty_overlay_3;
static lv_obj_t * open_all_doors_btn;
static lv_obj_t * close_all_doors_btn;
static lv_obj_t * open_all_windows_btn;
static lv_obj_t * close_all_windows_btn;

static lv_obj_t * left_blinker_btn;
static lv_obj_t * hazard_blinker_btn;
static lv_obj_t * right_blinker_btn;

static lv_obj_t * mud_toggle_btn;
static bool has_mud = false;
static lv_obj_t * checkbox_antialiasing;
static lv_demo_foldout_t * last_opened_foldout = NULL;
static lv_obj_t * logo_viewer = NULL;

static const float MAX_SPIN_SLIDER_VALUE = 10000.f;
static const float MIN_MAX_STEER_SLIDER_VALUE = 5000.f;

static const float TWO_PI = PI * 2.f;
static float KM_TO_MILE = 0.621371f;
static float MAX_SPIN_RATE = 1.f;
static float MAX_STEER_ANGLE = PI / 6.f;

static float MAX_SPEED_KMH = 105.f;

static const char * door_FD_path = "/truck/013_door_driver_group";
static const char * door_FP_path = "/truck/014_door_pass_group";
static const char * door_BD_path = "/truck/017_door_back_driver_group";
static const char * door_BP_path = "/truck/018_door_back_pass_group";
static const char * hood_path = "/truck/012_hood_group";
static const char * tailgate_path = "/truck/010_tailgate_group";

static const char * window_FD_path = "/truck/013_door_driver_group/door_driver_window_pivot";
static const char * window_FP_path = "/truck/014_door_pass_group/door_pass_window_pivot";
static const char * window_BD_path = "/truck/017_door_back_driver_group/door_back_driver_window_pivot";
static const char * window_BP_path = "/truck/018_door_back_pass_group/door_back_pass_window_pivot";
static const char * sunroof_path = "/truck/500_glass_sunroof";

static const char * steer_FD_path = "/truck/030_tire_front_driver";
static const char * steer_FP_path = "/truck/033_tire_front_pass";
static const char * tire_FD_path = "/truck/030_tire_front_driver/tire_spin_controller_000";
static const char * tire_FP_path = "/truck/033_tire_front_pass/tire_spin_controller_001";
static const char * tire_BDP_path = "/truck/036_tires_back";
static const char * tire_FD_type1_path =
    "/truck/030_tire_front_driver/tire_spin_controller_000/032_tire_front_driver_clean";
static const char * tire_FD_type2_path =
    "/truck/030_tire_front_driver/tire_spin_controller_000/031_tire_front_driver_muddy";
static const char * tire_FP_type1_path =
    "/truck/033_tire_front_pass/tire_spin_controller_001/035_tire_front_pass_clean";
static const char * tire_FP_type2_path =
    "/truck/033_tire_front_pass/tire_spin_controller_001/034_tire_front_pass_muddy";
static const char * tire_BDP_type1_path = "/truck/036_tires_back/038_tires_back_clean";
static const char * tire_BDP_type2_path = "/truck/036_tires_back/037_tires_back_muddy";

static const char * paintset_A_body = "/truck/001a_body_painted";
static const char * paintset_B_body = "/truck/001b_body_painted";
static const char * paintset_C_body = "/truck/001c_body_painted";
static const char * paintset_A_hood = "/truck/012_hood_group/012a_hood_painted_body";
static const char * paintset_B_hood = "/truck/012_hood_group/012b_hood_painted_body";
static const char * paintset_C_hood = "/truck/012_hood_group/012c_hood_painted_body";
static const char * paintset_A_door_FD = "/truck/013_door_driver_group/002a_door_driver_painted_body";
static const char * paintset_B_door_FD = "/truck/013_door_driver_group/002b_door_driver_painted_body";
static const char * paintset_C_door_FD = "/truck/013_door_driver_group/002c_door_driver_painted_body";
static const char * paintset_A_door_FP = "/truck/014_door_pass_group/002a_door_pass_painted_body";
static const char * paintset_B_door_FP = "/truck/014_door_pass_group/002b_door_pass_painted_body";
static const char * paintset_C_door_FP = "/truck/014_door_pass_group/002c_door_pass_painted_body";
static const char * paintset_A_door_BD = "/truck/017_door_back_driver_group/004a_doors_back_driver_painted_body";
static const char * paintset_B_door_BD = "/truck/017_door_back_driver_group/004b_doors_back_driver_painted_body";
static const char * paintset_C_door_BD = "/truck/017_door_back_driver_group/004c_doors_back_driver_painted_body";
static const char * paintset_A_door_BP = "/truck/018_door_back_pass_group/004a_doors_back_pass_painted_body";
static const char * paintset_B_door_BP = "/truck/018_door_back_pass_group/004b_doors_back_pass_painted_body";
static const char * paintset_C_door_BP = "/truck/018_door_back_pass_group/004c_doors_back_pass_painted_body";
static const char * paintset_A_tailgate = "/truck/010_tailgate_group/011a_tailgate_painted_body";
static const char * paintset_B_tailgate = "/truck/010_tailgate_group/011b_tailgate_painted_body";
static const char * paintset_C_tailgate = "/truck/010_tailgate_group/011c_tailgate_painted_body";

static const char * dirty_overlay_1 = "/truck/013_door_driver_group/100_truck_splatters_door_driver";
static const char * dirty_overlay_2 = "/truck/014_door_pass_group/100_truck_splatters_door_pass";
static const char * dirty_overlay_3 = "/truck/100_truck_splatters_body";

static const char * node_speedometer_needle_path = "/truck/200_dash_parts_group/200_speedometer_needle";
static const char * node_tachometer_needle_path = "/truck/200_dash_parts_group/201_tachometer_needle";
static const char * node_left_turn_indicator_path = "/truck/200_dash_parts_group/205_left_turn_indicator";
static const char * node_right_turn_indicator_path = "/truck/200_dash_parts_group/206_right_turn_indicator";

static const char * node_left_blinker_on_path = "/truck/500_blinkers_driver_on";
static const char * node_left_blinker_off_path = "/truck/500_blinkers_driver_off";
static const char * node_right_blinker_on_path = "/truck/500_blinkers_pass_on";
static const char * node_right_blinker_off_path = "/truck/500_blinkers_pass_off";
static const char * node_brakes_on_path = "/truck/500_brakes_on";
static const char * node_brakes_off_path = "/truck/500_brakes_off";
static const char * node_lights_low_on_path = "/truck/500_headlights_low_on_group";
static const char * node_lights_low_off_path = "/truck/500_headlights_low_off_group";
static const char * node_lights_high_on_path = "/truck/500_headlights_high_on_group";
static const char * node_lights_high_off_path = "/truck/500_headlights_high_off_group";

static const char * node_left_wiper_path = "/truck/022_wiper_driver_pivot/022_wiper_driver";
static const char * node_right_wiper_path = "/truck/023_wiper_pass_pivot/023_wiper_pass";

static const char * node_steering_wheel_path = "/truck/027_steering_wheel_group/027_steering_wheel_pivot";

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static const char * ui_assets_path = NULL;

lv_obj_t * lv_demo_truck(const char * assets_path)
{
    ui_assets_path = assets_path;
    lv_obj_t * viewer = lv_gltf_create(lv_screen_active());

    lv_gltf_set_distance(viewer, .15);
    // lv_gltf_set_pitch(viewer, -85.f);
    lv_obj_set_size(viewer, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollable(viewer, false);
    lv_gltf_set_background_mode(viewer, LV_GLTF_BG_MODE_SOLID);

    // lv_gltf_set_focal_x(viewer, -10.f);
    char truck_model_path[256];
    lv_snprintf(truck_model_path, sizeof(truck_model_path), "%s/%s", ui_assets_path, "lv_truck.glb");
    lv_gltf_model_t * model = lv_gltf_load_model_from_file(viewer, truck_model_path);
    LV_ASSERT_NULL(model);

    lv_gltf_set_focal_y(viewer, 1);
    init_subjects(viewer);
    init_anim_controllers(viewer);
    init_paintset_controllers(viewer);
    init_tire_controllers(viewer);
    init_lights_controller(viewer);
    init_wipers_controller(viewer);
    init_interior_controller(viewer);
    init_camera_controller(viewer);

    create_control_panel(viewer);

    set_tireset_speed_type(tireset_controller, LV_SPEED_KMPH);

    mouse_event_data_t * mouse_state = lv_zalloc(sizeof(*mouse_state));
    LV_ASSERT_MALLOC(mouse_state);
    mouse_state->sensitivity = 0.3;

    lv_obj_add_event_cb(viewer, on_mouse_event, LV_EVENT_PRESSED, mouse_state);
    lv_obj_add_event_cb(viewer, on_mouse_event, LV_EVENT_PRESSING, mouse_state);
    lv_obj_add_event_cb(viewer, on_mouse_event, LV_EVENT_RELEASED, mouse_state);
    lv_obj_add_event_cb(viewer, on_mouse_event, LV_EVENT_PRESS_LOST, mouse_state);
    lv_obj_add_event_cb(viewer, on_mouse_event, LV_EVENT_DELETE, mouse_state);


    close_all_windows();
    close_all_doors();
    apply_dirt(false);
    set_wiper_speed(wipers_controller, LV_WIPERS_OFF);
    set_lightset_headlight_type(lights_controller, LV_HEADLIGHTS_OFF);
    close_hatch(tailgate_open_close);
    close_hatch(hood_open_close);
    close_hatch(sunroof_open_close);
    set_camera_num(camera_controller, LV_CAMERA_EXTERIOR);
    init_checkbox_states();
    select_paintset_A();
    enable_antialiasing(true);

    return viewer;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_demo_foldout_t * lv_demo_foldout(lv_obj_t * parent, const char * title)
{
    lv_demo_foldout_t * foldout;
    foldout = (lv_demo_foldout_t *)lv_malloc(sizeof(*foldout));
    lv_memzero(foldout, sizeof(*foldout));

    lv_obj_t * title_btn = lv_button_create(parent);
    lv_obj_set_size(title_btn, LV_PCT(100), 30);
    lv_obj_set_style_bg_color(title_btn, lv_color_hex(0xFF6B35), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(title_btn, LV_OPA_0, LV_PART_MAIN);
    lv_obj_set_style_radius(title_btn, 0, 0);
    lv_obj_add_event_cb(title_btn, show_foldout_event_cb, LV_EVENT_CLICKED, foldout);
    lv_obj_set_style_shadow_width(title_btn, 0, 0);
    lv_obj_set_style_border_color(title_btn, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_border_opa(title_btn, LV_OPA_30, LV_PART_MAIN);
    lv_obj_set_style_border_width(title_btn, 3, 0);
    lv_obj_set_style_border_side(title_btn, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_pad_all(title_btn, 5, 0);
    lv_obj_set_style_margin_top(title_btn, -3, 0);
    lv_obj_set_style_margin_bottom(title_btn, -4, 0);
    lv_obj_set_layout(title_btn, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(title_btn, LV_FLEX_FLOW_ROW);

    lv_obj_t * right_arrow = lv_label_create(title_btn);
    lv_label_set_text_static(right_arrow, LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_color(right_arrow, lv_color_white(), 0);
    lv_obj_set_style_margin_right(right_arrow, 2, 0);
    lv_obj_t * button_label = lv_label_create(title_btn);
    lv_label_set_text_static(button_label, title);
    lv_obj_set_style_text_color(button_label, lv_color_white(), 0);

    lv_obj_t * foldout_contents = add_row(parent);
    lv_obj_set_style_pad_all(foldout_contents, 0, LV_PART_MAIN);
    lv_obj_set_style_margin_all(foldout_contents, 0, LV_PART_MAIN);
    lv_obj_set_width(foldout_contents, LV_PCT(100));

    lv_obj_t * title_row = add_row(foldout_contents);
    lv_obj_set_style_margin_top(title_row, 2, 0);
    lv_obj_t * down_arrow = lv_label_create(title_row);
    lv_label_set_text_static(down_arrow, LV_SYMBOL_DOWN);
    lv_obj_set_style_text_color(down_arrow, lv_color_white(), 0);
    lv_obj_set_style_margin_right(down_arrow, -7, 0);
    add_title_to_row(title_row, title);
    lv_obj_set_flex_flow(title_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(title_row, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(title_row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(title_row, 0, LV_PART_MAIN);
    lv_obj_set_style_margin_bottom(title_row, 0, LV_PART_MAIN);
    lv_obj_add_event_cb(title_row, hide_foldout_event_cb, LV_EVENT_CLICKED, foldout);


    foldout->title = title_row;
    foldout->title_button = title_btn;
    foldout->contents = foldout_contents;
    foldout->contents_visible = true;
    //hide_foldout(foldout);
    return foldout;
}

static lv_demo_truck_camera_controller_t * lv_demo_truck_camera_controller(lv_obj_t * viewer)
{
    LV_UNUSED(viewer);
    lv_demo_truck_camera_controller_t * cameras;
    cameras = (lv_demo_truck_camera_controller_t *)lv_malloc(sizeof(*cameras));
    lv_memzero(cameras, sizeof(*cameras));

    lv_anim_init(&(cameras->anim_template));
    lv_anim_set_exec_cb(&(cameras->anim_template), (lv_anim_exec_xcb_t) camera_update_anim_cb);

    /* Set target of the Animation */
    lv_anim_set_var(&(cameras->anim_template), cameras);
    lv_anim_set_user_data(&(cameras->anim_template), cameras);

    /* Length of the Animation [ms] (this will change as soon as a valid speed is set)*/
    lv_anim_set_duration(&(cameras->anim_template), 1000);
    /* Loop forever */
    lv_anim_set_repeat_count(&(cameras->anim_template), LV_ANIM_REPEAT_INFINITE);
    /* Set start and end values. E.g. 0, 150 */
    lv_anim_set_values(&(cameras->anim_template), 0, 10000);
    cameras->running_anim = lv_anim_start(&(cameras->anim_template));

    return cameras;
}

static lv_demo_truck_interior_controller_t * lv_demo_truck_interior(lv_obj_t * viewer)
{

    lv_gltf_model_t * model = lv_gltf_get_primary_model(viewer);
    lv_demo_truck_interior_controller_t * interior;
    interior = (lv_demo_truck_interior_controller_t *)lv_malloc(sizeof(*interior));
    lv_memzero(interior, sizeof(*interior));
    interior->node_steering_wheel = lv_gltf_model_node_get_by_path(model, node_steering_wheel_path);
    interior->node_speedometer_needle = lv_gltf_model_node_get_by_path(model, node_speedometer_needle_path);
    interior->node_tachometer_needle = lv_gltf_model_node_get_by_path(model, node_tachometer_needle_path);
    interior->node_left_turn_indicator = lv_gltf_model_node_get_by_path(model, node_left_turn_indicator_path);
    interior->node_right_turn_indicator = lv_gltf_model_node_get_by_path(model, node_right_turn_indicator_path);

    lv_anim_init(&(interior->anim_template));
    lv_anim_set_exec_cb(&(interior->anim_template), (lv_anim_exec_xcb_t) interior_update_anim_cb);

    /* Set target of the Animation */
    lv_anim_set_var(&(interior->anim_template), interior);
    lv_anim_set_user_data(&(interior->anim_template), interior);

    /* Length of the Animation [ms] (this will change as soon as a valid speed is set)*/
    lv_anim_set_duration(&(interior->anim_template), 1000);
    /* Loop forever */
    lv_anim_set_repeat_count(&(interior->anim_template), LV_ANIM_REPEAT_INFINITE);
    /* Set start and end values. E.g. 0, 150 */
    lv_anim_set_values(&(interior->anim_template), 0, 10000);
    interior->running_anim = lv_anim_start(&(interior->anim_template));

    return interior;
}

static lv_demo_truck_hatch_t * lv_demo_truck_hatch(lv_obj_t * viewer, const char * node_path, uint32_t length_ms,
                                                   float open_degrees, float closed_degrees, lv_demo_truck_hatch_type hatch_type)
{
    lv_demo_truck_hatch_t * hatch;
    hatch = (lv_demo_truck_hatch_t *)lv_malloc(sizeof(*hatch));
    lv_memzero(hatch, sizeof(*hatch));

    hatch->open_degrees = open_degrees;
    hatch->closed_degrees = closed_degrees;
    hatch->length_ms = length_ms;
    hatch->hatch_type = hatch_type;

    lv_gltf_model_t * model = lv_gltf_get_primary_model(viewer);
    hatch->node = lv_gltf_model_node_get_by_path(model, node_path);

    lv_anim_init(&(hatch->anim_template));
    switch(hatch_type) {
        case LV_TRUCK_DOOR:
            lv_anim_set_exec_cb(&(hatch->anim_template), (lv_anim_exec_xcb_t) hatch_open_close_on_x_anim_cb);
            break;
        case LV_TRUCK_WINDOW:
            lv_anim_set_exec_cb(&(hatch->anim_template), (lv_anim_exec_xcb_t) hatch_open_close_on_y_anim_cb);
            break;
        case LV_TRUCK_TRUNKHOOD:
            lv_anim_set_exec_cb(&(hatch->anim_template), (lv_anim_exec_xcb_t) hatch_open_close_on_z_anim_cb);
            break;
        case LV_TRUCK_SUNROOF:
            lv_anim_set_exec_cb(&(hatch->anim_template), (lv_anim_exec_xcb_t) hatch_open_close_slide_z_anim_cb);
            break;
        default:
            lv_anim_set_exec_cb(&(hatch->anim_template), (lv_anim_exec_xcb_t) hatch_open_close_on_x_anim_cb);
            break;
    }
    lv_anim_set_completed_cb(&(hatch->anim_template), (lv_anim_completed_cb_t) hatch_open_close_complete_anim_cb);

    /* Set target of the Animation */
    lv_anim_set_var(&(hatch->anim_template), hatch);
    lv_anim_set_user_data(&(hatch->anim_template), hatch);

    /* Length of the Animation [ms] */
    lv_anim_set_duration(&(hatch->anim_template), (int32_t)hatch->length_ms);

    /* Set start and end values. E.g. 0, 150 */
    lv_anim_set_values(&(hatch->anim_template), (int32_t)(hatch->closed_degrees * 100.f),
                       (int32_t)(hatch->open_degrees * 100.f));

    hatch->last_set_value = (int32_t)(hatch->closed_degrees * 100.f);

    return hatch;
}

static lv_demo_truck_tire_t * lv_demo_truck_tire(
    lv_obj_t * viewer,
    const char * node_steering_path,
    const char * node_spin_path,
    const char * node_tire_type1_path,
    const char * node_tire_type2_path)
{

    lv_demo_truck_tire_t * tire;
    tire = (lv_demo_truck_tire_t *)lv_malloc(sizeof(*tire));
    lv_memzero(tire, sizeof(*tire));

    lv_gltf_model_t * model = lv_gltf_get_primary_model(viewer);
    tire->node_steering = (node_steering_path == NULL) ? NULL : lv_gltf_model_node_get_by_path(model, node_steering_path);
    tire->node_spin = lv_gltf_model_node_get_by_path(model, node_spin_path);
    tire->node_tire_type1 = lv_gltf_model_node_get_by_path(model, node_tire_type1_path);
    tire->node_tire_type2 = lv_gltf_model_node_get_by_path(model, node_tire_type2_path);

    lv_anim_init(&(tire->anim_template));
    lv_anim_set_exec_cb(&(tire->anim_template), (lv_anim_exec_xcb_t) tire_spin_on_z_anim_cb);

    /* Set target of the Animation */
    lv_anim_set_var(&(tire->anim_template), tire);
    lv_anim_set_user_data(&(tire->anim_template), tire);

    /* Length of the Animation [ms] (this will change as soon as a valid speed is set)*/
    lv_anim_set_duration(&(tire->anim_template), 1000);
    /* Loop forever */
    lv_anim_set_repeat_count(&(tire->anim_template), LV_ANIM_REPEAT_INFINITE);
    /* Set start and end values. E.g. 0, 150 */
    lv_anim_set_values(&(tire->anim_template), 0, 36000);
    tire->running_anim = lv_anim_start(&(tire->anim_template));

    return tire;
}

static lv_demo_truck_tireset_controller_t * lv_demo_truck_tireset_controller(
    lv_obj_t * viewer,
    lv_demo_truck_tire_type tire_type)
{

    lv_demo_truck_tireset_controller_t * tireset;
    tireset = (lv_demo_truck_tireset_controller_t *)lv_malloc(sizeof(*tireset));
    lv_memzero(tireset, sizeof(*tireset));

    tireset->tire_FD_spin = lv_demo_truck_tire(viewer, steer_FD_path, tire_FD_path, tire_FD_type1_path, tire_FD_type2_path);
    tireset->tire_FP_spin = lv_demo_truck_tire(viewer, steer_FP_path, tire_FP_path, tire_FP_type1_path, tire_FP_type2_path);
    tireset->tire_BDP_spin = lv_demo_truck_tire(viewer, NULL, tire_BDP_path, tire_BDP_type1_path, tire_BDP_type2_path);
    set_tireset_type(tireset, tire_type);
    return tireset;
}

static lv_demo_truck_lightset_controller_t * lv_demo_truck_lightset_controller(
    lv_obj_t * viewer)
{

    lv_demo_truck_lightset_controller_t * lightset;
    lightset = (lv_demo_truck_lightset_controller_t *)lv_malloc(sizeof(*lightset));
    lv_memzero(lightset, sizeof(*lightset));

    lv_gltf_model_t * model = lv_gltf_get_primary_model(viewer);
    lightset->node_left_blinker_on = lv_gltf_model_node_get_by_path(model, node_left_blinker_on_path);
    lightset->node_left_blinker_off = lv_gltf_model_node_get_by_path(model, node_left_blinker_off_path);
    lightset->node_right_blinker_on = lv_gltf_model_node_get_by_path(model, node_right_blinker_on_path);
    lightset->node_right_blinker_off = lv_gltf_model_node_get_by_path(model, node_right_blinker_off_path);
    lightset->node_brakes_on = lv_gltf_model_node_get_by_path(model, node_brakes_on_path);
    lightset->node_brakes_off = lv_gltf_model_node_get_by_path(model, node_brakes_off_path);
    lightset->node_lights_low_on = lv_gltf_model_node_get_by_path(model, node_lights_low_on_path);
    lightset->node_lights_low_off = lv_gltf_model_node_get_by_path(model, node_lights_low_off_path);
    lightset->node_lights_high_on = lv_gltf_model_node_get_by_path(model, node_lights_high_on_path);
    lightset->node_lights_high_off = lv_gltf_model_node_get_by_path(model, node_lights_high_off_path);

    lv_anim_init(&(lightset->anim_template));
    lv_anim_set_exec_cb(&(lightset->anim_template), (lv_anim_exec_xcb_t) blinker_lights_anim_cb);

    /* Set target of the Animation */
    lv_anim_set_var(&(lightset->anim_template), lightset);
    lv_anim_set_user_data(&(lightset->anim_template), lightset);

    /* Length of the Animation [ms] (this will change as soon as a valid speed is set)*/
    lv_anim_set_duration(&(lightset->anim_template), 667);
    /* Loop forever */
    lv_anim_set_repeat_count(&(lightset->anim_template), LV_ANIM_REPEAT_INFINITE);
    /* Set start and end values. E.g. 0, 150 */
    lv_anim_set_values(&(lightset->anim_template), 0, 1000);
    lightset->running_anim = lv_anim_start(&(lightset->anim_template));

    return lightset;
}

static lv_demo_truck_wipers_controller_t * lv_demo_truck_wipers_controller(
    lv_obj_t * viewer)
{

    lv_demo_truck_wipers_controller_t * wipers;
    wipers = (lv_demo_truck_wipers_controller_t *)lv_malloc(sizeof(*wipers));
    lv_memzero(wipers, sizeof(*wipers));

    lv_gltf_model_t * model = lv_gltf_get_primary_model(viewer);
    wipers->node_left_wiper = lv_gltf_model_node_get_by_path(model, node_left_wiper_path);
    wipers->node_right_wiper = lv_gltf_model_node_get_by_path(model, node_right_wiper_path);

    wipers->wipers_setting = LV_WIPERS_OFF;
    wipers->next_wipers_setting = LV_WIPERS_OFF;
    wipers->last_anim_value = 10000;

    lv_anim_init(&(wipers->anim_template));
    lv_anim_set_exec_cb(&(wipers->anim_template), (lv_anim_exec_xcb_t) wipers_anim_cb);

    /* Set target of the Animation */
    lv_anim_set_var(&(wipers->anim_template), wipers);
    lv_anim_set_user_data(&(wipers->anim_template), wipers);

    /* Length of the Animation [ms] (this will change as soon as a valid speed is set)*/
    lv_anim_set_duration(&(wipers->anim_template), 4000);
    /* Loop forever */
    lv_anim_set_repeat_count(&(wipers->anim_template), LV_ANIM_REPEAT_INFINITE);
    /* Set start and end values. E.g. 0, 150 */
    lv_anim_set_values(&(wipers->anim_template), 0, 10000);
    lv_anim_delete(&(wipers->anim_template), NULL);
    wipers->running_anim = NULL;

    return wipers;
}

static lv_demo_truck_paintset_t * lv_demo_truck_paintset(lv_obj_t * viewer, const char * body_node_path,
                                                         const char * hood_node_path,
                                                         const char * door_FD_node_path, const char * door_FP_node_path, const char * door_BD_node_path,
                                                         const char * door_BP_node_path, const char * tailgate_node_path)
{
    lv_demo_truck_paintset_t * paintset;
    paintset = (lv_demo_truck_paintset_t *)lv_malloc(sizeof(*paintset));
    lv_memzero(paintset, sizeof(*paintset));

    lv_gltf_model_t * model = lv_gltf_get_primary_model(viewer);
    paintset->body_node = lv_gltf_model_node_get_by_path(model, body_node_path);
    paintset->hood_node = lv_gltf_model_node_get_by_path(model, hood_node_path);
    paintset->door_FD_node = lv_gltf_model_node_get_by_path(model, door_FD_node_path);
    paintset->door_FP_node = lv_gltf_model_node_get_by_path(model, door_FP_node_path);
    paintset->door_BD_node = lv_gltf_model_node_get_by_path(model, door_BD_node_path);
    paintset->door_BP_node = lv_gltf_model_node_get_by_path(model, door_BP_node_path);
    paintset->tailgate_node = lv_gltf_model_node_get_by_path(model, tailgate_node_path);
    return paintset;
}

/*********************************
 *   STATIC ANIMATION CALLBACKS
 *********************************/

static void hatch_open_close_complete_anim_cb(lv_anim_t * obj)
{
    LV_ASSERT_NULL(obj);
    lv_demo_truck_hatch_t * hatch = (lv_demo_truck_hatch_t *)lv_anim_get_user_data(obj);
    LV_ASSERT_NULL(hatch);
    hatch->running_anim = NULL;
    hatch_update_checkbox(hatch);
}

static void hatch_update_checkbox(lv_demo_truck_hatch_t * hatch)
{
    LV_ASSERT_NULL(hatch);

    if(hatch->hatch_state == LV_HATCH_CLOSED) {
        lv_obj_add_state(hatch->checkbox, LV_STATE_CHECKED);
    }
    else {
        lv_obj_remove_state(hatch->checkbox, LV_STATE_CHECKED);
    }

    if((door_FD_open_close->hatch_state != LV_HATCH_CLOSED) || (door_FP_open_close->hatch_state != LV_HATCH_CLOSED) ||
       (door_BD_open_close->hatch_state != LV_HATCH_CLOSED) || (door_BP_open_close->hatch_state != LV_HATCH_CLOSED)) {
        lv_obj_is_hidden(close_all_doors_btn);
        lv_obj_set_hidden(open_all_doors_btn, true);
    }
    else {
        lv_obj_is_hidden(open_all_doors_btn);
        lv_obj_set_hidden(close_all_doors_btn, true);
    }

    if((window_FD_open_close->hatch_state != LV_HATCH_CLOSED) || (window_FP_open_close->hatch_state != LV_HATCH_CLOSED) ||
       (window_BD_open_close->hatch_state != LV_HATCH_CLOSED) || (window_BP_open_close->hatch_state != LV_HATCH_CLOSED)) {
        lv_obj_is_hidden(close_all_windows_btn);
        lv_obj_set_hidden(open_all_windows_btn, true);
    }
    else {
        lv_obj_is_hidden(open_all_windows_btn);
        lv_obj_set_hidden(close_all_windows_btn, true);
    }
}

static void hatch_open_close_on_x_anim_cb(lv_anim_t * obj, int32_t anim_value)
{
    LV_ASSERT_NULL(obj);
    lv_demo_truck_hatch_t * hatch = (lv_demo_truck_hatch_t *)lv_anim_get_user_data(obj);
    LV_ASSERT_NULL(hatch);
    hatch->last_set_value = anim_value;
    if(hatch->node != NULL) lv_gltf_model_node_set_rotation_x(hatch->node, ((float)anim_value / 100.f) * DEG_TO_RAD);
}

static void hatch_open_close_on_y_anim_cb(lv_anim_t * obj, int32_t anim_value)
{
    LV_ASSERT_NULL(obj);
    lv_demo_truck_hatch_t * hatch = (lv_demo_truck_hatch_t *)lv_anim_get_user_data(obj);
    LV_ASSERT_NULL(hatch);
    hatch->last_set_value = anim_value;
    if(hatch->node != NULL) lv_gltf_model_node_set_rotation_y(hatch->node, ((float)anim_value / 100.f) * DEG_TO_RAD);
}

static void hatch_open_close_on_z_anim_cb(lv_anim_t * obj, int32_t anim_value)
{
    LV_ASSERT_NULL(obj);
    lv_demo_truck_hatch_t * hatch = (lv_demo_truck_hatch_t *)lv_anim_get_user_data(obj);
    LV_ASSERT_NULL(hatch);
    hatch->last_set_value = anim_value;
    if(hatch->node != NULL) lv_gltf_model_node_set_rotation_z(hatch->node, ((float)anim_value / 100.f) * DEG_TO_RAD);
}

static void hatch_open_close_slide_z_anim_cb(lv_anim_t * obj, int32_t anim_value)
{
    LV_ASSERT_NULL(obj);
    lv_demo_truck_hatch_t * hatch = (lv_demo_truck_hatch_t *)lv_anim_get_user_data(obj);
    LV_ASSERT_NULL(hatch);
    hatch->last_set_value = anim_value;
    if(hatch->node != NULL) lv_gltf_model_node_set_position_z(hatch->node, ((float)anim_value / 100.f));
}

static void tire_spin_on_z_anim_cb(lv_anim_t * obj, int32_t anim_value)
{
    LV_UNUSED(anim_value);
    LV_ASSERT_NULL(obj);
    lv_demo_truck_tire_t * tire = (lv_demo_truck_tire_t *)lv_anim_get_user_data(obj);
    LV_ASSERT_NULL(tire);
    float current_spin = tire->actual_spin_angle;
    current_spin += tire->goal_spin_rate;
    while(current_spin > TWO_PI) {
        current_spin -= TWO_PI;
    }
    tire->actual_spin_angle = current_spin;
    if(tire->node_spin != NULL) lv_gltf_model_node_set_rotation_z(tire->node_spin, current_spin);
    if((tireset_controller != NULL) && (tire->node_steering != NULL)) {
        lv_gltf_model_node_set_rotation_x(tire->node_steering, tireset_controller->last_steer_angle);
    }
}

static void blinker_lights_anim_cb(lv_anim_t * obj, int32_t anim_value)
{
    LV_ASSERT_NULL(obj);
    lv_demo_truck_lightset_controller_t * lights = (lv_demo_truck_lightset_controller_t *)lv_anim_get_user_data(obj);
    LV_ASSERT_NULL(lights);
    bool lights_on = (anim_value > 500);
    switch(lights->blinker_setting) {
        case LV_BLINKER_LEFT:
            hide_node(lights_on ? lights->node_left_blinker_off : lights->node_left_blinker_on);
            show_node(lights_on ? lights->node_left_blinker_on : lights->node_left_blinker_off);
            hide_node(lights->node_right_blinker_on);
            show_node(lights->node_right_blinker_off);
            if(interior_controller != NULL) {
                hide_node(interior_controller->node_right_turn_indicator);
                if(lights_on) {
                    show_node(interior_controller->node_left_turn_indicator);
                }
                else {
                    hide_node(interior_controller->node_left_turn_indicator);
                }
            }
            break;
        case LV_BLINKER_RIGHT:
            hide_node(lights_on ? lights->node_right_blinker_off : lights->node_right_blinker_on);
            show_node(lights_on ? lights->node_right_blinker_on : lights->node_right_blinker_off);
            hide_node(lights->node_left_blinker_on);
            show_node(lights->node_left_blinker_off);
            hide_node(interior_controller->node_left_turn_indicator);
            if(interior_controller != NULL) {
                if(lights_on) {
                    show_node(interior_controller->node_right_turn_indicator);
                }
                else {
                    hide_node(interior_controller->node_right_turn_indicator);
                }
            }
            break;
        case LV_BLINKER_HAZARD:
            hide_node(lights_on ? lights->node_left_blinker_off : lights->node_left_blinker_on);
            show_node(lights_on ? lights->node_left_blinker_on : lights->node_left_blinker_off);
            hide_node(lights_on ? lights->node_right_blinker_off : lights->node_right_blinker_on);
            show_node(lights_on ? lights->node_right_blinker_on : lights->node_right_blinker_off);
            if(interior_controller != NULL) {
                if(lights_on) {
                    show_node(interior_controller->node_left_turn_indicator);
                    show_node(interior_controller->node_right_turn_indicator);
                }
                else {
                    hide_node(interior_controller->node_left_turn_indicator);
                    hide_node(interior_controller->node_right_turn_indicator);
                }
            }
            break;
        case LV_BLINKER_NONE:
        default:
            hide_node(lights->node_left_blinker_on);
            hide_node(lights->node_right_blinker_on);
            show_node(lights->node_left_blinker_off);
            show_node(lights->node_right_blinker_off);
            if(interior_controller != NULL) {
                hide_node(interior_controller->node_left_turn_indicator);
                hide_node(interior_controller->node_right_turn_indicator);
            }
            break;
    }

    if(lights->brakes_active) {
        hide_node(lights->node_brakes_off);
        show_node(lights->node_brakes_on);
    }
    else {
        hide_node(lights->node_brakes_on);
        show_node(lights->node_brakes_off);
    }

    if(lights->last_applied_headlights_setting != lights->headlights_setting) {
        lights->last_applied_headlights_setting = lights->headlights_setting;
        switch(lights->headlights_setting) {
            case LV_HEADLIGHTS_LOW:
                hide_node(lights->node_lights_low_off);
                hide_node(lights->node_lights_high_on);
                show_node(lights->node_lights_low_on);
                show_node(lights->node_lights_high_off);
                break;
            case LV_HEADLIGHTS_HIGH:
                hide_node(lights->node_lights_low_off);
                hide_node(lights->node_lights_high_off);
                show_node(lights->node_lights_low_on);
                show_node(lights->node_lights_high_on);
                break;
            case LV_HEADLIGHTS_OFF:
            default:
                hide_node(lights->node_lights_low_on);
                hide_node(lights->node_lights_high_on);
                show_node(lights->node_lights_low_off);
                show_node(lights->node_lights_high_off);
                break;
        }

    }

}

static void camera_update_anim_cb(lv_anim_t * obj, int32_t anim_value)
{
    LV_UNUSED(anim_value);
    LV_ASSERT_NULL(obj);
    lv_demo_truck_camera_controller_t * cameras = (lv_demo_truck_camera_controller_t *)lv_anim_get_user_data(obj);
    LV_ASSERT_NULL(cameras);
}

static void interior_update_anim_cb(lv_anim_t * obj, int32_t anim_value)
{
    LV_UNUSED(anim_value);
    LV_ASSERT_NULL(obj);
    lv_demo_truck_interior_controller_t * interior = (lv_demo_truck_interior_controller_t *)lv_anim_get_user_data(obj);
    LV_ASSERT_NULL(interior);
    if(interior->node_steering_wheel != NULL) {
        float wheel_angle = tireset_controller->last_steer_angle;
        const float MAX_STEERING_WHEEL_TURN_RATE = ((PI * 0.06125f) / (1000.f / LV_DEF_REFR_PERIOD));
        const float BLINKER_RESET_ANGLE = 6.f * DEG_TO_RAD;
        float turn_rate_scale = 1.f;
        if(wheel_angle < tireset_controller->goal_steer_angle) {
            /* Turning left */
            turn_rate_scale = (((tireset_controller->goal_steer_angle - wheel_angle) / MAX_STEERING_WHEEL_TURN_RATE) / 50.f) + 1.f;
            wheel_angle += MAX_STEERING_WHEEL_TURN_RATE * turn_rate_scale;
            if(wheel_angle > tireset_controller->goal_steer_angle) wheel_angle = tireset_controller->goal_steer_angle;
            if(lights_controller->blinker_setting == LV_BLINKER_LEFT) {
                if(wheel_angle > lights_controller->blinker_ext_angle) lights_controller->blinker_ext_angle = wheel_angle;
            }
            else if(lights_controller->blinker_setting == LV_BLINKER_RIGHT) {
                if(wheel_angle > lights_controller->blinker_ext_angle + BLINKER_RESET_ANGLE) set_lightset_blinker_type(
                        lights_controller, LV_BLINKER_NONE);
            }
        }
        else if(wheel_angle > tireset_controller->goal_steer_angle) {
            /* Turning right */
            turn_rate_scale = (((wheel_angle - tireset_controller->goal_steer_angle) / MAX_STEERING_WHEEL_TURN_RATE) / 50.f) + 1.f;
            wheel_angle -= MAX_STEERING_WHEEL_TURN_RATE * turn_rate_scale;
            if(wheel_angle < tireset_controller->goal_steer_angle) wheel_angle = tireset_controller->goal_steer_angle;
            if(lights_controller->blinker_setting == LV_BLINKER_RIGHT) {
                if(wheel_angle < lights_controller->blinker_ext_angle) lights_controller->blinker_ext_angle = wheel_angle;
            }
            else if(lights_controller->blinker_setting == LV_BLINKER_LEFT) {
                if(wheel_angle < lights_controller->blinker_ext_angle - BLINKER_RESET_ANGLE) set_lightset_blinker_type(
                        lights_controller, LV_BLINKER_NONE);
            }
        }
        tireset_controller->last_steer_angle = wheel_angle;
        lv_gltf_model_node_set_rotation_y(interior->node_steering_wheel, (wheel_angle * -18.f));
    }

    if(interior->node_speedometer_needle != NULL) {
        float needle_angle = ((tireset_controller->goal_speed_ratio * 270.f) - 135.f) * DEG_TO_RAD;
        lv_gltf_model_node_set_rotation_y(interior->node_speedometer_needle, needle_angle);
    }
    if(interior->node_tachometer_needle != NULL) {
        float needle_angle = ((tireset_controller->goal_speed_ratio * 125.f) - 105.f) * DEG_TO_RAD;
        needle_angle += (tireset_controller->tach_offset * 60.f) * DEG_TO_RAD;
        lv_gltf_model_node_set_rotation_y(interior->node_tachometer_needle, needle_angle);
    }
    tireset_controller->tach_offset *= 0.95f;
}

static void wipers_anim_cb(lv_anim_t * obj, int32_t anim_value)
{
    LV_ASSERT_NULL(obj);
    lv_demo_truck_wipers_controller_t * wipers = (lv_demo_truck_wipers_controller_t *)lv_anim_get_user_data(obj);
    LV_ASSERT_NULL(wipers);
    float wiper_angle = 0.f;

    const float max_wiper_angle = -85.f;
    const float max_passenger_wiper_angle = -115.f;
    const float passenger_wiper_ratio = max_passenger_wiper_angle / max_wiper_angle;

    if((uint32_t)anim_value < wipers->last_anim_value) {
        /* animation has cycled, can switch to other mode */
        if(wipers->next_wipers_setting != wipers->wipers_setting) {
            wipers->wipers_setting = wipers->next_wipers_setting;
        }
    }
    wipers->last_anim_value = anim_value;

    switch(wipers->wipers_setting) {
        case LV_WIPERS_INT:
            if(anim_value < 2000) {
                wiper_angle = ((float)anim_value / 2000.f) * max_wiper_angle;
            }
            else if(anim_value < 4000) {
                wiper_angle = max_wiper_angle - (((float)(anim_value - 2000) / 2000.f) * max_wiper_angle);
            }
            break;
        case LV_WIPERS_LOW:
            anim_value = anim_value % 5000;
            if(anim_value < 2000) {
                wiper_angle = ((float)anim_value / 2000.f) * max_wiper_angle;
            }
            else if(anim_value < 4000) {
                wiper_angle = max_wiper_angle - (((float)(anim_value - 2000) / 2000.f) * max_wiper_angle);
            }
            break;
        case LV_WIPERS_HIGH:
            anim_value = anim_value % 2500;
            if(anim_value < 1000) {
                wiper_angle = ((float)anim_value / 1000.f) * max_wiper_angle;
            }
            else if(anim_value < 2000) {
                wiper_angle = max_wiper_angle - (((float)(anim_value - 1000) / 1000.f) * max_wiper_angle);
            }
            break;
        case LV_WIPERS_OFF:
            wiper_angle = 0.f;
        default:
            break;
    }
    wiper_angle *= DEG_TO_RAD;

    if(wiper_angle == 0.f) {
        /* animation is at bottom of cycle, can switch to off */
        if(wipers->next_wipers_setting != wipers->wipers_setting) {
            wipers->wipers_setting = LV_WIPERS_OFF;
        }
    }

    if(wipers->wipers_setting == LV_WIPERS_OFF) {
        if(wipers->running_anim != NULL) {
            lv_anim_delete(wipers->running_anim, NULL);
            wipers->running_anim = NULL;
        }
        if(wipers->next_wipers_setting != LV_WIPERS_OFF) {
            wipers->running_anim = lv_anim_start(&(wipers->anim_template));
            wipers->wipers_setting = wipers->next_wipers_setting;
            wipers->last_anim_value = 10000;
        }
    }

    if(wipers->node_left_wiper != NULL) lv_gltf_model_node_set_rotation_x(wipers->node_left_wiper, wiper_angle);
    if(wipers->node_right_wiper != NULL) lv_gltf_model_node_set_rotation_x(wipers->node_right_wiper,
                                                                               wiper_angle * passenger_wiper_ratio);
}

/***********************************
 *   STATIC INPUT EVENT CALLBACKS
 ***********************************/

static void on_mouse_event(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * viewer = lv_event_get_target_obj(e);
    lv_indev_t * indev = lv_indev_active();
    mouse_event_data_t * mouse_state = (mouse_event_data_t *)lv_event_get_user_data(e);

    if(!mouse_state) {
        return;
    }

    lv_point_t current_pos;
    lv_indev_get_point(indev, &current_pos);

    switch(event_code) {
        case LV_EVENT_PRESSED:
            mouse_state->is_dragging = true;
            mouse_state->last_pos = current_pos;
            break;
        case LV_EVENT_PRESSING:
            if(mouse_state->is_dragging && lv_gltf_get_camera(viewer) == 0) {
                int32_t delta_x = current_pos.x - mouse_state->last_pos.x;
                int32_t delta_y = current_pos.y - mouse_state->last_pos.y;

                float current_yaw = lv_gltf_get_yaw(viewer);
                float current_pitch = lv_gltf_get_pitch(viewer);

                float new_yaw = current_yaw + (delta_x * -mouse_state->sensitivity);
                float new_pitch = current_pitch + (delta_y * -mouse_state->sensitivity);

                if(new_pitch > 89.0f)
                    new_pitch = 89.0f;
                if(new_pitch < -89.0f)
                    new_pitch = -89.0f;
                if(new_pitch > 0)
                    new_pitch = 0;


                lv_subject_set_float(&yaw_subject, new_yaw);
                lv_subject_set_float(&pitch_subject, new_pitch);
            }
            mouse_state->last_pos = current_pos;
            break;

        case LV_EVENT_RELEASED:
        case LV_EVENT_PRESS_LOST:
            mouse_state->is_dragging = false;
            break;
        case LV_EVENT_DELETE:
            lv_free(mouse_state);
        default:
            break;
    }
}

static void show_foldout(lv_demo_foldout_t * foldout)
{
    if(last_opened_foldout != NULL) {
        hide_foldout(last_opened_foldout);
    }
    last_opened_foldout = foldout;
    foldout->contents_visible = true;
    if(foldout->contents != NULL) {
        lv_obj_set_hidden(foldout->contents, false);
    }
    if(foldout->title != NULL) {
        lv_obj_set_hidden(foldout->title, false);
    }
    if(foldout->title_button != NULL) {
        lv_obj_set_hidden(foldout->title_button, true);
    }
}

static void hide_foldout(lv_demo_foldout_t * foldout)
{
    last_opened_foldout = NULL;
    foldout->contents_visible = false;
    if(foldout->contents != NULL) {
        lv_obj_set_hidden(foldout->contents, true);
    }
    if(foldout->title != NULL) {
        lv_obj_set_hidden(foldout->title, true);
    }
    if(foldout->title_button != NULL) {
        lv_obj_set_hidden(foldout->title_button, false);
    }
}

static void show_foldout_event_cb(lv_event_t * e)
{
    lv_demo_foldout_t * foldout = (lv_demo_foldout_t *)lv_event_get_user_data(e);
    show_foldout(foldout);
}

static void hide_foldout_event_cb(lv_event_t * e)
{
    lv_demo_foldout_t * foldout = (lv_demo_foldout_t *)lv_event_get_user_data(e);
    hide_foldout(foldout);
}

static void open_all_doors_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    open_all_doors();
}
static void close_all_doors_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    close_all_doors();
}
static void toggle_door_FD_event_handler(lv_event_t * e)
{
    if(lv_obj_has_state(lv_event_get_target_obj(e), LV_STATE_CHECKED)) {
        close_hatch(door_FD_open_close);
    }
    else {
        open_hatch(door_FD_open_close);
    }
}
static void toggle_door_FP_event_handler(lv_event_t * e)
{
    if(lv_obj_has_state(lv_event_get_target_obj(e), LV_STATE_CHECKED)) {
        close_hatch(door_FP_open_close);
    }
    else {
        open_hatch(door_FP_open_close);
    }
}
static void toggle_door_BD_event_handler(lv_event_t * e)
{
    if(lv_obj_has_state(lv_event_get_target_obj(e), LV_STATE_CHECKED)) {
        close_hatch(door_BD_open_close);
    }
    else {
        open_hatch(door_BD_open_close);
    }
}
static void toggle_door_BP_event_handler(lv_event_t * e)
{
    if(lv_obj_has_state(lv_event_get_target_obj(e), LV_STATE_CHECKED)) {
        close_hatch(door_BP_open_close);
    }
    else {
        open_hatch(door_BP_open_close);
    }
}
static void toggle_hood_event_handler(lv_event_t * e)
{
    if(lv_obj_has_state(lv_event_get_target_obj(e), LV_STATE_CHECKED)) {
        close_hatch(hood_open_close);
    }
    else {
        open_hatch(hood_open_close);
    }
}
static void toggle_tailgate_event_handler(lv_event_t * e)
{
    if(lv_obj_has_state(lv_event_get_target_obj(e), LV_STATE_CHECKED)) {
        close_hatch(tailgate_open_close);
    }
    else {
        open_hatch(tailgate_open_close);
    }
}
static void open_all_windows_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    open_all_windows();
}
static void close_all_windows_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    close_all_windows();
}
static void select_paintset_A_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    select_paintset_A();
}
static void select_paintset_B_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    select_paintset_B();
}
static void select_paintset_C_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    select_paintset_C();
}

static void toggle_mud_event_handler(lv_event_t * e)
{
    has_mud = !has_mud;
    apply_dirt(has_mud);
    lv_obj_t * btn = lv_event_get_target_obj(e);
    lv_obj_t * btn_label = lv_obj_get_child(btn, 0);
    if(has_mud) {
        lv_label_set_text_static(btn_label, "Remove mud");
        lv_obj_remove_state(btn, LV_STATE_CHECKED);
    }
    else {
        lv_obj_add_state(btn, LV_STATE_CHECKED);
        lv_label_set_text_static(btn_label, "Add mud");
    }
}
static void select_speed_event_handler(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    int32_t value = lv_slider_get_value(slider);
    float goal_spin = ((float)value / MAX_SPIN_SLIDER_VALUE);
    set_tireset_speed_ratio(tireset_controller, goal_spin);
}
static void select_mph_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    set_tireset_speed_type(tireset_controller, LV_SPEED_MPH);
}
static void select_kmph_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    set_tireset_speed_type(tireset_controller, LV_SPEED_KMPH);
}
static void select_steering_event_handler(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    int32_t value = lv_slider_get_value(slider);
    float goal_steer = (float)value / MIN_MAX_STEER_SLIDER_VALUE;
    set_tireset_steer_ratio(tireset_controller, goal_steer);
}
static void select_left_blinker_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    set_lightset_blinker_type(lights_controller, LV_BLINKER_LEFT);
}
static void select_right_blinker_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    set_lightset_blinker_type(lights_controller, LV_BLINKER_RIGHT);
}
static void select_hazard_blinker_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    set_lightset_blinker_type(lights_controller, LV_BLINKER_HAZARD);
}

static void select_headlights_low_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    set_lightset_headlight_type(lights_controller, LV_HEADLIGHTS_LOW);
}
static void select_headlights_high_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    set_lightset_headlight_type(lights_controller, LV_HEADLIGHTS_HIGH);
}

static void select_wipers_low_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    set_wiper_speed(wipers_controller, LV_WIPERS_INT);
}
static void select_wipers_med_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    set_wiper_speed(wipers_controller, LV_WIPERS_LOW);
}
static void select_wipers_high_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    set_wiper_speed(wipers_controller, LV_WIPERS_HIGH);
}

static void select_camera_interior_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    set_camera_num(camera_controller, LV_CAMERA_INTERIOR);
}
static void select_camera_exterior_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    set_camera_num(camera_controller, LV_CAMERA_EXTERIOR);
}

static void select_camera_free_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    set_camera_num(camera_controller, LV_CAMERA_UNSET);
}
static void toggle_antialias_event_handler(lv_event_t * e)
{
    if(lv_obj_has_state(lv_event_get_target_obj(e), LV_STATE_CHECKED)) {
        enable_antialiasing(true);
    }
    else {
        enable_antialiasing(false);
    }
}

static void gas_pressing_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    const float SPEED_GAIN_PER_SECOND_OF_GAS = 25.f;
    const float SPEED_GAIN_PER_FRAME_OF_GAS = SPEED_GAIN_PER_SECOND_OF_GAS / (float)LV_DEF_REFR_PERIOD;
    const float SPEED_GAIN_NORM_PER_FRAME_OF_GAS = SPEED_GAIN_PER_FRAME_OF_GAS / MAX_SPEED_KMH;

    float new_norm = tireset_controller->goal_speed_ratio + SPEED_GAIN_NORM_PER_FRAME_OF_GAS;
    tireset_controller->tach_offset += (SPEED_GAIN_NORM_PER_FRAME_OF_GAS * 5.f);
    tireset_controller->tach_offset = tireset_controller->tach_offset > 1.0f ? 1.0f : tireset_controller->tach_offset;
    new_norm = new_norm > 1.0f ? 1.0f : new_norm;
    new_norm = new_norm < 0.0f ? 0.0f : new_norm;
    int32_t new_slider_value = new_norm * MAX_SPIN_SLIDER_VALUE;
    lv_slider_set_value(tireset_controller->slider_speed, new_slider_value, LV_ANIM_OFF);
    set_tireset_speed_ratio(tireset_controller, new_norm);
}
static void brakes_press_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    lights_controller->brakes_active = true;
}
static void brakes_pressing_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    const float SPEED_LOST_PER_SECOND_OF_BRAKES = 15.f;
    const float SPEED_LOST_PER_FRAME_OF_BRAKES = SPEED_LOST_PER_SECOND_OF_BRAKES / (float)LV_DEF_REFR_PERIOD;
    const float SPEED_LOSS_NORM_PER_FRAME_OF_BRAKES = SPEED_LOST_PER_FRAME_OF_BRAKES / MAX_SPEED_KMH;

    float new_norm = tireset_controller->goal_speed_ratio - SPEED_LOSS_NORM_PER_FRAME_OF_BRAKES;
    tireset_controller->tach_offset -= (SPEED_LOSS_NORM_PER_FRAME_OF_BRAKES * 8.f);
    tireset_controller->tach_offset = tireset_controller->tach_offset < -1.0f ? -1.0f : tireset_controller->tach_offset;
    new_norm = new_norm > 1.0f ? 1.0f : new_norm;
    new_norm = new_norm < 0.0f ? 0.0f : new_norm;
    int32_t new_slider_value = new_norm * MAX_SPIN_SLIDER_VALUE;
    lv_slider_set_value(tireset_controller->slider_speed, new_slider_value, LV_ANIM_OFF);
    set_tireset_speed_ratio(tireset_controller, new_norm);
}
static void brakes_released_event_handler(lv_event_t * e)
{
    LV_UNUSED(e);
    lights_controller->brakes_active = false;
}
static void toggle_window_FD_event_handler(lv_event_t * e)
{
    if(lv_obj_has_state(lv_event_get_target_obj(e), LV_STATE_CHECKED)) {
        close_hatch(window_FD_open_close);
    }
    else {
        open_hatch(window_FD_open_close);
    }
}
static void toggle_window_FP_event_handler(lv_event_t * e)
{
    if(lv_obj_has_state(lv_event_get_target_obj(e), LV_STATE_CHECKED)) {
        close_hatch(window_FP_open_close);
    }
    else {
        open_hatch(window_FP_open_close);
    }
}
static void toggle_window_BD_event_handler(lv_event_t * e)
{
    if(lv_obj_has_state(lv_event_get_target_obj(e), LV_STATE_CHECKED)) {
        close_hatch(window_BD_open_close);
    }
    else {
        open_hatch(window_BD_open_close);
    }
}
static void toggle_window_BP_event_handler(lv_event_t * e)
{
    if(lv_obj_has_state(lv_event_get_target_obj(e), LV_STATE_CHECKED)) {
        close_hatch(window_BP_open_close);
    }
    else {
        open_hatch(window_BP_open_close);
    }
}
static void toggle_sunroof_event_handler(lv_event_t * e)
{
    if(lv_obj_has_state(lv_event_get_target_obj(e), LV_STATE_CHECKED)) {
        close_hatch(sunroof_open_close);
    }
    else {
        open_hatch(sunroof_open_close);
    }
}
static double distance_per_revolution(double tire_radius)
{
    return 2 * PI * tire_radius; // distance = circumference
}


static double revolution_rate(double tire_radius, double travel_rate_kmh)
{
    double distance_per_revolution_meters = distance_per_revolution(tire_radius);
    double distance_per_minute = (travel_rate_kmh * 1000) / 60; // Convert km/h to m/min
    return distance_per_minute / distance_per_revolution_meters; // RPM
}


/**************************
*   STATIC STATE SETTERS
***************************/

static void set_tire_type(lv_demo_truck_tire_t * tire, lv_demo_truck_tire_type tire_type)
{
    switch(tire_type) {
        case LV_TIRE_CLEAN:
            show_node(tire->node_tire_type1);
            hide_node(tire->node_tire_type2);
            break;
        case LV_TIRE_DIRTY:
            hide_node(tire->node_tire_type1);
            show_node(tire->node_tire_type2);
            break;
        default:
            show_node(tire->node_tire_type1);
            hide_node(tire->node_tire_type2);
            break;
    }
}

static void set_tireset_type(lv_demo_truck_tireset_controller_t * tireset, lv_demo_truck_tire_type tire_type)
{
    tireset->tire_type = tire_type;
    set_tire_type(tireset->tire_FD_spin, tire_type);
    set_tire_type(tireset->tire_FP_spin, tire_type);
    set_tire_type(tireset->tire_BDP_spin, tire_type);
}

static void set_tireset_speed_type(lv_demo_truck_tireset_controller_t * tireset, lv_demo_truck_speed_type speed_type)
{
    tireset->speed_type = speed_type;
    if(speed_type == LV_SPEED_MPH) {
        lv_obj_add_state(tireset->checkbox_mph, LV_STATE_CHECKED);
        lv_obj_remove_state(tireset->checkbox_kmph, LV_STATE_CHECKED);
        lv_label_set_text_static(tireset->label_speed_type, "mph");
    }
    else {
        lv_obj_add_state(tireset->checkbox_kmph, LV_STATE_CHECKED);
        lv_obj_remove_state(tireset->checkbox_mph, LV_STATE_CHECKED);
        lv_label_set_text_static(tireset->label_speed_type, "km/h");
    }
    set_tireset_speed_ratio(tireset, tireset->goal_speed_ratio);
}

static void set_lightset_blinker_type(lv_demo_truck_lightset_controller_t * lights,
                                      lv_demo_truck_blinker_setting blinker_type)
{
    if(lights->blinker_setting == blinker_type) {
        blinker_type = LV_BLINKER_NONE;
    }
    lv_obj_remove_state(left_blinker_btn, LV_STATE_CHECKED);
    lv_obj_remove_state(right_blinker_btn, LV_STATE_CHECKED);
    lv_obj_remove_state(hazard_blinker_btn, LV_STATE_CHECKED);
    lights->blinker_setting = blinker_type;
    lights->blinker_set_angle = tireset_controller->last_steer_angle;
    lights->blinker_ext_angle = tireset_controller->last_steer_angle;
    switch(blinker_type) {
        case LV_BLINKER_UNSET:
        case LV_BLINKER_NONE:
            break;
        case LV_BLINKER_LEFT:
            lv_obj_add_state(left_blinker_btn, LV_STATE_CHECKED);
            break;
        case LV_BLINKER_RIGHT:
            lv_obj_add_state(right_blinker_btn, LV_STATE_CHECKED);
            break;
        case LV_BLINKER_HAZARD:
            lv_obj_add_state(hazard_blinker_btn, LV_STATE_CHECKED);
            break;
    }
}

static void set_lightset_headlight_type(lv_demo_truck_lightset_controller_t * lights,
                                        lv_demo_truck_headlights_setting headlights_type)
{
    if(lights->headlights_setting == headlights_type) {
        headlights_type = LV_HEADLIGHTS_OFF;
    }
    lights->headlights_setting = headlights_type;
    lv_obj_remove_state(lights->checkbox_headlights_low, LV_STATE_CHECKED);
    lv_obj_remove_state(lights->checkbox_headlights_high, LV_STATE_CHECKED);
    switch(headlights_type) {
        case LV_HEADLIGHTS_LOW:
            lv_obj_add_state(lights->checkbox_headlights_low, LV_STATE_CHECKED);
            break;
        case LV_HEADLIGHTS_HIGH:
            lv_obj_add_state(lights->checkbox_headlights_high, LV_STATE_CHECKED);
            break;
        case LV_HEADLIGHTS_OFF:
        default:
            break;
    }

}

static void set_camera_num(lv_demo_truck_camera_controller_t * cameras, lv_demo_truck_camera_setting camera_type)
{
    cameras->next_camera_setting = camera_type;
    lv_obj_remove_state(cameras->checkbox_camera_interior, LV_STATE_CHECKED);
    lv_obj_remove_state(cameras->checkbox_camera_exterior, LV_STATE_CHECKED);
    lv_obj_remove_state(cameras->checkbox_camera_free, LV_STATE_CHECKED);
    switch(camera_type) {
        case LV_CAMERA_INTERIOR:
            lv_obj_add_state(cameras->checkbox_camera_interior, LV_STATE_CHECKED);
            lv_gltf_set_camera(cameras->viewer, 2);
            break;
        case LV_CAMERA_EXTERIOR:
            lv_obj_add_state(cameras->checkbox_camera_exterior, LV_STATE_CHECKED);
            lv_gltf_set_camera(cameras->viewer, 1);
            break;
        case LV_CAMERA_UNSET:
        default:
            lv_obj_add_state(cameras->checkbox_camera_free, LV_STATE_CHECKED);
            lv_gltf_set_camera(cameras->viewer, 0);
            break;
    }

}

static void set_wiper_speed(lv_demo_truck_wipers_controller_t * wipers, lv_demo_truck_wipers_setting wipers_setting)
{
    if(wipers->next_wipers_setting == wipers_setting) {
        wipers_setting = LV_WIPERS_OFF;
    }
    //wipers->wipers_setting = wipers_setting;
    wipers->next_wipers_setting = wipers_setting;

    lv_obj_remove_state(wipers->btn_wipers_low, LV_STATE_CHECKED);
    lv_obj_remove_state(wipers->btn_wipers_med, LV_STATE_CHECKED);
    lv_obj_remove_state(wipers->btn_wipers_high, LV_STATE_CHECKED);
    switch(wipers_setting) {
        case LV_WIPERS_INT:
            lv_obj_add_state(wipers->btn_wipers_low, LV_STATE_CHECKED);
            break;
        case LV_WIPERS_LOW:
            lv_obj_add_state(wipers->btn_wipers_med, LV_STATE_CHECKED);
            break;
        case LV_WIPERS_HIGH:
            lv_obj_add_state(wipers->btn_wipers_high, LV_STATE_CHECKED);
            break;
        case LV_WIPERS_OFF:
        default:
            break;
    }
    if(wipers->running_anim == NULL) wipers->running_anim = lv_anim_start(&(wipers->anim_template));

}

static void viewer_observer_float_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * viewer = lv_observer_get_target_obj(observer);
    float value = lv_subject_get_float(subject);
    lv_gltf_set_float_fn_union_t fn_union = { .ptr = lv_observer_get_user_data(observer) };

    fn_union.cb(viewer, value);
}

static void set_tireset_speed_ratio(lv_demo_truck_tireset_controller_t * tireset, float max_speed_ratio)
{
    tireset->goal_speed_ratio = max_speed_ratio;
    float goal_spin = tireset->goal_speed_ratio * MAX_SPIN_RATE;
    tireset->goal_spin_rate = goal_spin;
    tireset->tire_FD_spin->goal_spin_rate = tireset->goal_spin_rate;
    tireset->tire_FP_spin->goal_spin_rate = tireset->goal_spin_rate;
    tireset->tire_BDP_spin->goal_spin_rate = tireset->goal_spin_rate;
    char speed[8];
    float conv_factor = 1.f;
    if(tireset->speed_type == LV_SPEED_MPH) {
        conv_factor = KM_TO_MILE;
    }
    lv_snprintf(speed, sizeof(speed), "%0.0f", (tireset->goal_speed_ratio  * MAX_SPEED_KMH * conv_factor));
    lv_label_set_text(tireset_controller->label_speed, speed);
}

static void set_tireset_steer_ratio(lv_demo_truck_tireset_controller_t * tireset, float goal_steer_ratio)
{
    tireset->goal_steer_ratio = goal_steer_ratio;
    tireset->goal_steer_angle = -(tireset->goal_steer_ratio * MAX_STEER_ANGLE);
}

static void select_paintset_A(void)
{
    hide_all_paintsets();
    show_paintset(paintset_red);
}

static void select_paintset_B(void)
{
    hide_all_paintsets();
    show_paintset(paintset_gray);
}

static void select_paintset_C(void)
{
    hide_all_paintsets();
    show_paintset(paintset_green);
}

static void hide_all_paintsets(void)
{
    hide_paintset(paintset_red);
    hide_paintset(paintset_gray);
    hide_paintset(paintset_green);
}

static void hide_node(lv_gltf_model_node_t * node)
{
    if(node == NULL) return;
    lv_gltf_model_node_set_scale_x(node, 0.f);
    lv_gltf_model_node_set_scale_y(node, 0.f);
    lv_gltf_model_node_set_scale_z(node, 0.f);
}

static void show_node(lv_gltf_model_node_t * node)
{
    if(node == NULL) return;
    lv_gltf_model_node_set_scale_x(node, 1.f);
    lv_gltf_model_node_set_scale_y(node, 1.f);
    lv_gltf_model_node_set_scale_z(node, 1.f);
}

static void show_paintset(lv_demo_truck_paintset_t * paintset)
{
    LV_ASSERT_NULL(paintset);
    lv_obj_add_state(paintset->button, LV_STATE_CHECKED);
    show_node(paintset->body_node);
    show_node(paintset->hood_node);
    show_node(paintset->door_FD_node);
    show_node(paintset->door_FP_node);
    show_node(paintset->door_BD_node);
    show_node(paintset->door_BP_node);
    show_node(paintset->tailgate_node);
}

static void hide_paintset(lv_demo_truck_paintset_t * paintset)
{
    LV_ASSERT_NULL(paintset);
    lv_obj_remove_state(paintset->button, LV_STATE_CHECKED);
    hide_node(paintset->body_node);
    hide_node(paintset->hood_node);
    hide_node(paintset->door_FD_node);
    hide_node(paintset->door_FP_node);
    hide_node(paintset->door_BD_node);
    hide_node(paintset->door_BP_node);
    hide_node(paintset->tailgate_node);
}

static void apply_dirt(bool truck_is_dirty)
{
    if(truck_is_dirty) {
        tireset_controller->tire_type = LV_TIRE_DIRTY;
        show_node(node_dirty_overlay_1);
        show_node(node_dirty_overlay_2);
        show_node(node_dirty_overlay_3);
    }
    else {
        tireset_controller->tire_type = LV_TIRE_CLEAN;
        hide_node(node_dirty_overlay_1);
        hide_node(node_dirty_overlay_2);
        hide_node(node_dirty_overlay_3);
    }
    set_tireset_type(tireset_controller, tireset_controller->tire_type);
}

static void enable_antialiasing(bool use_antialiasing)
{
    if(use_antialiasing) {
        lv_obj_add_state(checkbox_antialiasing, LV_STATE_CHECKED);
        lv_gltf_set_antialiasing_mode(camera_controller->viewer, LV_GLTF_AA_MODE_ON);
    }
    else {
        lv_obj_remove_state(checkbox_antialiasing, LV_STATE_CHECKED);
        lv_gltf_set_antialiasing_mode(camera_controller->viewer, LV_GLTF_AA_MODE_OFF);
    }
}

static void open_all_doors(void)
{
    open_hatch(door_FD_open_close);
    open_hatch(door_FP_open_close);
    open_hatch(door_BD_open_close);
    open_hatch(door_BP_open_close);
}

static void close_all_doors(void)
{
    close_hatch(door_FD_open_close);
    close_hatch(door_FP_open_close);
    close_hatch(door_BD_open_close);
    close_hatch(door_BP_open_close);
}

static void open_all_windows(void)
{
    open_hatch(window_FD_open_close);
    open_hatch(window_FP_open_close);
    open_hatch(window_BD_open_close);
    open_hatch(window_BP_open_close);
    open_hatch(sunroof_open_close);
}

static void close_all_windows(void)
{
    close_hatch(window_FD_open_close);
    close_hatch(window_FP_open_close);
    close_hatch(window_BD_open_close);
    close_hatch(window_BP_open_close);
    close_hatch(sunroof_open_close);
}

static void open_hatch(lv_demo_truck_hatch_t * hatch)
{
    LV_ASSERT_NULL(hatch);

    if(hatch->hatch_state == LV_HATCH_UNSET) {
        hatch->hatch_state = LV_HATCH_OPEN;
        hatch_update_checkbox(hatch);
        return;
    }

    hatch->hatch_state = LV_HATCH_OPEN;
    int32_t start_value = hatch->last_set_value;
    int32_t goal_value = (int32_t)(hatch->open_degrees * 100.f);
    if(start_value == goal_value) return;
    lv_anim_set_values(&(hatch->anim_template), start_value, goal_value);
    if(hatch->running_anim != NULL) lv_anim_delete(hatch->running_anim, NULL);
    hatch->running_anim = lv_anim_start(&(hatch->anim_template));

}

static void close_hatch(lv_demo_truck_hatch_t * hatch)
{
    LV_ASSERT_NULL(hatch);

    if(hatch->hatch_state == LV_HATCH_UNSET) {
        hatch->hatch_state = LV_HATCH_CLOSED;
        hatch_update_checkbox(hatch);
        return;
    }

    hatch->hatch_state = LV_HATCH_CLOSED;
    int32_t start_value = hatch->last_set_value;
    int32_t goal_value = (int32_t)(hatch->closed_degrees * 100.f);
    if(start_value == goal_value) return;
    lv_anim_set_values(&(hatch->anim_template), start_value, goal_value);
    if(hatch->running_anim != NULL) lv_anim_delete(hatch->running_anim, NULL);
    hatch->running_anim = lv_anim_start(&(hatch->anim_template));
}

/**************************
 *   STATIC INITIALIZERS
 **************************/

static void init_anim_controllers(lv_obj_t * viewer)
{
    const uint32_t DOOR_OPEN_CLOSE_MS = 500;
    door_FD_open_close = lv_demo_truck_hatch(viewer, door_FD_path, DOOR_OPEN_CLOSE_MS, -60.f, 0.f, LV_TRUCK_DOOR);
    door_FP_open_close = lv_demo_truck_hatch(viewer, door_FP_path, DOOR_OPEN_CLOSE_MS, 60.f, 0.f, LV_TRUCK_DOOR);
    door_BD_open_close = lv_demo_truck_hatch(viewer, door_BD_path, DOOR_OPEN_CLOSE_MS, -60.f, 0.f, LV_TRUCK_DOOR);
    door_BP_open_close = lv_demo_truck_hatch(viewer, door_BP_path, DOOR_OPEN_CLOSE_MS, 60.f, 0.f, LV_TRUCK_DOOR);
    hood_open_close = lv_demo_truck_hatch(viewer, hood_path, DOOR_OPEN_CLOSE_MS, -40.f, 0.f, LV_TRUCK_TRUNKHOOD);
    tailgate_open_close = lv_demo_truck_hatch(viewer, tailgate_path, DOOR_OPEN_CLOSE_MS, -85.f, 0.f, LV_TRUCK_TRUNKHOOD);

    const uint32_t WINDOW_OPEN_CLOSE_MS = 1500;
    window_FD_open_close = lv_demo_truck_hatch(viewer, window_FD_path, WINDOW_OPEN_CLOSE_MS, -9.5f, 0.f, LV_TRUCK_WINDOW);
    window_FP_open_close = lv_demo_truck_hatch(viewer, window_FP_path, WINDOW_OPEN_CLOSE_MS, 9.5f, 0.f, LV_TRUCK_WINDOW);
    window_BD_open_close = lv_demo_truck_hatch(viewer, window_BD_path, WINDOW_OPEN_CLOSE_MS, -9.5f, 0.f, LV_TRUCK_WINDOW);
    window_BP_open_close = lv_demo_truck_hatch(viewer, window_BP_path, WINDOW_OPEN_CLOSE_MS, 9.5f, 0.f, LV_TRUCK_WINDOW);
    sunroof_open_close = lv_demo_truck_hatch(viewer, sunroof_path, WINDOW_OPEN_CLOSE_MS, -0.202454f, 0.202454f,
                                             LV_TRUCK_SUNROOF);

}

static void init_paintset_controllers(lv_obj_t * viewer)
{
    paintset_red = lv_demo_truck_paintset(viewer, paintset_A_body, paintset_A_hood, paintset_A_door_FD, paintset_A_door_FP,
                                          paintset_A_door_BD, paintset_A_door_BP, paintset_A_tailgate);
    paintset_gray = lv_demo_truck_paintset(viewer, paintset_B_body, paintset_B_hood, paintset_B_door_FD, paintset_B_door_FP,
                                           paintset_B_door_BD, paintset_B_door_BP, paintset_B_tailgate);
    paintset_green = lv_demo_truck_paintset(viewer, paintset_C_body, paintset_C_hood, paintset_C_door_FD,
                                            paintset_C_door_FP,
                                            paintset_C_door_BD, paintset_C_door_BP, paintset_C_tailgate);

    lv_gltf_model_t * model = lv_gltf_get_primary_model(viewer);
    node_dirty_overlay_1 = lv_gltf_model_node_get_by_path(model, dirty_overlay_1);
    node_dirty_overlay_2 = lv_gltf_model_node_get_by_path(model, dirty_overlay_2);
    node_dirty_overlay_3 = lv_gltf_model_node_get_by_path(model, dirty_overlay_3);

}

static void init_tire_controllers(lv_obj_t * viewer)
{
    MAX_SPIN_RATE = (revolution_rate(0.222f, MAX_SPEED_KMH) / 60.f) * ((float)LV_DEF_REFR_PERIOD / 1000.f);
    tireset_controller = lv_demo_truck_tireset_controller(viewer, LV_TIRE_DIRTY);
}

static void init_lights_controller(lv_obj_t * viewer)
{
    lights_controller = lv_demo_truck_lightset_controller(viewer);
}

static void init_wipers_controller(lv_obj_t * viewer)
{
    wipers_controller = lv_demo_truck_wipers_controller(viewer);
}

static void init_interior_controller(lv_obj_t * viewer)
{
    interior_controller = lv_demo_truck_interior(viewer);
}

static void init_camera_controller(lv_obj_t * viewer)
{
    camera_controller = lv_demo_truck_camera_controller(viewer);
}

static void init_subjects(lv_obj_t * viewer)
{
    lv_subject_init_float(&yaw_subject, lv_gltf_get_yaw(viewer));
    lv_subject_init_float(&pitch_subject, lv_gltf_get_pitch(viewer));
    lv_subject_init_int(&animation_speed_subject, LV_GLTF_ANIM_SPEED_NORMAL);
    lv_subject_init_int(&animation_subject, lv_gltf_model_get_animation(lv_gltf_get_primary_model(viewer)));
    lv_subject_add_observer(&animation_subject, animation_observer_cb, viewer);
    lv_subject_add_observer(&animation_speed_subject, animation_speed_observer_cb, viewer);
    lv_subject_add_observer_obj(&pitch_subject, viewer_observer_float_cb, viewer, pitch_fn.ptr);
    lv_subject_add_observer_obj(&yaw_subject, viewer_observer_float_cb, viewer, yaw_fn.ptr);
}

static void init_checkbox_states(void)
{
    hatch_update_checkbox(door_FD_open_close);
    hatch_update_checkbox(door_FP_open_close);
    hatch_update_checkbox(door_BD_open_close);
    hatch_update_checkbox(door_BP_open_close);
    hatch_update_checkbox(hood_open_close);
    hatch_update_checkbox(tailgate_open_close);

    hatch_update_checkbox(window_FD_open_close);
    hatch_update_checkbox(window_FP_open_close);
    hatch_update_checkbox(window_BD_open_close);
    hatch_update_checkbox(window_BP_open_close);
    hatch_update_checkbox(sunroof_open_close);
}
/*******************************
 *   STATIC UI PANEL CREATORS
 *******************************/

static void create_control_panel(lv_obj_t * viewer)
{
    lv_obj_t * control_panel = lv_obj_create(viewer);
    lv_obj_set_size(control_panel, LV_PCT(20), LV_PCT(100));
    lv_obj_align(control_panel, LV_ALIGN_RIGHT_MID, 0, 0);
    style_control_panel(control_panel);

    create_about_panel(control_panel, viewer);
    create_doors_panel(control_panel, viewer);
    create_windows_panel(control_panel, viewer);
    create_paint_panel(control_panel, viewer);
    create_speed_panel(control_panel, viewer);
    create_steering_panel(control_panel, viewer);
    create_headlights_panel(control_panel, viewer);
    create_wipers_panel(control_panel, viewer);

    create_camera_panel(control_panel, viewer);
    create_options_panel(control_panel, viewer);

    //create_animation_panel(control_panel, viewer);
    //create_background_panel(control_panel);
    //create_antialiasing_panel(control_panel);

}

static void create_about_panel(lv_obj_t * parent, lv_obj_t * viewer)
{
    LV_UNUSED(viewer);

    // lv_obj_t * panel = add_foldout_header(parent, "About");

    logo_viewer = lv_gltf_create(parent);
    lv_obj_set_size(logo_viewer, 128, 64);
    lv_obj_set_style_pad_top(parent, 25, 0);
    lv_obj_set_style_pad_bottom(parent, 25, 0);
    lv_obj_set_style_pad_all(logo_viewer, 0, 0);
    lv_obj_set_style_margin_all(parent, 0, 0);
    lv_obj_set_style_margin_all(logo_viewer, 0, 0);
    lv_obj_set_style_margin_top(logo_viewer, -12, 0);
    lv_obj_set_style_margin_left(logo_viewer, 10, 0);
    lv_obj_set_scrollable(logo_viewer, false);
    lv_gltf_set_background_mode(logo_viewer, LV_GLTF_BG_MODE_SOLID);

    char logo_model_path[256];
    lv_snprintf(logo_model_path, sizeof(logo_model_path), "%s/%s", ui_assets_path, "lvgl_logo_with_text.glb");
    lv_gltf_model_t * model = lv_gltf_load_model_from_file(logo_viewer, logo_model_path);
    LV_ASSERT_NULL(model);
    lv_gltf_model_play_animation(model, 0);
    lv_gltf_set_camera(logo_viewer, 1);
    lv_gltf_set_antialiasing_mode(logo_viewer, LV_GLTF_AA_MODE_ON);

    lv_obj_t * title_label = lv_label_create(parent);
    lv_label_set_text_static(title_label, "   9.5.0   ");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
    lv_obj_set_style_margin_bottom(title_label, 2, 0);
    lv_obj_set_style_margin_top(title_label, -32, 0);
    lv_obj_set_style_margin_left(title_label, 81, 0);
    lv_obj_set_style_bg_color(title_label, lv_color_hex3(0x000000), 0);
    lv_obj_set_style_bg_opa(title_label, LV_OPA_50, 0);
    lv_obj_set_style_radius(title_label, 3, 0);
    add_sep(parent);
}

static void create_doors_panel(lv_obj_t * parent, lv_obj_t * viewer)
{
    LV_UNUSED(viewer);
    lv_obj_t * panel = add_foldout_header(parent, "Doors");
    lv_obj_set_style_pad_top(panel, 0,
                             LV_PART_MAIN);

    /* --- Open All / Close All row --- */
    lv_obj_t * doors_button_row = add_row(panel);
    lv_obj_set_flex_flow(doors_button_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(doors_button_row, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(doors_button_row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(doors_button_row, 0, LV_PART_MAIN);

    open_all_doors_btn = add_labeled_event_button_to_row(doors_button_row, lv_color_hex(0xFF6B35), "Open All",
                                                         open_all_doors_event_handler);
    style_toggle_button(open_all_doors_btn, lv_color_hex(0xFF6B35));

    close_all_doors_btn = add_labeled_event_button_to_row(doors_button_row, lv_color_hex(0xFF6B35), "Close All",
                                                          close_all_doors_event_handler);
    style_toggle_button(close_all_doors_btn, lv_color_hex(0xFF6B35));
    lv_obj_set_hidden(close_all_doors_btn, true);

    /* --- 2x2 door grid --- */
    lv_obj_t * door_grid = add_row(panel);
    lv_obj_set_flex_flow(door_grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(door_grid, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(door_grid, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(door_grid, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_row(door_grid, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_column(door_grid, 8, LV_PART_MAIN);

    /* Row 1: Front Driver | Front Passenger */
    door_FD_open_close->checkbox = add_labeled_event_button_to_row(door_grid, lv_color_hex(0xFF6B35), "Driver",
                                                                   toggle_door_FD_event_handler);
    lv_obj_set_size(door_FD_open_close->checkbox, LV_PCT(47), 34);
    lv_obj_set_checkable(door_FD_open_close->checkbox, true);
    style_toggle_button(door_FD_open_close->checkbox, lv_color_hex(0xFF6B35));

    door_FP_open_close->checkbox = add_labeled_event_button_to_row(door_grid, lv_color_hex(0xFF6B35), "Passenger",
                                                                   toggle_door_FP_event_handler);
    lv_obj_set_size(door_FP_open_close->checkbox, LV_PCT(47), 34);
    lv_obj_set_checkable(door_FP_open_close->checkbox, true);
    style_toggle_button(door_FP_open_close->checkbox, lv_color_hex(0xFF6B35));

    /* Row 2: Back Driver | Back Passenger */
    door_BD_open_close->checkbox = add_labeled_event_button_to_row(door_grid, lv_color_hex(0xFF6B35), "Rear Left",
                                                                   toggle_door_BD_event_handler);
    lv_obj_set_size(door_BD_open_close->checkbox, LV_PCT(47), 34);
    lv_obj_set_checkable(door_BD_open_close->checkbox, true);
    style_toggle_button(door_BD_open_close->checkbox, lv_color_hex(0xFF6B35));

    door_BP_open_close->checkbox = add_labeled_event_button_to_row(door_grid, lv_color_hex(0xFF6B35), "Rear Right",
                                                                   toggle_door_BP_event_handler);
    lv_obj_set_size(door_BP_open_close->checkbox, LV_PCT(47), 34);
    lv_obj_set_checkable(door_BP_open_close->checkbox, true);
    style_toggle_button(door_BP_open_close->checkbox, lv_color_hex(0xFF6B35));

    /* --- Hood + Tailgate row --- */
    lv_obj_t * extra_row = add_row(panel);
    lv_obj_set_flex_flow(extra_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(extra_row, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(extra_row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(extra_row, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_column(extra_row, 8, LV_PART_MAIN);

    hood_open_close->checkbox = add_labeled_event_button_to_row(extra_row, lv_color_hex(0xFF6B35), "Hood",
                                                                toggle_hood_event_handler);
    lv_obj_set_size(hood_open_close->checkbox, LV_PCT(47), 34);
    lv_obj_set_checkable(hood_open_close->checkbox, true);
    style_toggle_button(hood_open_close->checkbox, lv_color_hex(0xFF6B35));

    tailgate_open_close->checkbox = add_labeled_event_button_to_row(extra_row, lv_color_hex(0xFF6B35), "Tailgate",
                                                                    toggle_tailgate_event_handler);
    lv_obj_set_size(tailgate_open_close->checkbox, LV_PCT(47), 34);
    lv_obj_set_checkable(tailgate_open_close->checkbox, true);
    style_toggle_button(tailgate_open_close->checkbox, lv_color_hex(0xFF6B35));

    add_sep(panel);
}
static void create_windows_panel(lv_obj_t * parent, lv_obj_t * viewer)
{
    LV_UNUSED(viewer);
    lv_obj_t * panel = add_foldout_header(parent, "Windows");

    /* --- Open All / Close All row --- */
    lv_obj_t * windows_row = add_row(panel);
    lv_obj_set_flex_flow(windows_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(windows_row, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(windows_row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(windows_row, 0, LV_PART_MAIN);

    open_all_windows_btn = add_labeled_event_button_to_row(windows_row, lv_color_hex(0xFF6B35), "Open All",
                                                           open_all_windows_event_handler);
    style_toggle_button(open_all_windows_btn, lv_color_hex(0xFF6B35));

    close_all_windows_btn = add_labeled_event_button_to_row(windows_row, lv_color_hex(0xFF6B35), "Close All",
                                                            close_all_windows_event_handler);
    style_toggle_button(close_all_windows_btn, lv_color_hex(0xFF6B35));
    lv_obj_set_hidden(close_all_windows_btn, true);

    /* --- 2x2 window grid --- */
    lv_obj_t * window_grid = add_row(panel);
    lv_obj_set_flex_flow(window_grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_all(window_grid, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(window_grid, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(window_grid, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_row(window_grid, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_column(window_grid, 8, LV_PART_MAIN);

    window_FD_open_close->checkbox = add_labeled_event_button_to_row(window_grid, lv_color_hex(0xFF6B35), "Driver",
                                                                     toggle_window_FD_event_handler);
    lv_obj_set_size(window_FD_open_close->checkbox, LV_PCT(47), 34);
    lv_obj_set_checkable(window_FD_open_close->checkbox, true);
    style_toggle_button(window_FD_open_close->checkbox, lv_color_hex(0xFF6B35));

    window_FP_open_close->checkbox = add_labeled_event_button_to_row(window_grid, lv_color_hex(0xFF6B35), "Passenger",
                                                                     toggle_window_FP_event_handler);
    lv_obj_set_size(window_FP_open_close->checkbox, LV_PCT(47), 34);
    lv_obj_set_checkable(window_FP_open_close->checkbox, true);
    style_toggle_button(window_FP_open_close->checkbox, lv_color_hex(0xFF6B35));

    window_BD_open_close->checkbox = add_labeled_event_button_to_row(window_grid, lv_color_hex(0xFF6B35), "Rear Left",
                                                                     toggle_window_BD_event_handler);
    lv_obj_set_size(window_BD_open_close->checkbox, LV_PCT(47), 34);
    lv_obj_set_checkable(window_BD_open_close->checkbox, true);
    style_toggle_button(window_BD_open_close->checkbox, lv_color_hex(0xFF6B35));

    window_BP_open_close->checkbox = add_labeled_event_button_to_row(window_grid, lv_color_hex(0xFF6B35), "Rear Right",
                                                                     toggle_window_BP_event_handler);
    lv_obj_set_size(window_BP_open_close->checkbox, LV_PCT(47), 34);
    lv_obj_set_checkable(window_BP_open_close->checkbox, true);
    style_toggle_button(window_BP_open_close->checkbox, lv_color_hex(0xFF6B35));

    /* --- Sunroof row --- */
    lv_obj_t * sunroof_row = add_row(panel);
    lv_obj_set_flex_flow(sunroof_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(sunroof_row, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(sunroof_row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(sunroof_row, 8, LV_PART_MAIN);

    sunroof_open_close->checkbox = add_labeled_event_button_to_row(sunroof_row, lv_color_hex(0xFF6B35), "Sunroof",
                                                                   toggle_sunroof_event_handler);
    lv_obj_set_size(sunroof_open_close->checkbox, LV_PCT(47), 34);
    lv_obj_set_checkable(sunroof_open_close->checkbox, true);
    style_toggle_button(sunroof_open_close->checkbox, lv_color_hex(0xFF6B35));

    add_sep(panel);
}

static void create_paint_panel(lv_obj_t * parent, lv_obj_t * viewer)
{
    LV_UNUSED(viewer);
    lv_obj_t * panel = add_foldout_header(parent, "Paint");

    /* --- Paint color selection row --- */
    lv_obj_t * paint_row = add_row(panel);
    lv_obj_set_flex_flow(paint_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(paint_row, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(paint_row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(paint_row, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_column(paint_row, 8, LV_PART_MAIN);

    paintset_red->button = add_labeled_event_button_to_row(paint_row, lv_color_hex(0xCC2200), "Red",
                                                           select_paintset_A_event_handler);
    lv_obj_set_size(paintset_red->button, LV_PCT(30), 34);
    lv_obj_set_checkable(paintset_red->button, true);
    style_toggle_button(paintset_red->button, lv_color_hex(0xCC2200));

    paintset_gray->button = add_labeled_event_button_to_row(paint_row, lv_color_hex(0x888888), "Gray",
                                                            select_paintset_B_event_handler);
    lv_obj_set_size(paintset_gray->button, LV_PCT(30), 34);
    lv_obj_set_checkable(paintset_gray->button, true);
    style_toggle_button(paintset_gray->button, lv_color_hex(0x888888));

    paintset_green->button = add_labeled_event_button_to_row(paint_row, lv_color_hex(0x2E7D32), "Green",
                                                             select_paintset_C_event_handler);
    lv_obj_set_size(paintset_green->button, LV_PCT(30), 34);
    lv_obj_set_checkable(paintset_green->button, true);
    style_toggle_button(paintset_green->button, lv_color_hex(0x2E7D32));

    /* --- Add Mud / Remove Mud row --- */
    lv_obj_t * mud_row = add_row(panel);
    lv_obj_set_flex_flow(mud_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(mud_row, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(mud_row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(mud_row, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_column(mud_row, 8, LV_PART_MAIN);

    mud_toggle_btn = add_labeled_event_button_to_row(mud_row, lv_color_hex(0xFF6B35), "Add Mud",
                                                     toggle_mud_event_handler);
    style_toggle_button(mud_toggle_btn, lv_color_hex(0xFF6B35));
    lv_obj_add_state(mud_toggle_btn, LV_STATE_CHECKED);

    add_sep(panel);
}

static void create_speed_panel(lv_obj_t * parent, lv_obj_t * viewer)
{
    LV_UNUSED(viewer);
    lv_obj_t * panel = add_foldout_header(parent, "Speed");

    /* --- Gas / Brake row --- */
    lv_obj_t * speed_row4 = add_row(panel);
    lv_obj_set_flex_flow(speed_row4, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(speed_row4, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(speed_row4, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(speed_row4, 0, LV_PART_MAIN);
    lv_obj_set_style_margin_bottom(speed_row4, 0, LV_PART_MAIN);

    lv_obj_t * gas_btn = add_button_to_row(speed_row4, lv_color_hex(0x22A646));
    lv_obj_set_size(gas_btn, LV_PCT(35), 30);
    lv_obj_add_event_cb(gas_btn, gas_pressing_event_handler, LV_EVENT_PRESSING, NULL);
    lv_obj_t * gas_btn_label = lv_label_create(gas_btn);
    lv_label_set_text_static(gas_btn_label, "Gas");
    lv_obj_set_style_text_color(gas_btn_label, lv_color_white(), 0);
    lv_obj_center(gas_btn_label);

    lv_obj_t * brakes_btn = add_button_to_row(speed_row4, lv_color_hex(0xD92D2D));
    lv_obj_set_size(brakes_btn, LV_PCT(45), 30);
    lv_obj_add_event_cb(brakes_btn, brakes_press_event_handler, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(brakes_btn, brakes_pressing_event_handler, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb(brakes_btn, brakes_released_event_handler, LV_EVENT_RELEASED, NULL);
    lv_obj_t * brakes_btn_label = lv_label_create(brakes_btn);
    lv_label_set_text_static(brakes_btn_label, "Brake");
    lv_obj_set_style_text_color(brakes_btn_label, lv_color_white(), 0);
    lv_obj_center(brakes_btn_label);

    /* --- Speed readout row --- */
    lv_obj_t * speed_row3 = add_row(panel);
    lv_obj_set_flex_flow(speed_row3, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(speed_row3, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(speed_row3, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(speed_row3, 0, LV_PART_MAIN);
    lv_obj_set_style_margin_top(speed_row3, 0, LV_PART_MAIN);
    lv_obj_set_style_margin_bottom(speed_row3, 0, LV_PART_MAIN);
    lv_obj_t * speed_label = lv_label_create(speed_row3);
    lv_label_set_text(speed_label, "0");
    lv_obj_set_style_text_color(speed_label, lv_color_white(), 0);
#if LV_FONT_MONTSERRAT_26
    lv_obj_set_style_text_font(speed_label, &lv_font_montserrat_26, 0);
#endif
    lv_obj_center(speed_label);
    tireset_controller->label_speed = speed_label;

    lv_obj_t * speed_slider_label = lv_label_create(speed_row3);
    lv_label_set_text_static(speed_slider_label, "km/h");
    lv_obj_set_style_text_color(speed_slider_label, lv_color_white(), 0);

    lv_obj_center(speed_slider_label);
    tireset_controller->label_speed_type = speed_slider_label;

    /* --- Speed slider row --- */
    lv_obj_t * speed_row2 = add_row(panel);
    lv_obj_set_flex_flow(speed_row2, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(speed_row2, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(speed_row2, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(speed_row2, 4, LV_PART_MAIN);
    lv_obj_set_style_margin_bottom(speed_row2, 4, LV_PART_MAIN);

    lv_obj_t * speed_slider = add_slider_to_row(speed_row2, lv_color_hex(0xFF6B35));
    lv_obj_add_event_cb(speed_slider, select_speed_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_slider_set_min_value(speed_slider, 0);
    lv_obj_set_style_pad_all(speed_slider, 5, LV_PART_MAIN);
    lv_slider_set_max_value(speed_slider, MAX_SPIN_SLIDER_VALUE);
    tireset_controller->slider_speed = speed_slider;

    /* --- Unit toggle row --- */
    lv_obj_t * unit_row = add_row(panel);
    lv_obj_set_flex_flow(unit_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(unit_row, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(unit_row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(unit_row, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_column(unit_row, 8, LV_PART_MAIN);

    lv_obj_t * mph_btn = add_labeled_event_button_to_row(unit_row, lv_color_hex(0xFF6B35), "mph",
                                                         select_mph_event_handler);
    lv_obj_set_size(mph_btn, LV_PCT(47), 34);
    lv_obj_set_checkable(mph_btn, true);
    style_toggle_button(mph_btn, lv_color_hex(0xFF6B35));
    tireset_controller->checkbox_mph = mph_btn;

    lv_obj_t * kmph_btn = add_labeled_event_button_to_row(unit_row, lv_color_hex(0xFF6B35), "km/h",
                                                          select_kmph_event_handler);
    lv_obj_set_size(kmph_btn, LV_PCT(47), 34);
    lv_obj_set_checkable(kmph_btn, true);
    style_toggle_button(kmph_btn, lv_color_hex(0xFF6B35));
    tireset_controller->checkbox_kmph = kmph_btn;

    add_sep(panel);
}

static void create_steering_panel(lv_obj_t * parent, lv_obj_t * viewer)
{
    LV_UNUSED(viewer);
    lv_obj_t * panel = add_foldout_header(parent, "Steering");

    /* --- Steering slider row --- */
    lv_obj_t * steering_row2 = add_row(panel);
    lv_obj_set_flex_flow(steering_row2, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(steering_row2, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(steering_row2, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(steering_row2, 4, LV_PART_MAIN);
    lv_obj_set_style_margin_bottom(steering_row2, 4, LV_PART_MAIN);

    lv_obj_t * steering_slider = add_slider_to_row(steering_row2, lv_color_hex(0xFF6B35));
    lv_slider_set_min_value(steering_slider, -MIN_MAX_STEER_SLIDER_VALUE);
    lv_slider_set_max_value(steering_slider, MIN_MAX_STEER_SLIDER_VALUE);
    lv_slider_set_value(steering_slider, 0, LV_ANIM_OFF);
    lv_slider_set_mode(steering_slider, LV_SLIDER_MODE_SYMMETRICAL);
    lv_obj_set_style_pad_all(steering_slider, 5, LV_PART_MAIN);
    lv_obj_add_event_cb(steering_slider, select_steering_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    tireset_controller->slider_steering = steering_slider;

    /* --- Blinker buttons row --- */
    lv_obj_t * steering_row = add_row(panel);
    lv_obj_set_flex_flow(steering_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(steering_row, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(steering_row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(steering_row, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_column(steering_row, 8, LV_PART_MAIN);
    lv_obj_set_style_margin_bottom(steering_row, 0, LV_PART_MAIN);

    left_blinker_btn = add_button_to_row(steering_row, lv_color_hex(0xFF6B35));
    lv_obj_set_size(left_blinker_btn, LV_PCT(30), 34);
    lv_obj_set_checkable(left_blinker_btn, true);
    lv_obj_add_event_cb(left_blinker_btn, select_left_blinker_event_handler, LV_EVENT_CLICKED, NULL);
    style_toggle_button(left_blinker_btn, lv_color_hex(0xFF6B35));
    lv_obj_t * left_blinker_label = lv_label_create(left_blinker_btn);
    lv_label_set_text_static(left_blinker_label, LV_SYMBOL_LEFT);
    lv_obj_set_style_text_color(left_blinker_label, lv_color_white(), 0);
    lv_obj_center(left_blinker_label);

    hazard_blinker_btn = add_button_to_row(steering_row, lv_color_hex(0xFFB300));
    lv_obj_set_size(hazard_blinker_btn, LV_PCT(30), 34);
    lv_obj_set_checkable(hazard_blinker_btn, true);
    lv_obj_add_event_cb(hazard_blinker_btn, select_hazard_blinker_event_handler, LV_EVENT_CLICKED, NULL);
    style_toggle_button(hazard_blinker_btn, lv_color_hex(0xFFB300));
    lv_obj_t * hazard_blinker_label = lv_label_create(hazard_blinker_btn);
    lv_label_set_text_static(hazard_blinker_label, LV_SYMBOL_WARNING);
    lv_obj_set_style_text_color(hazard_blinker_label, lv_color_white(), 0);
    lv_obj_center(hazard_blinker_label);

    right_blinker_btn = add_button_to_row(steering_row, lv_color_hex(0xFF6B35));
    lv_obj_set_size(right_blinker_btn, LV_PCT(30), 34);
    lv_obj_set_checkable(right_blinker_btn, true);
    lv_obj_add_event_cb(right_blinker_btn, select_right_blinker_event_handler, LV_EVENT_CLICKED, NULL);
    style_toggle_button(right_blinker_btn, lv_color_hex(0xFF6B35));
    lv_obj_t * right_blinker_label = lv_label_create(right_blinker_btn);
    lv_label_set_text_static(right_blinker_label, LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_color(right_blinker_label, lv_color_white(), 0);
    lv_obj_center(right_blinker_label);

    add_sep(panel);
}

static void create_headlights_panel(lv_obj_t * parent, lv_obj_t * viewer)
{
    LV_UNUSED(viewer);

    lv_obj_t * panel = add_foldout_header(parent, "Headlights");
    lights_controller->checkbox_headlights_low  = add_labeled_checkbox_row(panel, "Low",
                                                                           select_headlights_low_event_handler);
    lights_controller->checkbox_headlights_high  = add_labeled_checkbox_row(panel, "High",
                                                                            select_headlights_high_event_handler);
    add_sep(panel);
}

static void create_wipers_panel(lv_obj_t * parent, lv_obj_t * viewer)
{

    LV_UNUSED(viewer);
    lv_obj_t * panel = add_foldout_header(parent, "Wipers");

    lv_obj_t * wipers_row = add_row(panel);
    lv_obj_set_flex_flow(wipers_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(wipers_row, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(wipers_row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(wipers_row, 0, LV_PART_MAIN);
    lv_obj_set_style_margin_bottom(wipers_row, 0, LV_PART_MAIN);

    wipers_controller->btn_wipers_low = add_labeled_event_button_to_row(wipers_row, lv_color_hex(0xFF6B35), "Slow",
                                                                        select_wipers_low_event_handler);
    lv_obj_set_size(wipers_controller->btn_wipers_low, LV_PCT(25), 30);
    lv_obj_set_checkable(wipers_controller->btn_wipers_low, true);
    style_toggle_button(wipers_controller->btn_wipers_low, lv_color_hex(0xFF6B35));

    wipers_controller->btn_wipers_med = add_labeled_event_button_to_row(wipers_row, lv_color_hex(0xFF6B35), "Medium",
                                                                        select_wipers_med_event_handler);
    style_toggle_button(wipers_controller->btn_wipers_med, lv_color_hex(0xFF6B35));

    lv_obj_set_size(wipers_controller->btn_wipers_med, LV_PCT(25), 30);
    wipers_controller->btn_wipers_high = add_labeled_event_button_to_row(wipers_row, lv_color_hex(0xFF6B35), "Fast",
                                                                         select_wipers_high_event_handler);
    lv_obj_set_size(wipers_controller->btn_wipers_high, LV_PCT(25), 30);
    style_toggle_button(wipers_controller->btn_wipers_high, lv_color_hex(0xFF6B35));

    add_sep(panel);
}
static void create_camera_panel(lv_obj_t * parent, lv_obj_t * viewer)
{
    lv_obj_t * panel = add_foldout_header(parent, "Cameras");

    lv_obj_t * camera_row = add_row(panel);
    lv_obj_set_flex_flow(camera_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(camera_row, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(camera_row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(camera_row, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_column(camera_row, 8, LV_PART_MAIN);

    camera_controller->checkbox_camera_interior = add_labeled_event_button_to_row(camera_row,
                                                                                  lv_color_hex(0xFF6B35),
                                                                                  "Interior",
                                                                                  select_camera_interior_event_handler);
    lv_obj_set_size(camera_controller->checkbox_camera_interior, LV_PCT(30), 34);
    lv_obj_set_checkable(camera_controller->checkbox_camera_interior, true);
    style_toggle_button(camera_controller->checkbox_camera_interior, lv_color_hex(0xFF6B35));

    camera_controller->checkbox_camera_exterior = add_labeled_event_button_to_row(camera_row,
                                                                                  lv_color_hex(0xFF6B35),
                                                                                  "Exterior",
                                                                                  select_camera_exterior_event_handler);
    lv_obj_set_size(camera_controller->checkbox_camera_exterior, LV_PCT(30), 34);
    lv_obj_set_checkable(camera_controller->checkbox_camera_exterior, true);
    style_toggle_button(camera_controller->checkbox_camera_exterior, lv_color_hex(0xFF6B35));

    camera_controller->checkbox_camera_free = add_labeled_event_button_to_row(camera_row,
                                                                              lv_color_hex(0xFF6B35),
                                                                              "Free",
                                                                              select_camera_free_event_handler);
    lv_obj_set_size(camera_controller->checkbox_camera_free, LV_PCT(30), 34);
    lv_obj_set_checkable(camera_controller->checkbox_camera_free, true);
    style_toggle_button(camera_controller->checkbox_camera_free, lv_color_hex(0xFF6B35));

    camera_controller->viewer = viewer;
    add_sep(panel);
}

static void select_resolution_event_handler(lv_event_t * e)
{
    lv_obj_t * dropdown = lv_event_get_target(e);
    uint16_t idx = lv_dropdown_get_selected(dropdown);
    static const struct {
        int w, h;
    } resolutions[] = {
        {800,  480}, {800,  600},
        {1024, 600}, {1024, 768}, {1024, 800},
        {1280, 720}, {1280, 800},
        {1366, 768},
        {1920, 1080}
    };

    lv_display_set_resolution(NULL, resolutions[idx].w, resolutions[idx].h);
}


static void create_options_panel(lv_obj_t * parent, lv_obj_t * viewer)
{
    LV_UNUSED(viewer);
    lv_obj_t * panel = add_foldout_header(parent, "Options");

    /* --- Resolution label --- */
    lv_obj_t * res_label_row = add_row(panel);
    lv_obj_set_flex_flow(res_label_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(res_label_row, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(res_label_row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(res_label_row, 4, LV_PART_MAIN);

    lv_obj_t * res_label = lv_label_create(res_label_row);
    lv_label_set_text_static(res_label, "Resolution");
    lv_obj_set_style_text_color(res_label, lv_color_hex(0x888888), 0);

    /* --- Resolution dropdown --- */
    lv_obj_t * res_row = add_row(panel);
    lv_obj_set_flex_flow(res_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(res_row, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(res_row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(res_row, 8, LV_PART_MAIN);

    lv_obj_t * res_dropdown = lv_dropdown_create(res_row);
    lv_dropdown_set_options_static(res_dropdown,
                                   "800 x 480\n"
                                   "800 x 600\n"
                                   "1024 x 600\n"
                                   "1024 x 768\n"
                                   "1024 x 800\n"
                                   "1280 x 720\n"
                                   "1280 x 800\n"
                                   "1366 x 768\n"
                                   "1920 x 1080");
    lv_obj_set_width(res_dropdown, LV_PCT(90));
    lv_obj_add_event_cb(res_dropdown, select_resolution_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    /* Style the dropdown to match the dark theme */
    lv_obj_set_style_bg_color(res_dropdown, lv_color_hex(0x1A1A1A), LV_PART_MAIN);
    lv_obj_set_style_border_color(res_dropdown, lv_color_hex(0x444444), LV_PART_MAIN);
    lv_obj_set_style_border_width(res_dropdown, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(res_dropdown, 8, LV_PART_MAIN);
    lv_obj_set_style_text_color(res_dropdown, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(res_dropdown, 0, LV_PART_MAIN);

    /* Style the dropdown list */
    lv_obj_t * res_list = lv_dropdown_get_list(res_dropdown);
    lv_obj_set_style_bg_color(res_list, lv_color_hex(0x1A1A1A), LV_PART_MAIN);
    lv_obj_set_style_border_color(res_list, lv_color_hex(0x444444), LV_PART_MAIN);
    lv_obj_set_style_border_width(res_list, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(res_list, 8, LV_PART_MAIN);
    lv_obj_set_style_text_color(res_list, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_color(res_list, lv_color_hex(0xFF6B35), LV_PART_SELECTED | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(res_list, lv_color_white(), LV_PART_SELECTED | LV_STATE_CHECKED);

    /* --- Anti-alias toggle --- */
    lv_obj_t * antialias_row = add_row(panel);
    lv_obj_set_flex_flow(antialias_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(antialias_row, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(antialias_row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(antialias_row, 8, LV_PART_MAIN);

    checkbox_antialiasing = add_labeled_event_button_to_row(antialias_row, lv_color_hex(0xFF6B35), "Anti-Alias",
                                                            toggle_antialias_event_handler);
    lv_obj_set_size(checkbox_antialiasing, LV_PCT(60), 34);
    lv_obj_set_checkable(checkbox_antialiasing, true);
    style_toggle_button(checkbox_antialiasing, lv_color_hex(0xFF6B35));
    add_sep(panel);
}


/*****************************
 *   STATIC UI CREATE UTILS
 *****************************/

static lv_obj_t * add_title_to_row(lv_obj_t * row, const char * title)
{
    lv_obj_t * title_label = lv_label_create(row);
    lv_label_set_text_static(title_label, title);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
    lv_obj_set_style_margin_bottom(title_label, 2, 0);
    return title_label;
}

static lv_obj_t * add_foldout_header(lv_obj_t * parent, const char * title)
{

    lv_demo_foldout_t * foldout = lv_demo_foldout(parent, title);
    if(last_opened_foldout == NULL) {
        /* Show the first foldout (About) */
        show_foldout(foldout);
    }
    else {
        /* Hide the other foldouts */

        /* Retain a reference to the first foldout */
        lv_demo_foldout_t * first_foldout = last_opened_foldout;

        /* Hide the new foldout */
        hide_foldout(foldout);

        /* Restore the first foldout reference */
        last_opened_foldout = first_foldout;
    }
    return foldout->contents;
}

static lv_obj_t * add_row(lv_obj_t * parent)
{
    lv_obj_t * row = lv_obj_create(parent);
    lv_obj_set_size(row, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_left(row, 20, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(row, 10, LV_PART_MAIN);
    lv_obj_set_style_pad_right(row, 20, LV_PART_MAIN);
    lv_obj_set_layout(row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_COLUMN);
    return row;
}

static lv_obj_t * add_sep(lv_obj_t * parent)
{
    lv_obj_t * sep = lv_obj_create(parent);
    lv_obj_set_size(sep, LV_PCT(100), 3);
    lv_obj_set_style_bg_opa(sep, LV_OPA_30, LV_PART_MAIN);
    lv_obj_set_style_bg_color(sep, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_pad_top(sep, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(sep, 0, LV_PART_MAIN);
    lv_obj_set_style_margin_left(sep, 10, LV_PART_MAIN);
    lv_obj_set_style_margin_right(sep, 10, LV_PART_MAIN);
    lv_obj_set_style_margin_bottom(sep, 0, LV_PART_MAIN);

    lv_obj_set_style_radius(sep, 4, 0);
    lv_obj_set_style_border_width(sep, 0, 0);
    return sep;
}



static lv_obj_t * add_button_to_row(lv_obj_t * row, lv_color_t color)
{
    lv_obj_t * btn = lv_button_create(row);
    lv_obj_set_size(btn, LV_PCT(100), 30);
    lv_obj_set_style_bg_color(btn, color, LV_PART_MAIN);
    lv_obj_set_style_radius(btn, 4, 0);

    return btn;
}

static lv_obj_t * add_labeled_event_button_to_row(lv_obj_t * row, lv_color_t color, const char * label,
                                                  lv_event_cb_t event_cb)
{
    lv_obj_t * button = add_button_to_row(row, color);
    lv_obj_add_event_cb(button, event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * button_label = lv_label_create(button);
    lv_label_set_text_static(button_label, label);
    lv_obj_set_style_text_color(button_label, lv_color_white(), 0);
    lv_obj_center(button_label);
    return button;
}

static lv_obj_t * add_slider_to_row(lv_obj_t * row, lv_color_t color)
{
    lv_obj_t * sldr = lv_slider_create(row);
    lv_obj_set_size(sldr, LV_PCT(100), 20);
    lv_obj_set_style_bg_color(sldr, color, LV_PART_MAIN);
    lv_obj_set_style_radius(sldr, 4, 0);
    lv_obj_set_style_margin_all(sldr, 6, LV_PART_MAIN);
    style_slider(sldr, SLIDER_COLOR);

    return sldr;
}

static lv_obj_t * add_checkbox_to_row(lv_obj_t * row, lv_color_t color, lv_event_cb_t event_cb)
{
    lv_obj_t * checkbox = lv_checkbox_create(row);
    lv_obj_set_size(checkbox, 20, LV_TRUCK_DEMO_CHECKBOX_HEIGHT);
    lv_obj_set_style_pad_all(checkbox, 0, LV_PART_MAIN);
    lv_obj_set_style_margin_all(checkbox, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(checkbox, color, LV_PART_MAIN);
    lv_obj_set_style_radius(checkbox, 4, 0);
    lv_obj_add_event_cb(checkbox, event_cb, LV_EVENT_CLICKED, NULL);

    return checkbox;
}

static lv_obj_t * add_labeled_checkbox_row(lv_obj_t * panel, const char * label, lv_event_cb_t event_cb)
{
    lv_obj_t * row = add_row(panel);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row, 15, LV_PART_MAIN);
    lv_obj_set_style_pad_top(row, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(row, 0, LV_PART_MAIN);

    lv_obj_t * checkbox = add_checkbox_to_row(row, lv_color_hex(0xFF6B35), event_cb);
    lv_obj_t * checkbox_label = lv_label_create(row);
    lv_obj_set_style_pad_top(checkbox_label, 2, LV_PART_MAIN);
    lv_label_set_text_static(checkbox_label, label);
    lv_obj_set_style_text_color(checkbox_label, lv_color_white(), 0);
    style_checkbox(checkbox, lv_color_hex(0xFF6B35));
    return checkbox;
}


static void style_checkbox(lv_obj_t * checkbox, lv_color_t accent_color)
{
    /* --- Tick box (the square) --- */
    lv_obj_set_style_radius(checkbox, 6, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(checkbox, lv_color_hex(0x1A1A1A), LV_PART_INDICATOR);
    lv_obj_set_style_border_width(checkbox, 2, LV_PART_INDICATOR);
    lv_obj_set_style_border_color(checkbox, lv_color_hex(0x444444), LV_PART_INDICATOR);

    /* Checked state */
    lv_obj_set_style_bg_color(checkbox, accent_color, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_border_color(checkbox, accent_color, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(checkbox, 8, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_color(checkbox, accent_color, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_opa(checkbox, 150, LV_PART_INDICATOR | LV_STATE_CHECKED);

    /* Pressed state */
    lv_obj_set_style_bg_color(checkbox, lv_color_mix(accent_color, lv_color_black(), 180),
                              LV_PART_INDICATOR | LV_STATE_PRESSED);
    lv_obj_set_style_border_color(checkbox, accent_color,
                                  LV_PART_INDICATOR | LV_STATE_PRESSED);

    lv_obj_set_style_bg_color(checkbox, lv_color_hex(0x0F0F0F),
                              LV_PART_INDICATOR | LV_STATE_DISABLED);
    lv_obj_set_style_border_color(checkbox, lv_color_hex(0x222222),
                                  LV_PART_INDICATOR | LV_STATE_DISABLED);
    lv_obj_set_style_opa(checkbox, 102, LV_PART_INDICATOR | LV_STATE_DISABLED);

    lv_obj_set_style_shadow_width(checkbox, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(checkbox, LV_OPA_TRANSP, LV_PART_MAIN);
}

static void style_toggle_button(lv_obj_t * btn, lv_color_t accent_color)
{
    /* --- Default (unselected) state --- */
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x1A1A1A), LV_PART_MAIN);
    lv_obj_set_style_radius(btn, 8, LV_PART_MAIN);
    lv_obj_set_style_border_width(btn, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x444444), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn, 0, LV_PART_MAIN);

    /* Label color unselected */
    lv_obj_set_style_text_color(btn, lv_color_hex(0x888888), LV_PART_MAIN);

    /* --- Pressed state --- */
    lv_obj_set_style_bg_color(btn, lv_color_mix(accent_color, lv_color_black(), 180),
                              LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_border_color(btn, accent_color, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_text_color(btn, lv_color_white(), LV_PART_MAIN | LV_STATE_PRESSED);

    /* --- Checked/active state (for toggle buttons) --- */
    lv_obj_set_style_bg_color(btn, accent_color, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_border_color(btn, accent_color, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(btn, lv_color_white(), LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_width(btn, 10, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_color(btn, accent_color, LV_PART_MAIN | LV_STATE_CHECKED);
    lv_obj_set_style_shadow_opa(btn, 150, LV_PART_MAIN | LV_STATE_CHECKED);

    /* --- Disabled state --- */
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x0F0F0F), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_set_style_border_color(btn, lv_color_hex(0x222222), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_set_style_text_color(btn, lv_color_hex(0x444444), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_set_style_opa(btn, 102, LV_PART_MAIN | LV_STATE_DISABLED);
}

static void style_slider(lv_obj_t * slider, lv_color_t accent_color)
{
    lv_obj_set_style_bg_color(slider, lv_color_hex(0x1A1A1A), LV_PART_MAIN);
    lv_obj_set_style_radius(slider, 20, LV_PART_MAIN);
    lv_obj_set_style_border_width(slider, 1, LV_PART_MAIN);
    lv_obj_set_style_border_color(slider, lv_color_hex(0x444444), LV_PART_MAIN);

    lv_obj_set_style_bg_color(slider, accent_color, LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider, 20, LV_PART_INDICATOR);

    lv_obj_set_style_bg_color(slider, lv_color_white(), LV_PART_KNOB);
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_KNOB);
    lv_obj_set_style_shadow_width(slider, 6, LV_PART_KNOB);
    lv_obj_set_style_shadow_color(slider, accent_color, LV_PART_KNOB);
    lv_obj_set_style_shadow_opa(slider, 150, LV_PART_KNOB);
    lv_obj_set_style_border_width(slider, 2, LV_PART_KNOB);
    lv_obj_set_style_border_color(slider, accent_color, LV_PART_KNOB);

    lv_obj_set_style_bg_color(slider, lv_color_hex(0x0F0F0F), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_set_style_border_color(slider, lv_color_hex(0x222222), LV_PART_MAIN | LV_STATE_DISABLED);
    lv_obj_set_style_opa(slider, 128, LV_PART_MAIN | LV_STATE_DISABLED);

    lv_color_t dimmed_accent = lv_color_mix(accent_color, lv_color_black(), 128);
    lv_obj_set_style_bg_color(slider, dimmed_accent, LV_PART_INDICATOR | LV_STATE_DISABLED);
    lv_obj_set_style_opa(slider, 102, LV_PART_INDICATOR | LV_STATE_DISABLED);

    lv_obj_set_style_bg_color(slider, lv_color_hex(0xCCCCCC), LV_PART_KNOB | LV_STATE_DISABLED);
    lv_obj_set_style_shadow_width(slider, 2, LV_PART_KNOB | LV_STATE_DISABLED);
    lv_obj_set_style_shadow_opa(slider, 51, LV_PART_KNOB | LV_STATE_DISABLED);
}



static void style_control_panel(lv_obj_t * panel)
{

    lv_obj_set_style_bg_color(panel, lv_color_hex(0x2C2C2C), 0);
    lv_obj_set_style_border_width(panel, 1, 0);
    lv_obj_set_style_border_color(panel, lv_color_hex(0x555555), 0);
    lv_obj_set_style_radius(panel, 8, 0);
    lv_obj_set_style_pad_all(panel, 5, 0);
    lv_obj_set_layout(panel, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);

    lv_obj_set_flex_align(panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_bg_opa(panel, 128, LV_PART_MAIN);

}

/********************************
 *   STATIC OBSERVER CALLBACKS
 ********************************/


static void animation_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    int value = lv_subject_get_int(subject);
    lv_obj_t * viewer = lv_observer_get_user_data(observer);
    lv_gltf_model_t * model = lv_gltf_get_primary_model(viewer);

    lv_gltf_model_play_animation(model, value);
}

static void animation_speed_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    int value = lv_subject_get_int(subject);
    lv_obj_t * viewer = lv_observer_get_user_data(observer);
    lv_gltf_model_t * model = lv_gltf_get_primary_model(viewer);

    lv_gltf_model_set_animation_speed(model, value);
}

#endif /*LV_USE_GLTF*/
