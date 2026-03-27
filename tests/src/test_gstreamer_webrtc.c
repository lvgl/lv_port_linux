#if LV_BUILD_TEST

#include "lvgl.h"
#include "unity/unity.h"

#if LV_USE_GSTREAMER

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>     /* usleep */

#ifndef NON_AMD64_BUILD
    #define EXT_NAME ".lp64.png"
#else
    #define EXT_NAME ".lp32.png"
#endif

#define TEST_WEBRTC_SIGNAL_URI_ENV "LV_TEST_WEBRTC_SIGNAL_URI"
#define TEST_WEBRTC_WAIT_TIMEOUT_MS 15000U
#define TEST_WEBRTC_POLL_STEP_MS    20U

static void test_wait_ms(uint32_t wait_ms)
{
    uint32_t elapsed = 0;
    while(elapsed < wait_ms) {
        lv_timer_handler();
        lv_tick_inc(TEST_WEBRTC_POLL_STEP_MS);
        usleep((useconds_t)TEST_WEBRTC_POLL_STEP_MS * 1000u);
        elapsed += TEST_WEBRTC_POLL_STEP_MS;
    }
}

void setUp(void)
{
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

void test_gstreamer_webrtc_smpte75(void)
{
    const char * signal_uri = getenv(TEST_WEBRTC_SIGNAL_URI_ENV);
    TEST_ASSERT_NOT_NULL_MESSAGE(signal_uri, "Missing LV_TEST_WEBRTC_SIGNAL_URI environment variable");
    TEST_ASSERT_GREATER_THAN_MESSAGE(0, lv_strlen(signal_uri), "Empty LV_TEST_WEBRTC_SIGNAL_URI environment variable");

    lv_obj_t * gstreamer = lv_gstreamer_create(lv_screen_active());
    TEST_ASSERT_NOT_NULL(gstreamer);

    lv_result_t set_src_res = lv_gstreamer_set_src(gstreamer,
                                                   LV_GSTREAMER_FACTORY_WEBRTCSRC,
                                                   LV_GSTREAMER_PROPERTY_WEBRTCSRC,
                                                   signal_uri);
    TEST_ASSERT_EQUAL_MESSAGE(LV_RESULT_OK, set_src_res, "Failed to set WebRTC source");

    lv_obj_set_size(gstreamer, 320, 240);
    lv_obj_center(gstreamer);

    lv_gstreamer_play(gstreamer);

    uint32_t elapsed = 0;
    while(elapsed < TEST_WEBRTC_WAIT_TIMEOUT_MS) {
        lv_timer_handler();

        lv_gstreamer_state_t state = lv_gstreamer_get_state(gstreamer);
        int32_t width = lv_image_get_src_width(gstreamer);
        int32_t height = lv_image_get_src_height(gstreamer);
        if(state == LV_GSTREAMER_STATE_PLAYING && width > 0 && height > 0) {
            break;
        }

        lv_tick_inc(TEST_WEBRTC_POLL_STEP_MS);
        usleep((useconds_t)TEST_WEBRTC_POLL_STEP_MS * 1000u);
        elapsed += TEST_WEBRTC_POLL_STEP_MS;
    }

    TEST_ASSERT_TRUE_MESSAGE(elapsed < TEST_WEBRTC_WAIT_TIMEOUT_MS,
                             "Timed out waiting for WebRTC stream to start");

    test_wait_ms(500);
    lv_refr_now(NULL);

    TEST_ASSERT_EQUAL_SCREENSHOT("gstreamer/webrtc_smpte75" EXT_NAME);

    lv_gstreamer_stop(gstreamer);
}

#else

void setUp(void)
{
}

void tearDown(void)
{
}

void test_gstreamer_webrtc_smpte75(void)
{
}

#endif /* LV_USE_GSTREAMER */

#endif /* LV_BUILD_TEST */
