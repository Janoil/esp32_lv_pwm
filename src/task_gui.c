#include "task_gui.h"
#include "lvgl_tft/disp_driver.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "esp_timer.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "demos/benchmark/lv_demo_benchmark.h"
#include "demos/keypad_encoder/lv_demo_keypad_encoder.h"

#include "task_adc.h"
#include "task_pwm.h"


#include "ui.h"
#include "lv_cb.h"

#define TAG "GUI"
#define LV_TICK_PERIOD_MS 5

static void lv_tick_task(void* arg) {
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

void lv_ui_pwm(void)
{
    g = lv_group_get_default();
    if (g == NULL) {
        g = lv_group_create();
        lv_group_set_default(g);
    }
    lv_indev_t* cur_drv = NULL;
    cur_drv = lv_indev_get_next(cur_drv);
    lv_indev_set_group(cur_drv, g);

    ui_init();
}

void TASK_GUI(void* param)
{
    SemaphoreHandle_t xGuiSemaphore = xSemaphoreCreateMutex();

    lv_init();
    /* Init display driver */
    lvgl_driver_init();

    /* Init lvgl buffer&draw */
    lv_port_disp_init();
    lv_port_indev_init();

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    // esp_register_freertos_tick_hook(lv_tick_task);
    const esp_timer_create_args_t periodic_timer_args = {
    .callback = &lv_tick_task,
    .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    /* Demo */
    // lv_demo_benchmark();
    // lv_demo_keypad_encoder();

    lv_ui_pwm();//persional
    ui_cb_init();//各项回调函数/功能初始化
    disp_backlight_set(blk_handle, 100);
    /*显示任务*/
    xTaskCreatePinnedToCore(TASK_ADC, "adc", 2048 * 2, NULL, 6, NULL, 1);//adc的显示相关
    xTaskCreatePinnedToCore(TASK_PWM, "pwm", 2048 * 2, NULL, 7, NULL, 1);//pwm的显示相关

    while (1)
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 20ms */
        vTaskDelay(pdMS_TO_TICKS(LV_DISP_DEF_REFR_PERIOD));
        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }
    /* A task should NEVER return */
    vTaskDelete(NULL);
}

