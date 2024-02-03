#ifndef __UART_H_
#define __UART_H_

typedef int (*uart_call_back_t) (char*, int);
void uart_init(void);
void uart_set_call_back(void *cb);
#endif