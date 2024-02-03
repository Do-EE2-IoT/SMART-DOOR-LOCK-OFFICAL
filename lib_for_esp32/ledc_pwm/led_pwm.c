/* LEDC (LED Controller) fade example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "led_pwm.h"

/*
 * About this example
 *
 * 1. Start with initializing LEDC module:
 *    a. Set the timer of LEDC first, this determines the frequency
 *       and resolution of PWM.
 *    b. Then set the LEDC channel you want to use,
 *       and bind with one of the timers.
 *
 * 2. You need first to install a default fade function,
 *    then you can use fade APIs.
 *
 * 3. You can also set a target duty directly without fading.
 *
 * 4. This example uses GPIO18/19/4/5 as LEDC output,
 *    and it will change the duty repeatedly.
 *
 * 5. GPIO18/19 are from high speed channel group.
 *    GPIO4/5 are from low speed channel group.
 *
 */
#ifdef CONFIG_IDF_TARGET_ESP32
#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (18)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH1_GPIO       (19)
#define LEDC_HS_CH1_CHANNEL    LEDC_CHANNEL_1
#endif
#define LEDC_LS_TIMER          LEDC_TIMER_1
#define LEDC_LS_MODE           LEDC_LOW_SPEED_MODE
#ifdef CONFIG_IDF_TARGET_ESP32S2
#define LEDC_LS_CH0_GPIO       (18)
#define LEDC_LS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_LS_CH1_GPIO       (19)
#define LEDC_LS_CH1_CHANNEL    LEDC_CHANNEL_1
#endif
#define LEDC_LS_CH2_GPIO       (4)
#define LEDC_LS_CH2_CHANNEL    LEDC_CHANNEL_2
#define LEDC_LS_CH3_GPIO       (5)
#define LEDC_LS_CH3_CHANNEL    LEDC_CHANNEL_3

#define LEDC_TEST_CH_NUM       (4)
#define LEDC_TEST_DUTY         (4000)
#define LEDC_TEST_FADE_TIME    (3000)

 ledc_channel_config_t ledc_channel = {
            .channel    = LEDC_HS_CH0_CHANNEL,
            .duty       = 0,
            .gpio_num   = GPIO_NUM_2,
            .speed_mode = LEDC_HS_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0
    };

void pwm_init(){
/*
     * Prepare and set configuration of timers
     * that will be used by LED Controller
     */


    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = LEDC_HS_MODE,           // timer mode
        .timer_num = LEDC_TIMER_0,            // timer index
        .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
    };
     // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);

    ledc_channel_config(&ledc_channel);
    }

    void Pwm_duty(unsigned int duty){
    ledc_set_duty(ledc_channel.speed_mode, LEDC_CHANNEL_0, duty*82);
    ledc_update_duty(ledc_channel.speed_mode, LEDC_CHANNEL_0);
    }





    

//     while (1) {
//         printf("1. LEDC fade up to duty = %d\n", LEDC_TEST_DUTY);
//         for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
//             ledc_set_fade_with_time(ledc_channel[ch].speed_mode,
//                     ledc_channel[ch].channel, LEDC_TEST_DUTY, LEDC_TEST_FADE_TIME);
//             ledc_fade_start(ledc_channel[ch].speed_mode,
//                     ledc_channel[ch].channel, LEDC_FADE_NO_WAIT);
//         }
//         vTaskDelay(LEDC_TEST_FADE_TIME / portTICK_PERIOD_MS);

//         printf("2. LEDC fade down to duty = 0\n");
//         for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
//             ledc_set_fade_with_time(ledc_channel[ch].speed_mode,
//                     ledc_channel[ch].channel, 0, LEDC_TEST_FADE_TIME);
//             ledc_fade_start(ledc_channel[ch].speed_mode,
//                     ledc_channel[ch].channel, LEDC_FADE_NO_WAIT);
//         }
//         vTaskDelay(LEDC_TEST_FADE_TIME / portTICK_PERIOD_MS);

//         printf("3. LEDC set duty = %d without fade\n", LEDC_TEST_DUTY);
//         for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
//             ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, LEDC_TEST_DUTY);
//             ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
//         }
//         vTaskDelay(1000 / portTICK_PERIOD_MS);

//         printf("4. LEDC set duty = 0 without fade\n");
//         for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
//             ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, 0);
//             ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
//         }
//         vTaskDelay(1000 / portTICK_PERIOD_MS);
//     }
// }
