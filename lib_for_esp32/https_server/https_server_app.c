#include <stdio.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "string.h"

#include "esp_log.h"
#include "esp_http_client.h"
#include <esp_event.h>
#include <esp_system.h>



// Define client certificate
const uint8_t crt_pem_start[] asm("_binary_crt_pem_start");
const uint8_t crt_pem_end[]   asm("_binary_crt_pem_end");


esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("Client HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}

void client_post_rest_function()
{
    esp_err_t ret;
    esp_http_client_config_t config_post = {
        .url = "https://do30032003.pythonanywhere.com/post_here/",
        .method = HTTP_METHOD_POST,
        .cert_pem = (const char *)crt_pem_start,
        .event_handler = client_event_get_handler};
        
    esp_http_client_handle_t client = esp_http_client_init(&config_post);

    char  *post_data = "{\"content1\":\"OPEN DOOR\"}";
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_http_client_set_header(client, "Content-Type", "application/json");
    do{
    ret = esp_http_client_perform(client);
    }while(ret != ESP_OK);
    esp_http_client_cleanup(client);
}

