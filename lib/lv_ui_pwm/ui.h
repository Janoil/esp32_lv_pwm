// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.2
// LVGL version: 8.3.6
// Project name: pwm

#ifndef _PWM_UI_H
#define _PWM_UI_H

#ifdef __cplusplus
extern "C" {
#endif

    #include "lvgl.h"

#include "ui_helpers.h"
#include "ui_events.h"
// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
extern lv_obj_t *ui_Screen1;
extern lv_obj_t *ui_Switch_Mode;
extern lv_obj_t *ui_Label_Title;
extern lv_obj_t *ui_Panel_Battery;
extern lv_obj_t *ui_Label_Battery;
extern lv_obj_t *ui_Button_Switch4;
extern lv_obj_t *ui_Label_CH4;
extern lv_obj_t *ui_Button_Switch3;
extern lv_obj_t *ui_Label_CH3;
extern lv_obj_t *ui_Button_Switch2;
extern lv_obj_t *ui_Label_CH2;
extern lv_obj_t *ui_Button_Switch1;
extern lv_obj_t *ui_Label_CH1;
extern lv_obj_t *ui_Button_Switch0;
extern lv_obj_t *ui_Label_CH0;
extern lv_obj_t *ui_Panel_Freq;
extern lv_obj_t *ui_Label_Freq;
extern lv_obj_t *ui_Spinbox_Freq;
extern lv_obj_t *ui_Label_Freq1;
extern lv_obj_t *ui_Panel_Duty;
extern lv_obj_t *ui_Label_Duty;
extern lv_obj_t *ui_Spinbox_Duty;
extern lv_obj_t *ui_Label_Duty1;
extern lv_obj_t *ui_Panel_Gener;
extern lv_obj_t *ui_Label_Gener;
extern lv_obj_t *ui_Spinbox_Gener;
extern lv_obj_t *ui_Label_Gener1;
extern lv_obj_t *ui_Panel_Sent;
extern lv_obj_t *ui_Label_Sent;
extern lv_obj_t *ui_Label_Sent_Num;
extern lv_obj_t *ui_Button_Gener;
extern lv_obj_t *ui_Button_Pause;
extern lv_obj_t *ui_Button_Save;
extern lv_obj_t *ui_Image1;
extern lv_obj_t *ui____initial_actions0;

LV_IMG_DECLARE( ui_img_kkl_240_png);   // assets\kkl_240.png
LV_IMG_DECLARE( ui_img_send_35px_png);   // assets\Send_35px.png
LV_IMG_DECLARE( ui_img_begin_35px_png);   // assets\Begin_35px.png
LV_IMG_DECLARE( ui_img_stop_35px_png);   // assets\Stop_35px.png
LV_IMG_DECLARE( ui_img_save_35px_png);   // assets\Save_35px.png
LV_IMG_DECLARE( ui_img_lighting_20px_png);   // assets\Lighting_20px.png
LV_IMG_DECLARE( ui_img_lighting_17px_png);   // assets\Lighting_17px.png

LV_FONT_DECLARE( ui_font_SmileySans_12);
LV_FONT_DECLARE( ui_font_SmileySans_16);
LV_FONT_DECLARE( ui_font_SmileySans_20);
LV_FONT_DECLARE( ui_font_SmileySans_24);
LV_FONT_DECLARE( ui_font_SmileySans_28);
LV_FONT_DECLARE( ui_font_SmileySans_32);
LV_FONT_DECLARE( ui_font_SmileySans_36);
LV_FONT_DECLARE( ui_font_SmileySans_40);

void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
