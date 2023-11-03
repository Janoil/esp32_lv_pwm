#include "task_pwm.h"
#include "lv_cb.h"
#include "timer_pwm.h"


void TASK_PWM(void* param)
{
    /*----------pwm初始化----------*/
    timer_pwm_init();
    static char str[15];

    while (1)
    {
        if (timer_pwm_get_state(lv_cb_param.ch_current) != 0)//在发送pwm
        {
            if (!(lv_obj_get_state(ui_Button_Pause) & LV_STATE_CHECKED))
                lv_obj_add_state(ui_Button_Pause, LV_STATE_CHECKED);
        }
        else
        {
            if (lv_obj_get_state(ui_Button_Pause) & LV_STATE_CHECKED)
                lv_obj_clear_state(ui_Button_Pause, LV_STATE_CHECKED);
        }

        sprintf(str, "%ld  >", timer_pwm_get_sent(lv_cb_param.ch_current));
        lv_label_set_text(ui_Label_Sent_Num, str);

        ui_panel_set_schedule(ui_Panel_Sent, timer_pwm_get_sent(lv_cb_param.ch_current), timer_pwm_get_sent(lv_cb_param.ch_current) + timer_pwm_get_num(lv_cb_param.ch_current));

        vTaskDelay(pdMS_TO_TICKS(100));
    }
    vTaskDelete(NULL);
}