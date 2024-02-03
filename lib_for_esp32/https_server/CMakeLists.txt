set(pri_req esp_http_client)
idf_component_register(SRCS "https_server_app.c"
                    INCLUDE_DIRS "."
                    EMBED_TXTFILES "crt.pem"
                    REQUIRES ${pri_req})
