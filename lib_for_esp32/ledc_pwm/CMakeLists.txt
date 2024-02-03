set(pri_req nvs_flash)

idf_component_register(SRCS "led_pwm.c"
                    INCLUDE_DIRS "."
                    REQUIRES ${pri_req})