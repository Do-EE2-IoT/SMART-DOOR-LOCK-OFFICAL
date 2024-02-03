#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_err.h"

#include "stdbool.h"
#include "rgb_app.h"
// handle for RGB channel config
bool g_ledc_channel_handle = false;
/**
 * Initialize Led setting per channel
*/
 static void RGB_LEDC_channel_init(void){
    int channel = 3;
    // choose channel RGB for red
    led_ch[0].channel = LEDC_CHANNEL_0;
    led_ch[0].duty = 0;
    led_ch[0].gpio_num = LEDC_RED_GPIO;
    led_ch[0].intr_type = LEDC_INTR_DISABLE;
    led_ch[0].speed_mode = LEDC_HIGH_SPEED_MODE;
    led_ch[0].timer_select = LEDC_TIMER_0;
    
    //choose channel RGB for green
    led_ch[1].channel = LEDC_CHANNEL_1;
    led_ch[1].duty = 0;
    led_ch[1].gpio_num = LEDC_GREEN_GPIO;
    led_ch[1].intr_type = LEDC_INTR_DISABLE;
    led_ch[1].speed_mode = LEDC_HIGH_SPEED_MODE;
    led_ch[1].timer_select = LEDC_TIMER_0;
    
    // choose channel RGB for blue
    led_ch[2].channel = LEDC_CHANNEL_2;
    led_ch[2].duty = 0;
    led_ch[2].gpio_num = LEDC_BLUE_GPIO;
    led_ch[2].intr_type = LEDC_INTR_DISABLE;
    led_ch[2].speed_mode = LEDC_HIGH_SPEED_MODE;
    led_ch[2].timer_select = LEDC_TIMER_0;
    
    //config timer zero
    ledc_timer_config_t ledc_timer ={
        .duty_resolution = LEDC_TIMER_8_BIT,
        .freq_hz = 100,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
    };

    ledc_timer_config(&ledc_timer);
    // config channels
    for(channel = 0; channel < LED_C_channel_num; channel ++){
        ledc_channel_config_t ledc_channel = {
            .channel = led_ch[channel].channel,
            .duty = led_ch[channel].duty,
            .gpio_num = led_ch[channel].gpio_num,
            .intr_type = led_ch[channel].intr_type,
            .speed_mode = led_ch[channel].speed_mode,
            .timer_sel = led_ch[channel].timer_select,
        };
        ledc_channel_config(&ledc_channel);
    }
    g_ledc_channel_handle = true;
}

 static void RGB_led_set_color_static(uint8_t red,uint8_t green, uint8_t blue){
     // value should by 0 - 255 for 8 bits number
     ledc_set_duty(led_ch[0].speed_mode,led_ch[0].channel,red);
     ledc_update_duty(led_ch[0].speed_mode,led_ch[0].channel);
     
     ledc_set_duty(led_ch[1].speed_mode,led_ch[1].channel,green);
     ledc_update_duty(led_ch[1].speed_mode,led_ch[1].channel);

     ledc_set_duty(led_ch[2].speed_mode,led_ch[2].channel,blue);
     ledc_update_duty(led_ch[2].speed_mode,led_ch[2].channel);

     }


/**
 * @brief color to define the WIFI initialization
*/
void RGB_color_WIFI_start(void){
    // if RGB_channel has'nt init yet -> Call init
    if(g_ledc_channel_handle == false){
        RGB_LEDC_channel_init();
    }
    RGB_led_set_color_static(0,255,153);
}

/**
 * @brief color to indicate http server start
 * 
*/
void RGB_color_HTTP_start(void){
RGB_led_set_color_static(255,0,153);
   
}

/**
 * @brief color to indicate wifi connected
*/
void RGB_color_WIFI_connected(void){
RGB_led_set_color_static(255,255,0);
}

