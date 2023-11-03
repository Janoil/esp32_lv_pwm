#include "lv_cb.h"
#include "ui.h"
#include "nvs_flash.h"
#include "timer_pwm.h"

lv_group_t* g = NULL;
lv_cb_param_t lv_cb_param = {
    .func_mode = FUNC_PWM,
    .ch_pwm_last = 0,
    .ch_dac_last = 3,
    .pwm_duty = {50,50,50},
    .pwm_freq = {100,100,100},
    .pwm_num = {10,10,10}
};

/*关掉属于的模式的通道*/
void ui_switch_disable(Generator_Func_e mode)
{
    if (mode == FUNC_PWM)
    {
        for (int i = 3;i < 5;i++)
        {
            lv_obj_set_style_bg_color(lv_cb_param.ch_switch_obj[i], lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DISABLED);
            lv_obj_set_style_bg_opa(lv_cb_param.ch_switch_obj[i], 200, LV_PART_MAIN | LV_STATE_DISABLED);
            lv_obj_clear_state(lv_cb_param.ch_switch_obj[i], LV_STATE_ANY);
            lv_obj_add_state(lv_cb_param.ch_switch_obj[i], LV_STATE_DISABLED);
        }
    }
    else
    {
        for (int i = 0;i < 3;i++)
        {
            lv_obj_set_style_bg_color(lv_cb_param.ch_switch_obj[i], lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DISABLED);
            lv_obj_set_style_bg_opa(lv_cb_param.ch_switch_obj[i], 200, LV_PART_MAIN | LV_STATE_DISABLED);
            lv_obj_clear_state(lv_cb_param.ch_switch_obj[i], LV_STATE_ANY);
            lv_obj_add_state(lv_cb_param.ch_switch_obj[i], LV_STATE_DISABLED);
        }
    }
}

