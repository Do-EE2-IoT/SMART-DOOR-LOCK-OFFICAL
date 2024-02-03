#ifndef __RGB_H_
#define __RGB_H_
// define GPIO_NUM_FOR_RGB
#define LEDC_RED_GPIO GPIO_NUM_1
#define LEDC_GREEN_GPIO GPIO_NUM_3
#define LEDC_BLUE_GPIO GPIO_NUM_15


// define number of channel
#define LED_C_channel_num 3
// channel config typedef struct
typedef struct {
    int gpio_num;
    int speed_mode;
    int channel;
    int intr_type;
    int timer_select;
    int duty;
}ledc_info_t;

ledc_info_t led_ch[LED_C_channel_num];


/**
 * @brief color to define the WIFI initialization
*/
void RGB_color_WIFI_start(void);

/**
 * @brief color to indicate http server start
 * 
*/
void RGB_color_HTTP_start(void);

/**
 * @brief color to indicate wifi connected
*/
void RGB_color_WIFI_connected(void);




#endif