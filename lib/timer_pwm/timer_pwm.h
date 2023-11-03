
#ifndef _TIMER_PWM_H
#define _TIMER_PWM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "driver/gptimer.h"

#define TIMER_PWM_CHANNEL_MAX 3
#define TIMER_PWM_BASIC_FREQ    1000*1000*40
#define TIMER_PWM_ALARM_COUNT   400

#define PWM_CH_PIN_0 27
#define PWM_CH_PIN_1 14
#define PWM_CH_PIN_2 33

    typedef struct {
        int pin;
        uint32_t freq;
        uint8_t duty;   //xx%
        volatile uint64_t freq_count;
        volatile uint64_t duty_count;
        volatile uint64_t count;
        volatile int num;        //-1:无限; 0:不使能
        volatile uint32_t sent;       //已经发送的数量
    } timer_pwm_channel_t;

    void timer_pwm_task(void* arg);
    void timer_pwm_init(void);

    void timer_pwm_attach_pin(uint8_t channel, int pin);
    void timer_pwm_set(uint8_t channel, uint32_t freq, uint8_t duty, int num);
    void timer_pwm_stop(uint8_t channel);
    int8_t timer_pwm_get_state(uint8_t channel);
    uint8_t timer_pwm_get_duty(uint8_t channel);
    uint32_t timer_pwm_get_freq(uint8_t channel);
    uint32_t timer_pwm_get_sent(uint8_t channel);
    int timer_pwm_get_num(uint8_t channel);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif

