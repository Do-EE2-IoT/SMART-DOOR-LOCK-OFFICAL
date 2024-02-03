/* RMT example -- RGB LED Strip

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "led_strip.h"
#include "wsheader_me.h"
#include "led_strip.h"
static const char *TAG = "example";

#define RMT_TX_CHANNEL RMT_CHANNEL_0

#define EXAMPLE_CHASE_SPEED_MS (10)
#define CONFIG_EXAMPLE_STRIP_LED_NUMBER 4
/**
 * @brief Simple helper function, converting HSV color space to RGB color space
 *
 * Wiki: https://en.wikipedia.org/wiki/HSL_and_HSV
 *
 */

led_strip_t *strip;
void ws2812_init(int txpin, int number_of_led)
{
    
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(txpin, RMT_TX_CHANNEL);
    // set counter clock to 40MHz
    config.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(number_of_led, (led_strip_dev_t)config.channel);
strip = led_strip_new_rmt_ws2812(&strip_config);

    // install ws2812 driver
    if (!strip) {
        ESP_LOGE(TAG, "install WS2812 driver failed");
    }
    // Clear LED strip (turn off all LEDs)
    ESP_ERROR_CHECK(strip->clear(strip, 100));
    // Show simple rainbow chasing pattern
    ESP_LOGI(TAG, "LED Rainbow Chase Start");
}
// index is equal to LED number x 
void ws2812_set_color(int index,int r, int g, int b){
   
   ESP_ERROR_CHECK(strip->set_pixel(strip, index, r, g, b));
}


void ws2812_update_color(void){
ESP_ERROR_CHECK(strip->refresh(strip, 100));
vTaskDelay(pdMS_TO_TICKS(EXAMPLE_CHASE_SPEED_MS));
}

