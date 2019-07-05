#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_examples/lv_apps/demo/demo.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define DISP_BUF_SIZE (80*LV_HOR_RES_MAX)

static void * tick_thread(void *data);

int main(void)
{
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = fbdev_flush;
    lv_disp_drv_register(&disp_drv);

    /*Create a Demo*/
    demo_create();

    /* Tick init.
     * You have to call 'lv_tick_inc()' in every milliseconds */
    pthread_t inc_x_thread;
    pthread_create(&inc_x_thread, NULL, tick_thread, NULL);

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_task_handler();
        usleep(5000);
    }

    return 0;
}

/**
 * A task to measure the elapsed time for LittlevGL
 * @param data unused
 * @return never return
 */
static void * tick_thread(void *data)
{
    uint32_t time_ms = 0;
    struct timespec begin;
    struct timespec end;
    while(1) {
        /*Sleep and measure the sleep time*/
        timespec_get(&begin, TIMER_ABSTIME);
        usleep(5000);
        timespec_get(&end, TIMER_ABSTIME);
        time_ms = (uint64_t)((uint64_t)end.tv_nsec - begin.tv_nsec) / 1000000; //in milliseconds
        lv_tick_inc(time_ms);
    }

    return NULL;
}
