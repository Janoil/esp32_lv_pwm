// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.2
// LVGL version: 8.3.6
// Project name: pwm

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////


// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
lv_obj_t *ui_Screen1;
lv_obj_t *ui_Switch_Mode;
lv_obj_t *ui_Label_Title;
lv_obj_t *ui_Panel_Battery;
lv_obj_t *ui_Label_Battery;
lv_obj_t *ui_Button_Switch4;
lv_obj_t *ui_Label_CH4;
lv_obj_t *ui_Button_Switch3;
lv_obj_t *ui_Label_CH3;
lv_obj_t *ui_Button_Switch2;
lv_obj_t *ui_Label_CH2;
lv_obj_t *ui_Button_Switch1;
lv_obj_t *ui_Label_CH1;
lv_obj_t *ui_Button_Switch0;
lv_obj_t *ui_Label_CH0;
lv_obj_t *ui_Panel_Freq;
lv_obj_t *ui_Label_Freq;
lv_obj_t *ui_Spinbox_Freq;
lv_obj_t *ui_Label_Freq1;
lv_obj_t *ui_Panel_Duty;
lv_obj_t *ui_Label_Duty;
lv_obj_t *ui_Spinbox_Duty;
lv_obj_t *ui_Label_Duty1;
lv_obj_t *ui_Panel_Gener;
lv_obj_t *ui_Label_Gener;
lv_obj_t *ui_Spinbox_Gener;
lv_obj_t *ui_Label_Gener1;
lv_obj_t *ui_Panel_Sent;
lv_obj_t *ui_Label_Sent;
lv_obj_t *ui_Label_Sent_Num;
lv_obj_t *ui_Button_Gener;
lv_obj_t *ui_Button_Pause;
lv_obj_t *ui_Button_Save;
lv_obj_t *ui_Image1;
lv_obj_t *ui____initial_actions0;
const lv_img_dsc_t *ui_imgset_kkl_[1] = {&ui_img_kkl_240_png};

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=1
    #error "LV_COLOR_16_SWAP should be 1 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

///////////////////// SCREENS ////////////////////

void ui_init( void )
{
lv_disp_t *dispp = lv_disp_get_default();
lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
lv_disp_set_theme(dispp, theme);
ui_Screen1_screen_init();
ui____initial_actions0 = lv_obj_create(NULL);
lv_disp_load_scr( ui_Screen1);
}