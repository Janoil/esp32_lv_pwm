#ifndef __TASK_ADC_H
#define __TASK_ADC_H
#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

    typedef struct {
        int voltage;//电源电压 mV
        uint8_t pow;//是否有usb电源
    }Battery_t;

    void TASK_ADC(void* param);

#ifdef __cplusplus
}
#endif
#endif