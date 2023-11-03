#include "esp_ledc.h"
#include "math.h"

#define LEDC_CH_PIN_0 27
#define LEDC_CH_PIN_1 14
#define LEDC_CH_PIN_2 33

static uint8_t channels_resolution[LEDC_CHANNEL_MAX] = { 0 };//各通道分辨率
esp_timer_handle_t ledc_cb_timer[LEDC_CHANNEL_MAX];

void ledc_cbs(void* args)
{
    static int count = 0;
    count++;
    printf("ledc count %d", count);
}

void ledc_driver_init(void)
{
    pinMode(LEDC_CH_PIN_0, GPIO_MODE_OUTPUT, GPIO_PULLUP_ONLY);
    pinMode(LEDC_CH_PIN_1, GPIO_MODE_OUTPUT, GPIO_PULLUP_ONLY);
    pinMode(LEDC_CH_PIN_2, GPIO_MODE_OUTPUT, GPIO_PULLUP_ONLY);
    // pinMode(LEDC_CH_PIN_3, GPIO_MODE_OUTPUT, GPIO_PULLUP_ONLY);
    // pinMode(LEDC_CH_PIN_4, GPIO_MODE_OUTPUT, GPIO_PULLUP_ONLY);

    ledcSetup(LEDC_CH_PIN_0, LEDC_CHANNEL_0, 10);
    ledcSetup(LEDC_CH_PIN_1, LEDC_CHANNEL_1, 10);
    ledcSetup(LEDC_CH_PIN_2, LEDC_CHANNEL_2, 10);
}

void ledcSetup(uint8_t pin, ledc_channel_t chan, uint8_t bit_num)
{
    if (chan >= LEDC_CHANNEL_MAX || bit_num > LEDC_TIMER_BIT_MAX) {
        ESP_LOGE("LEDC", "No more LEDC channels available! (maximum %u) or bit width too big (maximum %u)", LEDC_CHANNEL_MAX, LEDC_TIMER_BIT_MAX);
        return;
    }
    if (pin >= GPIO_NUM_MAX)
    {
        ESP_LOGE("LEDC", "Pin num exceed (maximum %u) ", GPIO_NUM_MAX);
        return;
    }

    uint8_t group = LEDC_HIGH_SPEED_MODE, timer = chan % 4;//((chan / 2) % 4);//直接定死通道对应的速率与定时器

    ledc_timer_config_t ledc_timer = {
        .speed_mode = group,
        .timer_num = timer,
        .duty_resolution = bit_num,
        .freq_hz = 1000,
        .clk_cfg = 0
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .channel = chan,
        .duty = 0,                            // 初始占空比为 0
        .gpio_num = pin,
        .speed_mode = group,
        .timer_sel = timer,
        .intr_type = LEDC_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    channels_resolution[chan] = bit_num;
    // printf("LEDC channel %u ,resolution: %u\n", chan, bit_num);
}

void ledcStop(void* chan)
{
    ledc_stop(LEDC_HIGH_SPEED_MODE, *(uint8_t*)chan, 0);
    printf("ledc stop channel %d\n", *(uint8_t*)chan);
}


void pwmstart_cb(void* args)
{
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, *(uint8_t*)args);
}

void ledcWrite(uint8_t chan, uint32_t freq, uint8_t percent, uint32_t num, esp_timer_create_args_t* timer_config)
{
    if (chan >= LEDC_CHANNEL_MAX) {
        ESP_LOGE("LEDC", "No more LEDC channels available! (maximum %u)", LEDC_CHANNEL_MAX);
        return;
    }
    else if (freq > 100000)
    {
        ESP_LOGE("LEDC", "Frequence out of range ! (maximum 100K)");
        return;
    }
    if (percent > 100)percent = 100;
    /*------------------------------------------------*/
    uint8_t max_resolution = (uint8_t)log2(80000000 / freq);
    if (channels_resolution[chan] > max_resolution)
        channels_resolution[chan] = max_resolution;

    uint8_t group = LEDC_HIGH_SPEED_MODE, timer = chan % 4;//((chan / 2) % 4);//直接定死通道对应的速率与定时器

    uint32_t max_duty = (1 << channels_resolution[chan]);
    uint32_t duty = max_duty * percent / 100;
    // printf("Max Duty %ld\n", max_duty);
    // printf("Duty %ld\n", duty);
    if ((duty == max_duty) && (max_duty != 1))
        duty = max_duty + 1;

    ledc_timer_config_t ledc_timer = {
    .speed_mode = group,
    .timer_num = timer,
    .duty_resolution = channels_resolution[chan],
    .freq_hz = freq,
    .clk_cfg = 0,
    };

    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    ledc_set_duty(group, chan, duty);

    // ESP_LOGI("LEDC", "Ledc write channel %d,%ldHz,%d%%", chan, freq, percent);
    /*------------------------------------------------*/
    if (num && timer_config != NULL)//指定个数的pwm
    {
        esp_timer_stop(ledc_cb_timer[chan]);
        esp_timer_delete(ledc_cb_timer[chan]);
        uint64_t alarm_ticks;

        float add = 0;

        if (freq < 1360)
            add = 0.5;
        else if (freq < 2730)
            add = 0;
        else if (freq < 3950)
            add = 0.5;
        else if (freq < 5230)
            add = 0.0;
        else if (freq < 6715)
            add = 0.5;
        else if (freq < 8160)
            add = 0.0;
        else if (freq < 9600)
            add = 0.5;


        alarm_ticks = (uint64_t)(1000000.0 / freq * (num + add));//us
        printf("%.2fus/per; %lldus/sum; add:%.1f\n", 1000000.0 / freq, alarm_ticks, add);
        printf("Send num #%ld\n", num);

        static uint8_t channel[LEDC_CHANNEL_MAX] = { 0 };
        channel[chan] = chan;
        esp_timer_create_args_t tconfig = {
        .callback = pwmstart_cb,
        .arg = (void*)&channel[chan],
        .name = "pwm_start",
        .skip_unhandled_events = true,
        };
        static esp_timer_handle_t ledc_cb_timer_x[LEDC_CHANNEL_MAX];

        ESP_ERROR_CHECK(esp_timer_create(&tconfig, &ledc_cb_timer_x[chan]));
        ESP_ERROR_CHECK(esp_timer_create(timer_config, &ledc_cb_timer[chan]));
        ESP_ERROR_CHECK(esp_timer_start_once(ledc_cb_timer_x[chan], 1));

        // ledc_update_duty(group, chan);
        ESP_ERROR_CHECK(esp_timer_start_once(ledc_cb_timer[chan], alarm_ticks));
        // time2stop(timer_config->arg, alarm_ticks);
    }
    else
        ledc_update_duty(group, chan);
}

