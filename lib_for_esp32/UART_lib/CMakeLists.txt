set(pri_req nvs_flash)

idf_component_register(SRCS "uart_events_example_main.c"
                    INCLUDE_DIRS "."
                    REQUIRES ${pri_req})