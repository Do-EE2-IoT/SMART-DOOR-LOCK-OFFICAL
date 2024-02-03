#ifndef __INPUT_IOT_COUNT_H_
#define __INPUT_IOT__COUNT_H_

#include "esp_err.h"
#include "hal/gpio_types.h"
#include "driver/uart.h"



typedef void (*input_callback_count_t) (int,uint64_t);



void input_io_create(gpio_num_t gpio_num, int type);
void input_set_callback_count(void *cb);


#endif