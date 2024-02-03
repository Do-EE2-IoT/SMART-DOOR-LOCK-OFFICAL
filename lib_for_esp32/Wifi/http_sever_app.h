#ifndef __HTTP_SEVER_APP_H_
#define __HTTP_SEVER_APP_H_

#include "esp_err.h"
#include "esp_http_server.h"
#include "stdint.h"
void start_webserver(void);
void stop_webserver(void);


typedef void (*http_post_wifi_callback_t) (char *, char *);
void http_set_wifi_callback(void *cb);
void wifi_sta_init(void);
void wifi_stop_sta(void);
void wifi_start_sta(void);

void http_set_wifi_callback(void *cb);



#endif