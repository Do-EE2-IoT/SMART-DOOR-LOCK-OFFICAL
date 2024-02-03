#ifndef __WSHEADER_ME_H_
#define __WSHEADER_ME_H_

void ws2812_init(int txpin, int number_of_led);
void ws2812_set_color(int index,int r, int g, int b);
void ws2812_update_color(void);


#endif