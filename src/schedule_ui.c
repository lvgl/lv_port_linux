#include "schedule_ui.h"
#include "schedule_data.h"
#include <lvgl/lvgl.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define MAX_NUMBER_OF_LESSONS 16

typedef struct {
    const char* type;
    uint32_t color;
} TypeColor;

// Dictionary of lesson types and their background colors
static const TypeColor type_colors[] = {
    {"Зачет", 0xe91e63},
    {"Экзамен", 0xe91e63},
    {"Дифференцированный зачет", 0xe91e63},
    {"Консультация к промежуточной аттестации", 0x9e5fa1},
    {"-", 0x407ab2},
    {"Курсовая работа", 0x407ab2},
    {"Лекции", 0x276093},
    {"Практические занятия и семинары", 0xff8f00},
    {"Лабораторные занятия", 0x3e8470},
    {NULL, 0xCCCCCC}
};

static lv_obj_t* list_container;
static lv_obj_t* progress_bars[MAX_NUMBER_OF_LESSONS]; // Store lessons progress bars
static struct tm current_display_date;

void update_schedule_display(struct tm* display_date)
{
    if (!list_container || !display_date) return;

    // Clear existing content
    lv_obj_clean(list_container);
    for (int i = 0; i < MAX_NUMBER_OF_LESSONS; i++)
    {
        progress_bars[i] = NULL;
    }

    // Store display date
    memcpy(&current_display_date, display_date, sizeof(struct tm));

    // Get current time
    time_t now = time(NULL);
    struct tm* current_time = localtime(&now);
    int current_minutes = current_time->tm_hour * 60 + current_time->tm_min;

    // Compare dates (ignoring time)
    int is_past_date = 0, is_future_date = 0;
    if (display_date->tm_year < current_time->tm_year ||
        (display_date->tm_year == current_time->tm_year && display_date->tm_mon < current_time->tm_mon) ||
        (display_date->tm_year == current_time->tm_year && display_date->tm_mon == current_time->tm_mon &&
         display_date->tm_mday < current_time->tm_mday))
    {
        is_past_date = 1;
    }
    else if (display_date->tm_year > current_time->tm_year ||
        (display_date->tm_year == current_time->tm_year && display_date->tm_mon > current_time->tm_mon) ||
        (display_date->tm_year == current_time->tm_year && display_date->tm_mon == current_time->tm_mon &&
         display_date->tm_mday > current_time->tm_mday))
    {
        is_future_date = 1;
    }

    // Get total number of lessons
    int lesson_count = get_lesson_count(); // Update to get_lesson_count_for_date(&current_display_date)

    // Create a block for each lesson
    for (int i = 0; i < lesson_count; i++)
    {
        lesson_t lesson = get_lesson(i); // Update to get_lesson_for_date(&current_display_date, i)

        // Create block container
        lv_obj_t* block = lv_obj_create(list_container);
        lv_obj_set_size(block, 630, LV_SIZE_CONTENT);
        //lv_obj_set_style_bg_color(block, lv_color_hex(0xF0F0F0), 0);
        lv_obj_set_style_border_width(block, 1, 0);
        lv_obj_set_style_border_color(block, lv_color_hex(0x525252), 0);
        lv_obj_set_style_radius(block, 0, 0);
        lv_obj_set_layout(block, LV_LAYOUT_FLEX);
        lv_obj_set_flex_flow(block, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(block, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        // Progress bar with striped pattern
        //LV_IMAGE_DECLARE(img_skew_strip);
        //static lv_style_t style_indic;
        //lv_style_init(&style_indic);
        //lv_style_set_bg_image_src(&style_indic, &img_skew_strip);
        //lv_style_set_bg_image_tiled(&style_indic, true);
        //lv_style_set_bg_image_opa(&style_indic, LV_OPA_30);

        //static lv_style_t style_main;
        //lv_style_init(&style_main);
        //lv_style_set_bg_image_src(&style_main, &img_skew_strip);
        //lv_style_set_bg_image_tiled(&style_main, true);
        //lv_style_set_bg_image_opa(&style_main, LV_OPA_50);

        //lv_obj_t* progress_bar = lv_bar_create(block);
        //lv_obj_add_style(progress_bar, &style_indic, LV_PART_INDICATOR);
        //lv_obj_add_style(progress_bar, &style_main, LV_PART_MAIN);
        //lv_obj_set_size(progress_bar, 600, 30);
        //lv_bar_set_range(progress_bar, 0, 100);
        //lv_obj_set_style_radius(progress_bar, 0, LV_PART_MAIN);
        //lv_obj_set_style_radius(progress_bar, 0, LV_PART_INDICATOR);
        //lv_obj_set_style_bg_color(progress_bar, lv_color_hex(0x90EE90), LV_PART_MAIN); // Fallback green
        //lv_obj_set_style_bg_color(progress_bar, lv_color_hex(0x228B22), LV_PART_INDICATOR); // Fallback green
        ////lv_obj_set_style_bg_color(progress_bar, lv_color_hex(0xCCCCCC), 0);
        //progress_bars[i] = progress_bar;

        // Progress bar
        lv_obj_t* progress_bar = lv_bar_create(block);
        lv_obj_set_size(progress_bar, 600, 30);
        lv_bar_set_range(progress_bar, 0, 100);
        lv_obj_set_style_radius(progress_bar, 0, LV_PART_MAIN);
        lv_obj_set_style_radius(progress_bar, 0, LV_PART_INDICATOR);
        lv_obj_set_style_bg_color(progress_bar, lv_color_hex(0x90EE90), LV_PART_MAIN);
        lv_obj_set_style_bg_color(progress_bar, lv_color_hex(0x228B22), LV_PART_INDICATOR);
        progress_bars[i] = progress_bar;

        // Calculate progress
        int progress = 0;
        if (is_past_date)
        {
            progress = 100;
        }
        else if (is_future_date)
        {
            progress = 0;
        }
        else
        {
            int start_minutes = lesson.start_hour * 60 + lesson.start_minute;
            int end_minutes = lesson.end_hour * 60 + lesson.end_minute;
            if (current_minutes > end_minutes)
            {
                progress = 100;
            }
            else if (current_minutes >= start_minutes && current_minutes <= end_minutes)
            {
                progress = ((current_minutes - start_minutes) * 100) / (end_minutes - start_minutes);
            }
        }
        lv_bar_set_value(progress_bar, progress, LV_ANIM_ON);

        char buffer[6];

        // Start time label
        lv_obj_t* start_time_label = lv_label_create(block);
        snprintf(buffer, sizeof(buffer), "%02d:%02d", lesson.start_hour, lesson.start_minute);
        lv_label_set_text(start_time_label, buffer);
        lv_obj_set_style_text_font(start_time_label, &lv_font_my_montserrat_20, 0);
        //lv_obj_set_style_text_color(start_time_label, lv_color_hex(0x2fb255), 0);
        lv_obj_set_style_text_color(start_time_label, lv_color_hex(0x000000), 0);
        lv_obj_set_style_text_align(start_time_label, LV_TEXT_ALIGN_LEFT, 0);
        lv_obj_add_flag(start_time_label, LV_OBJ_FLAG_FLOATING);
        lv_obj_align_to(start_time_label, progress_bar, LV_ALIGN_LEFT_MID, 5, -1);

        // End time label
        lv_obj_t* end_time_label = lv_label_create(block);
        snprintf(buffer, sizeof(buffer), "%02d:%02d", lesson.end_hour, lesson.end_minute);
        lv_label_set_text(end_time_label, buffer);
        lv_obj_set_style_text_font(end_time_label, &lv_font_my_montserrat_20, 0);
        //lv_obj_set_style_text_color(end_time_label, lv_color_hex(0x2fb255), 0);
        lv_obj_set_style_text_color(end_time_label, lv_color_hex(0x000000), 0);
        lv_obj_set_style_text_align(end_time_label, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_add_flag(end_time_label, LV_OBJ_FLAG_FLOATING);
        lv_obj_align_to(end_time_label, progress_bar, LV_ALIGN_RIGHT_MID, -5, -1);

        // Type label
        lv_obj_t* type_label = lv_label_create(block);
        lv_label_set_text(type_label, lesson.type);
        //lv_obj_set_size(type_label, 600, 30);
        lv_obj_set_width(type_label, 600);
        lv_obj_set_style_text_font(type_label, &lv_font_my_montserrat_20, 0);
        lv_obj_set_style_text_color(type_label, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_text_align(type_label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_bg_opa(type_label, LV_OPA_COVER, 0);
        lv_obj_set_style_pad_all(type_label, 5, 0);
        //lv_obj_set_style_radius(type_label, 4, 0);
        
        uint32_t bg_color = 0xCCCCCC; // Default color
        for (size_t j = 0; type_colors[j].type != NULL; j++)
        {
            if (strcmp(lesson.type, type_colors[j].type) == 0)
            {
                bg_color = type_colors[j].color;
                break;
            }
        }
        lv_obj_set_style_bg_color(type_label, lv_color_hex(bg_color), 0);
        
        // Subject label (WRAP)
        lv_obj_t* subject_label = lv_label_create(block);
        lv_label_set_text(subject_label, lesson.subject);
        lv_label_set_long_mode(subject_label, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(subject_label, 600);
        lv_obj_set_style_text_font(subject_label, &lv_font_my_montserrat_20, 0);
        //lv_obj_set_style_text_color(subject_label, lv_color_hex(0x525252), 0);
        lv_obj_set_style_text_color(subject_label, lv_color_hex(0x000000), 0);
        lv_obj_set_style_pad_top(subject_label, 5, 0);
        lv_obj_set_style_pad_bottom(subject_label, 10, 0);

        // Dashed line
        static lv_point_precise_t line_points[] = { {0, 0}, {600, 0} };
        lv_obj_t* line = lv_line_create(block);
        lv_line_set_points(line, line_points, 2);
        lv_obj_set_style_line_color(line, lv_color_hex(0x000000), 0);
        lv_obj_set_style_line_width(line, 1, 0);
        lv_obj_set_style_line_dash_width(line, 2, 0);
        lv_obj_set_style_line_dash_gap(line, 2, 0);
        lv_obj_set_width(line, 600);
        //lv_obj_center(line);

        // Teacher label
        lv_obj_t* teacher_label = lv_label_create(block);
        lv_label_set_text(teacher_label, lesson.teacher);
        lv_obj_set_width(teacher_label, 600);
        lv_obj_set_style_text_font(teacher_label, &lv_font_my_montserrat_20, 0);
        lv_obj_set_style_text_color(teacher_label, lv_color_hex(0x000000), 0);
    }
}

void update_progress_bar(void)
{
    if (!list_container) return;

    // Get current time
    time_t now = time(NULL);
    struct tm* current_time = localtime(&now);
    int current_minutes = current_time->tm_hour * 60 + current_time->tm_min;

    // Check if displayed date is today
    int is_today = (current_display_date.tm_year == current_time->tm_year &&
                    current_display_date.tm_mon == current_time->tm_mon &&
                    current_display_date.tm_mday == current_time->tm_mday);
    if (!is_today) return;

    // Get total number of lessons
    int lesson_count = get_lesson_count(); // Update to get_lesson_count_for_date(&current_display_date)

    // MIGHT BE OPTIMIZED
    // Update progress bar of current lesson
    for (int i = 0; i < lesson_count; i++)
    {
        if (progress_bars[i])
        {
            lesson_t lesson = get_lesson(i); // Update to get_lesson_for_date(&current_display_date, i)
            int start_minutes = lesson.start_hour * 60 + lesson.start_minute;
            int end_minutes = lesson.end_hour * 60 + lesson.end_minute;
            int progress = 0;
            if (current_minutes > end_minutes)
            {
                progress = 100;
            }
            else if (current_minutes >= start_minutes && current_minutes <= end_minutes)
            {
                progress = ((current_minutes - start_minutes) * 100) / (end_minutes - start_minutes);
            }
            lv_bar_set_value(progress_bars[i], progress, LV_ANIM_ON);
        }
    }
}

void init_schedule_ui(void)
{
    // Create main container
    list_container = lv_obj_create(lv_screen_active());
    lv_obj_set_size(list_container, 670, 400);
    lv_obj_center(list_container);
    lv_obj_set_scroll_dir(list_container, LV_DIR_VER); // Vertical scrolling only
    lv_obj_set_scrollbar_mode(list_container, LV_SCROLLBAR_MODE_ON);
    lv_obj_set_style_bg_color(list_container, lv_color_hex(0xFFFFFF), 0); // White background
    lv_obj_set_style_radius(list_container, 0, 0);
    lv_obj_add_flag(list_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(list_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(list_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_gap(list_container, 15, 0);

    // Initial update (current date)
    time_t now = time(NULL);
    struct tm* current_date = localtime(&now);
    update_schedule_display(current_date);
}
