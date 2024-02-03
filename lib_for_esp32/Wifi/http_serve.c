/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include <esp_http_server.h>
#include <stdio.h>
#include "http_sever_app.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "wifi_app.h"

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */
extern int station_set;
static const char *TAG = "HTTP SERVER";
httpd_handle_t server = NULL;
#define ESP_MAXIMUM_RETRY  6

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;
int station_setup = 0;
/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
static const char *TAGG = "wifi station";
static int s_retry_num = 0;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAGG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAGG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}


char ssid[32];
char pass[32];

 extern const uint8_t index_htlm_start[24] asm("_binary_index_html_start");
 extern const uint8_t index_htlm_end[22] asm("_binary_index_html_end");


 static http_post_wifi_callback_t http_post_wifi_callback = NULL;

/* An HTTP GET handler */
static esp_err_t hello_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req,"text/html");
    httpd_resp_send(req,(const char *)index_htlm_start, index_htlm_end - index_htlm_start);
    return ESP_OK;
}

 const httpd_uri_t get_data_dht11 = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = NULL
};


static esp_err_t wifi_hanler_get_from_server(httpd_req_t *req){
 char buf[32];
 char *token;
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;
        printf("%s and %d\n",buf,req->content_len);
       token = strtok(buf,"@");
       strcpy(ssid,token);
       token = strtok(NULL, "@");
       strcpy(pass,token);  
       http_post_wifi_callback(ssid,pass);
       
        
}
 // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t wifi = {
    .uri       = "/wifi_ssid_and_pass",
    .method    = HTTP_POST,
    .handler   = wifi_hanler_get_from_server,
    .user_ctx  = NULL
};




// start overhere // 
void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
         httpd_register_uri_handler(server, &get_data_dht11);
         httpd_register_uri_handler(server, &wifi);

    }
       else{ 
        ESP_LOGI(TAG, "Error starting server!");
       }
    }


 void stop_webserver(void)
{
    // Stop the httpd server
    httpd_stop(server);
}



void http_set_wifi_callback(void *cb){
    http_post_wifi_callback = cb;
}




void wifi_start_sta(void){
ESP_ERROR_CHECK(esp_wifi_start());
}
void wifi_stop_sta(void){
    esp_wifi_stop();
}

void wifi_sta_init(void){
    // config for wifi_station right here
    s_wifi_event_group = xEventGroupCreate();

    
    
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    printf("Hello\n");
    strcpy((char*)wifi_config.sta.ssid, ssid);
    strcpy((char*)wifi_config.sta.password, pass);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAGG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAGG, "connected to ap SSID:%s password:%s",
                 ssid, pass);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAGG, "Failed to connect to SSID:%s, password:%s ",
                 ssid, pass);
    } else {
        ESP_LOGE(TAGG, "UNEXPECTED EVENT");
    }
        
}

