#ifndef __ESP_EC11_H
#define __ESP_EC11_H
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define CONFIG_EC11_PIN_A 21
#define CONFIG_EC11_PIN_B 5
#define CONFIG_EC11_PIN_K 4

    typedef enum {
        EC_STA_NONE,
        EC_STA_A2B,
        EC_STA_B2A,
        EC_STA_FWD, //正转
        EC_STA_REV, //反转
    }EC11_STA_e;

    void EC11_Init(void);
    uint8_t Key_State(void);

    extern QueueHandle_t EC11EventQueue;
    extern int16_t ec11diff;

#ifdef __cplusplus
}
#endif
#endif