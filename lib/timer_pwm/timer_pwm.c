#include "timer_pwm.h"

static const char* TAG = "timer pwm";
static timer_pwm_channel_t _timer_pwm[TIMER_PWM_CHANNEL_MAX];
static gptimer_handle_t gptimer = NULL;

static bool IRAM_ATTR timer_pwm_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t* edata, void* user_data)
{
    for (int i = 0;i < TIMER_PWM_CHANNEL_MAX;i++)
    {
        if (_timer_pwm[i].num != 0)//使能
        {
            if (_timer_pwm[i].count == 0)
            {
                gpio_set_level(_timer_pwm[i].pin, 1);
            }
            else if (_timer_pwm[i].count == _timer_pwm[i].duty_count)
                gpio_set_level(_timer_pwm[i].pin, 0);
            _timer_pwm[i].count += 1;

            if (_timer_pwm[i].count >= _timer_pwm[i].freq_count)
            {
                _timer_pwm[i].count = 0;
                _timer_pwm[i].sent += 1;
                if (_timer_pwm[i].num >= 1)
                {
                    _timer_pwm[i].num -= 1;
                }
            }
        }
    }
    return pdFALSE;
}

void timer_pwm_attach_pin(uint8_t channel, int pin)
{
    if (channel >= TIMER_PWM_CHANNEL_MAX)
    {
        ESP_LOGE(TAG, "channel out of range");
        return;
    }
    if (pin < 0)
    {
        ESP_LOGE(TAG, "pin out of range");
        return;
    }
    _timer_pwm[channel].pin = pin;
    _timer_pwm[channel].num = 0;
    _timer_pwm[channel].sent = 0;
    _timer_pwm[channel].count = 0;
    _timer_pwm[channel].duty_count = 0;
    _timer_pwm[channel].freq = 0;
    pinMode(pin, GPIO_MODE_OUTPUT, GPIO_PULLDOWN_ONLY);
}

void timer_pwm_set(uint8_t channel, uint32_t freq, uint8_t duty, int num)
{
    _timer_pwm[channel].freq = freq;
    _timer_pwm[channel].duty = duty;
    _timer_pwm[channel].freq_count = (uint64_t)((TIMER_PWM_BASIC_FREQ / TIMER_PWM_ALARM_COUNT) / (double)(freq));
    _timer_pwm[channel].duty_count = (uint64_t)(_timer_pwm[channel].freq_count * duty / 100.0);
    _timer_pwm[channel].num = num;
    _timer_pwm[channel].sent = 0;
    _timer_pwm[channel].count = 0;
    printf("[PWM]: freq-%ld; dutt-%d; num-%d\n", freq, duty, num);
}

void timer_pwm_stop(uint8_t channel)
{
    _timer_pwm[channel].num = 0;
    gpio_set_level(_timer_pwm[channel].pin, 0);
}

void timer_pwm_init(void)
{
    /*通用定时器参数*/
    ESP_LOGI(TAG, "Create timer handle");
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = TIMER_PWM_BASIC_FREQ, // 定时器频率
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
    /*注册回调函数*/
    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_pwm_on_alarm_cb,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));

    ESP_LOGI(TAG, "Enable timer");
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    /*设置报警计数*/
    ESP_LOGI(TAG, "Start timer, stop it at alarm event");
    gptimer_alarm_config_t alarm_config1 = {
        .alarm_count = TIMER_PWM_ALARM_COUNT, //
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config1));
    ESP_LOGI(TAG, "Set count value");
    ESP_ERROR_CHECK(gptimer_set_raw_count(gptimer, 0));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
    /*初始化引脚*/
    timer_pwm_attach_pin(0, PWM_CH_PIN_0);
    timer_pwm_attach_pin(1, PWM_CH_PIN_1);
    timer_pwm_attach_pin(2, PWM_CH_PIN_2);
}

uint32_t timer_pwm_get_freq(uint8_t channel)
{
    return _timer_pwm[channel].freq;
}
uint32_t timer_pwm_get_sent(uint8_t channel)
{
    return _timer_pwm[channel].sent;
}
uint8_t timer_pwm_get_duty(uint8_t channel)
{
    return _timer_pwm[channel].duty;
}
/*!=发送pwm中*/
int8_t timer_pwm_get_state(uint8_t channel)
{
    if (_timer_pwm[channel].num == 0)
        return 0;
    else if (_timer_pwm[channel].num > 0) return 1;
    else return -1;
}
int timer_pwm_get_num(uint8_t channel)
{
    return _timer_pwm[channel].num;
}
void timer_pwm_task(void* arg)
{
    timer_pwm_init();

    vTaskDelay(pdMS_TO_TICKS(1000));
    timer_pwm_set(0, 10000, 70, 10);
    vTaskDelay(pdMS_TO_TICKS(10));
    timer_pwm_set(0, 1000, 10, 5);
    while (1)
    {
        ESP_LOGI("PWM", "num:%d", _timer_pwm[0].num);
        vTaskDelete(NULL);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    vTaskDelete(NULL);
}