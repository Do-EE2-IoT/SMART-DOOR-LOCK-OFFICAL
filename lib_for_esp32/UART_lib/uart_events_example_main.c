
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/timers.h"
#include "freertos/semphr.h"
#include "string.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "uart.h"

static const char *TAG = "uart_events";

#define uart_num_x UART_NUM_0
#define BUF_SIZE 2048
#define USER_SIZE_QUEUE 0
#define RX_UART_NUM_2 16
#define TX_UART_NUM_2 17
#define CTS_UART_NUM_2 8
#define RTS_UART_NUM_2 7
#define PATTERN_CHR_NUM    (3)         /*!< Set the number of consecutive and identical characters received by receiver which defines a UART pattern*/

uart_call_back_t uart_call_back = NULL;

QueueHandle_t uart0_queue;



/*need to add lib "driver/uart.h", this one use uart_num_0 */
void uart_init()
{
 
    //****************************SET UP FOR UART****************************//
    esp_log_level_set(TAG, ESP_LOG_INFO);

    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    //Install UART driver, and get the queue.
    uart_driver_install(uart_num_x, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart0_queue, 0);
    uart_param_config(uart_num_x, &uart_config);

    //Set UART log level
    esp_log_level_set(TAG, ESP_LOG_INFO);
    //Set UART pins (using UART0 default pins ie no changes.)
    uart_set_pin(uart_num_x,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE);

    //Set uart pattern detect function.
    uart_enable_pattern_det_baud_intr(uart_num_x, '+', PATTERN_CHR_NUM, 9, 0, 0);
    //Reset the pattern queue length to record at most 20 pattern positions.

    uart_pattern_queue_reset(uart_num_x, 20);

    
    }
    
void UART_INTERRUPT_TASK(void *parameter){
    uint8_t *receive = (uint8_t *) malloc(BUF_SIZE);
    uart_event_t event;
    size_t buffered_size;
    receive = (uint8_t *)parameter;
    if(xQueueReceive(uart0_queue,(void *)&event,portMAX_DELAY) == pdPASS){
    switch(event.type){
                 case UART_DATA:
                   uart_read_bytes(uart_num_x,receive,event.size,portMAX_DELAY);
                   uart_call_back((char *)receive, strlen((char*)receive));
                    uart_write_bytes(uart_num_x,(const char*)receive,event.size);
                break;
                case UART_FIFO_OVF:
                    ESP_LOGI(TAG, "hw fifo overflow");
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    // The ISR has already reset the rx FIFO,
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(uart_num_x);
                    xQueueReset(uart0_queue);
                    break;
                //Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    ESP_LOGI(TAG, "ring buffer full");
                    // If buffer full happened, you should consider encreasing your buffer size
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(uart_num_x);
                    xQueueReset(uart0_queue);
                    break;
                //Event of UART RX break detected
                case UART_BREAK:
                    ESP_LOGI(TAG, "uart rx break");
                    break;
                //Event of UART parity check error
                case UART_PARITY_ERR:
                    ESP_LOGI(TAG, "uart parity error");
                    break;
                //Event of UART frame error
                case UART_FRAME_ERR:
                    ESP_LOGI(TAG, "uart frame error");
                    break;
                //UART_PATTERN_DET
                case UART_PATTERN_DET:
                    uart_get_buffered_data_len(uart_num_x, &buffered_size);
                    int pos = uart_pattern_pop_pos(uart_num_x);
                    ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
                    if (pos == -1) {
                        // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
                        // record the position. We should set a larger queue size.
                        // As an example, we directly flush the rx buffer here.
                        uart_flush_input(uart_num_x);
                    } else {
                        uart_read_bytes(uart_num_x, receive, pos, 100 / portTICK_PERIOD_MS);
                        uint8_t pat[PATTERN_CHR_NUM + 1];
                        memset(pat, 0, sizeof(pat));
                        uart_read_bytes(uart_num_x, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
                        ESP_LOGI(TAG, "read data: %s", receive);
                        ESP_LOGI(TAG, "read pat : %s", pat);
                    }
                    break;
                //Others
                default:
                ESP_LOGI(TAG, "uart event type: %d", event.type);
                    break;
    }
    }

    free(receive);
    receive = NULL;
    vTaskDelete(NULL);
   }


void uart_set_call_back(void *cb){
    uart_call_back = cb;
}