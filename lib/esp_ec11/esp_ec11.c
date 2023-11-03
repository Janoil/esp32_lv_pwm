#include "esp_ec11.h"
#include "driver/gpio.h"

QueueHandle_t EC11EventQueue = NULL;
int16_t ec11diff = 0;

#define STA_A               gpio_get_level(CONFIG_EC11_PIN_A)
#define STA_B               gpio_get_level(CONFIG_EC11_PIN_B)


/*双pin中断检测*/
static void IRAM_ATTR ec11_isr_handler(void* arg)
{
    static EC11_STA_e state = EC_STA_NONE;
    for (int i = 0; i < 1000; i++);//delay
    uint32_t gpio_num = (uint32_t)arg;
    int flag = 0;

    if (gpio_num == CONFIG_EC11_PIN_A)
    {
        if (STA_A != 0)return;
    }
    else if (gpio_num == CONFIG_EC11_PIN_B)
    {
        if (STA_B != 0)return;
    }

    if (state == EC_STA_NONE)//起始
    {
        if (gpio_num == CONFIG_EC11_PIN_B && STA_A == 1)//
        {
            state = EC_STA_B2A;
        }
        else if (gpio_num == CONFIG_EC11_PIN_A && STA_B == 1)
        {
            state = EC_STA_A2B;
        }
    }
    else if (state == EC_STA_B2A)
    {
        if (gpio_num == CONFIG_EC11_PIN_A)//
        {
            if (STA_B == 0)
            {
                state = EC_STA_NONE;
                flag = -1;
            }
        }
        else state = EC_STA_NONE;

    }
    else if (state == EC_STA_A2B)
    {
        if (gpio_num == CONFIG_EC11_PIN_B)//
        {
            if (STA_A == 0)
            {
                state = EC_STA_NONE;
                flag = 1;
            }
        }
        else state = EC_STA_NONE;
    }

    if (flag != 0)
    {
        xQueueSendFromISR(EC11EventQueue, &flag, NULL);
    }
}

/*单pin中断检测*/
static void IRAM_ATTR ec11_B_isr_handler(void* arg)
{
    static EC11_STA_e state = EC_STA_NONE;

    int flag = 0;

    if (state == EC_STA_NONE)//起始
    {
        if (STA_B == 0)
        {
            if (STA_A == 0)state = EC_STA_FWD;
            else state = EC_STA_REV;
        }
    }
    else if (state == EC_STA_FWD)
    {
        if (STA_B == 1 && STA_A == 1)
            flag = 1;
        state = EC_STA_NONE;
    }
    else if (state == EC_STA_REV)
    {
        if (STA_B == 1 && STA_A == 0)//
            flag = -1;
        state = EC_STA_NONE;
    }


    if (flag != 0)
    {
        xQueueSendFromISR(EC11EventQueue, &flag, NULL);
    }
}



void EC11_Init(void)
{
    pinMode(CONFIG_EC11_PIN_A, GPIO_MODE_INPUT, GPIO_PULLUP_ONLY);
    pinMode(CONFIG_EC11_PIN_B, GPIO_MODE_INPUT, GPIO_PULLUP_ONLY);
    pinMode(CONFIG_EC11_PIN_K, GPIO_MODE_INPUT, GPIO_PULLUP_ONLY);

#if 0
    gpio_set_intr_type(CONFIG_EC11_PIN_B, GPIO_INTR_ANYEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(CONFIG_EC11_PIN_B, ec11_B_isr_handler, NULL);
#else
    gpio_set_intr_type(CONFIG_EC11_PIN_A, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(CONFIG_EC11_PIN_B, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(CONFIG_EC11_PIN_A, ec11_isr_handler, (void*)CONFIG_EC11_PIN_A);
    gpio_isr_handler_add(CONFIG_EC11_PIN_B, ec11_isr_handler, (void*)CONFIG_EC11_PIN_B);
#endif

    EC11EventQueue = xQueueCreate(20, sizeof(uint32_t));
    printf("EC11 init success\n");
    /* Add to the read cb */
    //     if (xQueueReceive(EC11EventQueue, &ec11, portMAX_DELAY)) {
    //         printf("isr:%d\n", ec11);
    //     }
}

uint8_t Key_State(void)
{
    return gpio_get_level(CONFIG_EC11_PIN_K);
}