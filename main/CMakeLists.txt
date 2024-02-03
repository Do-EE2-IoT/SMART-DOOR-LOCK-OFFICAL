set(pri_req nvs_flash)
idf_component_register(SRCS "station_example_main.c" 
                    INCLUDE_DIRS "."
                    REQUIRES ${pre_req})