
// FREERTOS HEADER
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

// ESP HEADER
#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "lwip/netdb.h"
#include "esp_wifi_types.h"
#include "http_sever_app.h"

//header of me
// wifi application task
#define WIFI_APP_TASK_STACK_SIZE 4096
#define WIFI_APP_TASK_PRIORITY 5
#define WIFI_APP_TASK_CORE_ID 0

#include "wifi_app.h"


// TAG used for ESP serial console message
static const char TAG[] = "wifi app";
// Queue handle used to manipulate the main queue event
static QueueHandle_t wifi_app_queue_handle = NULL;

//netif objects for station and access point
esp_netif_t* esp_netif_sta = NULL;
esp_netif_t* esp_netif_ap = NULL;
/**
 * WiFi application event handler
 * @param arg data, aside from event data, that is passed to the handler when it is called
 * @param event_base the base id of the event to register the handler for
 * @param event_id the id fo the event to register the handler for
 * @param event_data event data
 */
static void wifi_app_event_handler(void *parameter, esp_event_base_t event_base,int32_t event_id,void *data){
            if(event_base == WIFI_EVENT){
                switch(event_id){
                    case WIFI_EVENT_AP_START:
                    ESP_LOGI(TAG,"WIFI_EVENT_AP_START");
                    break;

                    case WIFI_EVENT_AP_STOP:
                    ESP_LOGI(TAG,"WIFI_EVENT_AP_STOP");
                    break;
                    
                    case WIFI_EVENT_AP_STACONNECTED:
                    ESP_LOGI(TAG,"WIFI_EVENT_STA_CONNECTED");
                    break;

                    case WIFI_EVENT_AP_STADISCONNECTED:
                    ESP_LOGI(TAG,"WIFI_EVENT_STA_DISCONNECTED");
                    break;

                    case WIFI_EVENT_STA_START:
                    ESP_LOGI(TAG,"WIFI_EVENT_STA_START");
                    break;

                    case WIFI_EVENT_STA_STOP:
                    ESP_LOGI(TAG,"WIFI_EVENT_STA_STOP");
                    break;

                    case WIFI_EVENT_STA_CONNECTED:
                    ESP_LOGI(TAG,"WIFI_EVENT_STA_CONNECTED");
                    break;

                    case WIFI_EVENT_STA_DISCONNECTED:
                    ESP_LOGI(TAG,"WIFI_EVENT_STA_DISCONNECTED");
                    break;
                }
            }
        else if(event_base == IP_EVENT){
            switch(event_id){
                case IP_EVENT_STA_GOT_IP:
                ESP_LOGI(TAG,"IP_EVENT_STA_GOT_IP");
            }
        }
}

/**
 * @brief Initialize The TCP stack and default wifi configuration
*/
void wifi_app_default_wifi_config(void ){
      // Initialize the TCP stack

      // Default wifi config - operation must be this order 
      wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
      ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
      ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
      esp_netif_sta = esp_netif_create_default_wifi_sta();
      esp_netif_ap = esp_netif_create_default_wifi_ap();
}

/**
 * @brief config the wifi access point 
*/
void wifi_app_softap_config(void){
      // soft AP WIFI access point configuration
      wifi_config_t wifi_ap_config = {
        .ap = {
            .ssid = WIFI_AP_SSID,
            .ssid_len = strlen(WIFI_AP_SSID),
            .password = WIFI_AP_PASSWORD,
            .channel = WIFI_AP_CHANNEL,
            .ssid_hidden = WIFI_AP_SSID_HIDDEN,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .max_connection = WIFI_AP_MAX_CONNECTIONS,
            .beacon_interval = WIFI_AP_BEACON_INTERVAL,
        },
      };
      
      // config DHCP for the Access point mode
    esp_netif_ip_info_t ap_ip_info;
    memset(&ap_ip_info,0x00,sizeof(ap_ip_info));

    esp_netif_dhcps_stop(esp_netif_ap); // must call this first
    inet_pton(AF_INET,WIFI_AP_IP, &ap_ip_info.ip);    // Assign access points static IP/ gW / netmask
    inet_pton(AF_INET,WIFI_AP_GATEWAY,&ap_ip_info.gw);
    inet_pton(AF_INET,WIFI_AP_NETMASK,&ap_ip_info.netmask);
    ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_netif_ap,&ap_ip_info));  //statically configure the network interface
    ESP_ERROR_CHECK(esp_netif_dhcps_start(esp_netif_ap)); // start ap DHCP server
 
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA)); // setting mode with accesspoint / station
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP,&wifi_ap_config)); // config all tham so 
    ESP_ERROR_CHECK(esp_wifi_set_bandwidth(WIFI_IF_AP,WIFI_AP_BANDWIDTH));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_STA_POWER_SAVE));
 }  
      
/**
 * @brief Initialize the Wifi application event handler for WIFI and IP event
*/
void wifi_app_event_handler_init(void){
       // event loop for wifi driver
       // Event handler for the connection
       esp_event_handler_instance_t Instance_wifi_event;
       esp_event_handler_instance_t Instance_ip_event;
       
       // Thong so dau la x_event
       ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,ESP_EVENT_ANY_ID,&wifi_app_event_handler,NULL,&Instance_wifi_event));
       ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,ESP_EVENT_ANY_ID,&wifi_app_event_handler,NULL,&Instance_ip_event));
}

static void wifi_app_task(void *parameters){
    wifi_app_queue_message_t msg;
    
    //Initialize event handler 

    // Initialize the TCP/IP stack and WIFI config
    // wifi_app_default_wifi_config();

    //soft ap config
    // wifi_app_softap_config();

    // start wifi
    ESP_ERROR_CHECK(esp_wifi_start());

    // send first event message
    wifi_app_send_message(WIFI_APP_MSG_START_HTTP_SERVER);

    while(1){
        if(xQueueReceive(wifi_app_queue_handle,&msg,portMAX_DELAY)){
          switch(msg.msgID){
            case WIFI_APP_MSG_START_HTTP_SERVER:
                ESP_LOGI(TAG,"WIFI_APP_MSG_START_HTTP_SERVER");
                 start_webserver();
                // RGB_color_HTTP_start();
                break;
            case WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER:
                 ESP_LOGI(TAG,"WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER");

                break;
            case WIFI_APP_MSG_STA_CONNECTED_GOT_IP:
                 ESP_LOGI(TAG,"WIFI_APP_MSG_STA_CONNECTED_GOT_IP");
                //  RGB_color_WIFI_connected();
                 break;

            default:
                break;
        }
        }
    }
}

BaseType_t wifi_app_send_message(wifi_app_message_e msgID){
    wifi_app_queue_message_t msg;
    msg.msgID = msgID;
    return xQueueSend(wifi_app_queue_handle,&msgID,portMAX_DELAY);
}

void wifi_app_start(void){
    ESP_LOGI(TAG,"Start WIFI APPLICATION");

    // start color of wifi start
    // RGB_color_WIFI_start();

    //Disable default Wifi logging massages
    esp_log_level_set("wifi",ESP_LOG_NONE);

    //Creat message queue
    wifi_app_queue_handle = xQueueCreate(3,sizeof(wifi_app_queue_message_t));

    //creat task for wifi application 
    xTaskCreatePinnedToCore(&wifi_app_task,"wifi application task",WIFI_APP_TASK_STACK_SIZE,NULL,WIFI_APP_TASK_PRIORITY,NULL,WIFI_APP_TASK_CORE_ID);
   
}

void wifi_app_stop(void){
    esp_wifi_stop();
}