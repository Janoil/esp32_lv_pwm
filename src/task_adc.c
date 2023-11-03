#include "task_adc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "lv_cb.h"

#define EXAMPLE_ADC1_CHAN0 ADC_CHANNEL_7
static adc_oneshot_unit_handle_t adc1_handle;
static adc_cali_handle_t adc1_cali_handle;

void adc1_init(adc_oneshot_unit_handle_t* adc_handle, adc_cali_handle_t* ret_handle, adc_channel_t channel)
{
    /*创建 ADC 单元实例句柄*/
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, adc_handle));
    /*配置 ADC 单元实例*/
    adc_oneshot_chan_cfg_t config = {
    .bitwidth = ADC_BITWIDTH_DEFAULT,
    .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(*adc_handle, channel, &config));


    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, ret_handle));
}

void TASK_ADC(void* param)
{
    /*----------adc初始化----------*/
    pinMode(34, GPIO_MODE_INPUT, GPIO_FLOATING);
    adc1_init(&adc1_handle, &adc1_cali_handle, EXAMPLE_ADC1_CHAN0);
    ESP_LOGI("ADC", "ADC1 init success");
    char str[15];
    /*-----------------------------*/
    while (1)
    {
        Battery_t result = { 0 };
        adc_oneshot_read(adc1_handle, EXAMPLE_ADC1_CHAN0, &result.voltage);
        // adc_cali_raw_to_voltage(adc1_cali_handle, result.voltage, &result.voltage);
        if (result.voltage)
        {
            result.voltage = result.voltage * 1000 * 3.3 / 4095;
            result.voltage *= (4.43 / 2.12);//比例
            result.pow = gpio_get_level(34);
            if (result.pow)lv_obj_add_flag(ui_Image1, LV_OBJ_FLAG_HIDDEN);
            else lv_obj_clear_flag(ui_Image1, LV_OBJ_FLAG_HIDDEN);

            int16_t percent = (result.voltage - 3300) * 100 / (4000 - 3300);
            if (percent > 100)percent = 100;
            else if (percent < 0)percent = 0;
            sprintf(str, "%d%%", percent);
            lv_label_set_text(ui_Label_Battery, str);
            ui_panel_set_schedule(ui_Panel_Battery, percent, 100);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
    vTaskDelete(NULL);
}
