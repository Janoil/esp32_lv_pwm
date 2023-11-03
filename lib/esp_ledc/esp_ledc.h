#ifndef __ESP_LEDC_H
#define __ESP_LEDC_H
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_timer.h"

    void ledc_driver_init(void);
    void ledcSetup(uint8_t pin, ledc_channel_t chan, uint8_t bit_num);
    void ledcWrite(uint8_t chan, uint32_t freq, uint8_t percent, uint32_t num, esp_timer_create_args_t* timer_config);
    void ledcStop(void* chan);
#ifdef __cplusplus
}
#endif
#endif