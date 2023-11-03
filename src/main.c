#include "main.h"
#include "task_uart.h"
#include "task_gui.h"


void app_main(void)
{
    // xTaskCreatePinnedToCore(TASK_UART, "uart", 2048, NULL, 10, NULL, 1);
    xTaskCreatePinnedToCore(TASK_GUI, "gui", 4096 * 3, NULL, 5, NULL, 0);
}

void pinMode(gpio_num_t gpio_num, gpio_mode_t mode, gpio_pull_mode_t pullup)
{
    gpio_config_t cfg = {
        .mode = mode,
        .pin_bit_mask = 1 << gpio_num,
    };
    if (mode == GPIO_MODE_INPUT)    //输入上拉
    {
        if (pullup == GPIO_PULLUP_ONLY)
            cfg.pull_up_en = 1;
        else if (pullup == GPIO_PULLDOWN_ONLY)
            cfg.pull_down_en = 1;
    }
    gpio_config(&cfg);
    if (mode == GPIO_MODE_OUTPUT)
        gpio_set_level(gpio_num, !pullup);
}