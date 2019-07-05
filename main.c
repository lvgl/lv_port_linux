#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_examples/lv_apps/demo/demo.h"
#include <unistd.h>

#define DISP_BUF_SIZE (80*LV_HOR_RES_MAX)

void dummy_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p)
{
    lv_disp_flush_ready(drv);
}
int main(void)
{
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    static lv_color_t buf[DISP_BUF_SIZE];
    static lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);
//
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = fbdev_flush;
    lv_disp_drv_register(&disp_drv);


    /*Create a Demo*/
    demo_create();


    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_tick_inc(5);
        lv_task_handler();
        usleep(5000);
    }

    return 0;
}
