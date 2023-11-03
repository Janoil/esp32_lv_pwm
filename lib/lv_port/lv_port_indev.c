/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "esp_ec11.h"
 /*********************
  *      DEFINES
  *********************/

  /**********************
   *      TYPEDEFS
   **********************/

   /**********************
    *  STATIC PROTOTYPES
    **********************/
static void encoder_init(void);
static void encoder_read(lv_indev_drv_t* indev_drv, lv_indev_data_t* data);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t* indev_encoder;
static lv_indev_drv_t indev_drv;

/**********************
 *      MACROS
 **********************/

 /**********************
  *   GLOBAL FUNCTIONS
  **********************/

void lv_port_indev_init(void)
{
    /*------------------
     * Encoder
     * -----------------*/

     /*Initialize your encoder if you have*/
    encoder_init();

    /*Register a encoder input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = encoder_read;
    // indev_drv.long_press_time = 500;    //按下500ms位长按
    // indev_drv.long_press_repeat_time = 500;
    indev_encoder = lv_indev_drv_register(&indev_drv);

    /* Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     * add objects to the group with `lv_group_add_obj(group, obj)`
     * and assign this input device to group to navigate in it:
     * `lv_indev_set_group(indev_encoder, group);` */
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

 /*------------------
  * Encoder
  * -----------------*/

  /*Initialize your keypad*/
static void encoder_init(void)
{
    /*Your code comes here*/
    EC11_Init();
}

/*Will be called by the library to read the encoder*/
static void encoder_read(lv_indev_drv_t* indev_drv, lv_indev_data_t* data)
{
    data->enc_diff = 0;
    data->state = LV_INDEV_STATE_RELEASED;
    int32_t encoder_diff = 0;

    if (gpio_get_level(4) == 0)
    {
        data->state = LV_INDEV_STATE_PRESSED;
        // printf("pressed\n");
    }
    if (xQueueReceive(EC11EventQueue, &encoder_diff, 0))
    {
        // printf("isr:%ld\n", encoder_diff);
        data->enc_diff = encoder_diff;
    }
}