void ui_panel_set_schedule(lv_obj_t* pobj, uint64_t value, uint64_t max)
{
    uint16_t main = (uint16_t)round(255.0 * value / max * 0.9);
    uint16_t grad = (uint16_t)round(255.0 * value / max * 1.1);
    if (main > 255)main = 255;
    if (grad > 255)grad = 255;
    // printf("%d|%d\n", main, grad);
    lv_obj_set_style_bg_main_stop(pobj, main, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(pobj, grad, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void ui_switch_check(Generator_Func_e mode, uint8_t ch)
{
    if (ch >= 5)
    {
        printf("switch error"); return;
    }
    //先全default
    for (int i = 0;i < 5;i++)
    {
        lv_obj_clear_state(lv_cb_param.ch_switch_obj[i], LV_STATE_CHECKED | LV_STATE_DISABLED);
        lv_obj_add_state(lv_cb_param.ch_switch_obj[i], LV_STATE_DEFAULT);
        //红色
        lv_obj_set_style_bg_color(lv_cb_param.ch_switch_obj[i], lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DISABLED);
        lv_obj_set_style_bg_opa(lv_cb_param.ch_switch_obj[i], 200, LV_PART_MAIN | LV_STATE_DISABLED);
    }
    //再disable
    ui_switch_disable(mode);
    //使能
    lv_obj_clear_state(lv_cb_param.ch_switch_obj[ch], LV_STATE_DEFAULT);
    lv_obj_add_state(lv_cb_param.ch_switch_obj[ch], LV_STATE_CHECKED);
    // lv_obj_add_state(lv_cb_param.ch_switch_obj[ch], LV_STATE_CHECKED | LV_STATE_DISABLED);
    //设置值
    lv_spinbox_set_value(ui_Spinbox_Freq, lv_cb_param.pwm_freq[ch]);
    lv_spinbox_set_value(ui_Spinbox_Duty, lv_cb_param.pwm_duty[ch]);
    lv_spinbox_set_value(ui_Spinbox_Gener, lv_cb_param.pwm_num[ch]);
    ui_panel_set_schedule(ui_Panel_Freq, lv_cb_param.pwm_freq[ch], 10000);
    ui_panel_set_schedule(ui_Panel_Duty, lv_cb_param.pwm_duty[ch], 100);
    ui_panel_set_schedule(ui_Panel_Gener, lv_cb_param.pwm_num[ch], 999999999);
    //设置开始停止图标
}

void ui_switch_ch(lv_event_t* e)
{
    lv_obj_t* target = lv_event_get_target(e);

    for (int i = 0;i < 5;i++)
        if (target == lv_cb_param.ch_switch_obj[i])
        {
            lv_cb_param.ch_current = i;break;
        }

    ui_switch_check(lv_cb_param.func_mode, lv_cb_param.ch_current);

    printf("Current channel %d\n", lv_cb_param.ch_current);
}

void ui_switch_title(lv_event_t* e)
{
    lv_obj_t* target = lv_event_get_target(e);
    lv_state_t state = lv_obj_get_state(target);

    if (state & LV_STATE_CHECKED)//PWM->DAC
    {
        lv_cb_param.func_mode = FUNC_DAC;
        lv_cb_param.ch_pwm_last = lv_cb_param.ch_current;
        lv_cb_param.ch_current = lv_cb_param.ch_dac_last;
        lv_label_set_text(ui_Label_Title, "DAC Generater");
    }
    else//DAC->PWM
    {
        lv_cb_param.func_mode = FUNC_PWM;
        lv_cb_param.ch_dac_last = lv_cb_param.ch_current;
        lv_cb_param.ch_current = lv_cb_param.ch_pwm_last;
        lv_label_set_text(ui_Label_Title, "PWM Generater");
    }
    ui_switch_check(lv_cb_param.func_mode, lv_cb_param.ch_current);
}

void spinbox_change_freq(lv_event_t* e)
{
    if (lv_cb_param.ch_current > 2)
    {
        printf("[spinbox]: Current channel is not valid\n");
        return;
    }
    lv_obj_t* target = lv_event_get_target(e);
    uint32_t value = (uint32_t)lv_spinbox_get_value(target);
    lv_cb_param.pwm_freq[lv_cb_param.ch_current] = value;
    ui_panel_set_schedule(ui_Panel_Freq, value, 10000);
}

void spinbox_change_duty(lv_event_t* e)
{
    lv_obj_t* target = lv_event_get_target(e);
    uint32_t value = (uint32_t)lv_spinbox_get_value(target);
    lv_cb_param.pwm_duty[lv_cb_param.ch_current] = value;
    ui_panel_set_schedule(ui_Panel_Duty, value, 100);
}

void spinbox_change_generate_num(lv_event_t* e)
{
    lv_obj_t* target = lv_event_get_target(e);
    uint32_t value = (uint32_t)lv_spinbox_get_value(target);
    lv_cb_param.pwm_num[lv_cb_param.ch_current] = value;
    ui_panel_set_schedule(ui_Panel_Gener, value, 999999999);
}

void pwm_generate(lv_event_t* e)
{
    if (lv_cb_param.pwm_num[lv_cb_param.ch_current] &&
        lv_cb_param.pwm_freq[lv_cb_param.ch_current])
        timer_pwm_set(lv_cb_param.ch_current, lv_cb_param.pwm_freq[lv_cb_param.ch_current], lv_cb_param.pwm_duty[lv_cb_param.ch_current], lv_cb_param.pwm_num[lv_cb_param.ch_current]);
    else printf("[Generate] No number to send");
}

void pwm_start_stop(lv_event_t* e)
{
    lv_obj_t* target = lv_event_get_target(e);
    lv_state_t state = lv_obj_get_state(target);
    //根据ch_current来开关pwm
    if (state & LV_STATE_CHECKED)  //开始
        timer_pwm_set(lv_cb_param.ch_current, lv_cb_param.pwm_freq[lv_cb_param.ch_current], lv_cb_param.pwm_duty[lv_cb_param.ch_current], -1);
    else    //停止
        timer_pwm_stop(lv_cb_param.ch_current);
}

void ui_save_param(lv_event_t* e)
{
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvs);
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    if (err != ESP_OK)
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    else
        printf("Done\n");

    printf("Saving lvgl callback parameter... ");
    err = nvs_set_blob(nvs, "lv_cb_param", &lv_cb_param, sizeof(lv_cb_param));

    if (err != ESP_OK)
        printf("Error (%s) saving!\n", esp_err_to_name(err));
    else
    {
        printf("Done\n");
        uint8_t tmp_data[96];
        memcpy(tmp_data, &lv_cb_param, sizeof(lv_cb_param));
        printf("lv_cb_param:\n  ");
        for (int i = 0;i < sizeof(lv_cb_param);i++)
            printf("%02X ", tmp_data[i]);
        printf("\n");
    }
    printf("Committing updates in NVS ... ");
    err = nvs_commit(nvs);
    if (err != ESP_OK)
        printf("Error (%s) saving!\n", esp_err_to_name(err));
    else
        printf("Done\n");
    nvs_close(nvs);
}

void ui_obj_set_style(lv_obj_t* pobj)
{
    lv_obj_set_style_outline_opa(pobj, 0, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_outline_opa(pobj, 0, LV_PART_MAIN | LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_opa(pobj, 0, LV_PART_MAIN | LV_STATE_EDITED);
    lv_obj_set_style_border_opa(pobj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_color(pobj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(pobj, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_text_color(pobj, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_FOCUS_KEY);
    lv_obj_set_style_text_color(pobj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_EDITED);
    lv_obj_set_style_text_decor(pobj, LV_TEXT_DECOR_UNDERLINE, LV_PART_MAIN | LV_STATE_FOCUSED);
    lv_obj_set_style_text_decor(pobj, LV_TEXT_DECOR_UNDERLINE, LV_PART_MAIN | LV_STATE_FOCUS_KEY);
}

void ui_spinbox_set_style(lv_obj_t* pobj)
{
    lv_obj_set_style_text_letter_space(pobj, 1, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_font(pobj, &ui_font_SmileySans_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_text_font(pobj, &ui_font_SmileySans_20, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(pobj, lv_color_hex(0x000000), LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(pobj, lv_color_hex(0xFF0000), LV_PART_CURSOR | LV_STATE_FOCUS_KEY);
    lv_obj_set_style_text_decor(pobj, LV_TEXT_DECOR_UNDERLINE, LV_PART_CURSOR | LV_STATE_FOCUS_KEY);

    lv_obj_set_style_bg_opa(pobj, 0, LV_PART_CURSOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(pobj, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_obj_set_style(pobj);
}

void ui_cb_init(void)
{
    /*----------nvs初始化----------*/
    nvs_flash_init();
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &nvs);
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    if (err != ESP_OK)
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    else
        printf("Done\n");
    uint8_t tmp_data[96] = { 0 };
    size_t tmp_size = sizeof(lv_cb_param);
    printf("Reading lvgl callback parameter... ");
    err = nvs_get_blob(nvs, "lv_cb_param", (uint8_t*)&lv_cb_param, &tmp_size);
    if (err != ESP_OK)
    {
        printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
    else
    {
        printf("Done\n");
        memcpy(tmp_data, (uint8_t*)&lv_cb_param, sizeof(lv_cb_param));
    }
    nvs_close(nvs);
    /*----------指定----------*/
    lv_cb_param.ch_switch_obj[0] = ui_Button_Switch0;
    lv_cb_param.ch_switch_obj[1] = ui_Button_Switch1;
    lv_cb_param.ch_switch_obj[2] = ui_Button_Switch2;
    lv_cb_param.ch_switch_obj[3] = ui_Button_Switch3;
    lv_cb_param.ch_switch_obj[4] = ui_Button_Switch4;
    //修正style
    lv_spinbox_set_digit_step_direction(ui_Spinbox_Freq, LV_DIR_LEFT);
    lv_spinbox_set_digit_step_direction(ui_Spinbox_Duty, LV_DIR_LEFT);
    lv_spinbox_set_digit_step_direction(ui_Spinbox_Gener, LV_DIR_LEFT);
    ui_spinbox_set_style(ui_Spinbox_Freq);
    ui_spinbox_set_style(ui_Spinbox_Duty);
    ui_spinbox_set_style(ui_Spinbox_Gener);
    ui_obj_set_style(ui_Switch_Mode);
    ui_obj_set_style(ui_Button_Pause);
    ui_obj_set_style(ui_Button_Gener);
    ui_obj_set_style(ui_Button_Save);
    for (int i = 0; i < 5; i++)
        ui_obj_set_style(lv_cb_param.ch_switch_obj[i]);
    //初始化各项显示值
    ui_switch_check(lv_cb_param.func_mode, lv_cb_param.ch_current);
    /*----------回调注册----------*/
    //模式选择
    lv_obj_add_event_cb(ui_Switch_Mode, ui_switch_title, LV_EVENT_RELEASED, NULL);
    //通道选择
    lv_obj_add_event_cb(ui_Button_Switch0, ui_switch_ch, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(ui_Button_Switch1, ui_switch_ch, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(ui_Button_Switch2, ui_switch_ch, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(ui_Button_Switch3, ui_switch_ch, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(ui_Button_Switch4, ui_switch_ch, LV_EVENT_RELEASED, NULL);
    //改变值
    lv_obj_add_event_cb(ui_Spinbox_Freq, spinbox_change_freq, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_Spinbox_Duty, spinbox_change_duty, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(ui_Spinbox_Gener, spinbox_change_generate_num, LV_EVENT_VALUE_CHANGED, NULL);
    //按钮
    lv_obj_add_event_cb(ui_Button_Gener, pwm_generate, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(ui_Button_Pause, pwm_start_stop, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(ui_Button_Save, ui_save_param, LV_EVENT_RELEASED, NULL);
    /*--------------------------------------------------*/
    //删除group
    lv_group_remove_obj(ui_Switch_Mode);
    lv_group_focus_obj(ui_Button_Gener);
}

void ui_cb_handler(void)
{

}



