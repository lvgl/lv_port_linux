#include "time_date_display.h"
#include <lvgl/lvgl.h>
#include <time.h>

static lv_obj_t* time_label;
static lv_obj_t* date_label;

static const char* days_of_week[] = {
    "Воскресенье", "Понедельник", "Вторник", "Среда",
    "Четверг", "Пятница", "Суббота"
};

static const char* months[] = {
    "января", "февраля", "марта", "апреля", "мая", "июня",
    "июля", "августа", "сентября", "октября", "ноября", "декабря"
};

void update_time_and_date_display(void)
{
    if (!time_label || !date_label) return;

    time_t now = time(NULL);
    struct tm* t = localtime(&now);

    char time_str[16];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", t->tm_hour, t->tm_min);
    lv_label_set_text(time_label, time_str);

    char date_str[64];
    snprintf(date_str, sizeof(date_str), "%s, %d %s %d",
        days_of_week[t->tm_wday], t->tm_mday, months[t->tm_mon], t->tm_year + 1900);
    lv_label_set_text(date_label, date_str);
}

void init_time_and_date_display(void)
{
    // Time label
    time_label = lv_label_create(lv_screen_active());
    lv_obj_align(time_label, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_label_set_text(time_label, "time not initialized");
    lv_obj_set_style_text_font(time_label, &lv_font_my_montserrat_20, 0);

    // Date label
    date_label = lv_label_create(lv_screen_active());
    lv_obj_align(date_label, LV_ALIGN_TOP_LEFT, 80, 10);
    lv_label_set_text(date_label, "date not initialized");
    lv_obj_set_style_text_font(date_label, &lv_font_my_montserrat_20, 0);

    update_time_and_date_display();
}
