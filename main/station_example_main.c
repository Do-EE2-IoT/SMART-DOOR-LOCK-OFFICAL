
/**
 * file main xử lý toàn bộ công việc 
 * Sử dụng FREERTOS để xử lý công việc
 * Sử dụng các task handle để kiểm soát sự hoạt động của các task
 * Đối với chạy đa luồng, Hàm Vtaskresume(task handle) để mở khóa task, Hàm VtaskSuspend(task handle)
 * dùng để khóa task.
 * Sử dụng Eventgroup để cho phép một task chạy dựa trên một điều kiện cấp phép cụ thể. 
 * Sử dụng Sermaphore để cho phép task hoạt động. 
 * Kiểm soát tài nguyên trong việc dùng task
 *  xTaskCreate(task_for_wifi_ap,"task for button interrupt",2048,NULL,7,&WIFI_button_task_handle);
    http_set_wifi_callback(callback);
    xTaskCreate(Task_B_number,"B_number_entered",4096,NULL,8,&B_number_task_handle);
    xTaskCreate(task_for_security, "security task", 4096,NULL,8,&security_task_handle);
    xTaskCreate(Task_accept_change_block_time,"task accept change block time",2048,NULL,8,&Accept_change_block_time_task_handle);
    xTaskCreate(Task_Unreal_password, "Unreal",8192,NULL,7,&Unreal_password_handle);
    xTaskCreate(Task_for_lcd_enter_password,"task_lcd",2048,NULL,7,&lcd_dislay_password_task_handle); 
    xTaskCreate(nfc_task, "nfc_task", 4096, NULL, 7, &RFID_card_task_handle);
    xTaskCreate(task_for_https_operation,"task_for_https",4096,NULL,6,NULL);
*/

#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/i2c.h"
#include "lcd.h"
#include "rfid_rc522.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "spi.h"
#include "keypad.h"
#include "stdlib.h"
#include "output_iot.h"
#include "wifi_app.h"
#include "input_iot_no_count.h"
#include "https_server_app.h"
#include "http_sever_app.h"
#include "esp_wifi.h"
#include "lwip/netdb.h"
#include "esp_wifi_types.h"



// http server application task
#define HTTP_SERVER_TASK_STACK_SIZE 8192
#define HTTP_SERVER_TASK_PRIORITY 4
#define HTTP_SERVER_TASK_CORE_ID 0

// http server monitor task
#define HTTP_SERVER_MONITOR_STACK_SIZE 4096
#define HTTP_SERVER_MONITOR_PRIORITY 3
#define HTTP_SERVER_MONITOR_CORE_ID 0
extern int station_setup;

static const char *TAG = "i2c-simple-example";
// Khởi tạo handle cho task thẻ từ 
TaskHandle_t RFID_card_task_handle;
void nfc_task(void *pvParameter); // Task thẻ từ

// Khởi tạo handle cho task hiển thị số lượng ký tự mật khẩu lên màn hình LCD
TaskHandle_t lcd_dislay_password_task_handle;
void Task_for_lcd_enter_password(void *parameter);// Task hiển thị số lượng mật khẩu lên màn hình LCD

//Khởi tạo handle cho task liên tục so sánh số ký tự được nhập vào với mật khẩu chuẩn
TaskHandle_t Unreal_password_handle;
void Task_Unreal_password(void *parameter); // task xử lý 

// Khởi tạo handle cho task chức năng nút B
TaskHandle_t B_number_task_handle;
void Task_B_number(void* parameter); // task xử lý chức năng nút B

// khởi tạo handle cho task chấp nhận thay đổi thời gian khóa 
TaskHandle_t Accept_change_block_time_task_handle;
void Task_accept_change_block_time(void * parameter); // task chấp nhận thay đổi thời gian khóa

// Khởi tạo handle cho task kích hoạt WIFI accesspoint
TaskHandle_t WIFI_button_task_handle;
void task_for_wifi_ap(void *parameter);// task xử lý wifi accesspoint

// Khởi tạo handle cho task security 
TaskHandle_t security_task_handle;
void task_for_security(void *parameter);


TaskHandle_t C_number_task_handle;
void Task_C_number(void *parameter);


TaskHandle_t accept_change_password_task_handle;
void Task_accept_change_password(void *parameter);

TaskHandle_t Open_door_task_handle;
void Task_for_Open_Door(void *parameter);

void task_for_https_operation(void *parameter);

EventGroupHandle_t Block_time; // Điều kiện để mở Task_accept_change_block_time
EventGroupHandle_t WIFI_AP;   // Điều kiện để mở task_for_wifi_ap

SemaphoreHandle_t security; // điều kiện mở task security


int block = 10;
int time_success_opendoor = 15;
#define Button_B_ENTERED 1<<0
#define ACCEPT_CHANGE_BY_ADMIN 1 << 1
#define WIFI_AP_EV  ( 1 << 2)
#define HTTPS_EVENT (1 << 5)
#define Button_C_ENTERED 1 << 6
#define ACCEPT_CHANGE_PW_BY_ADMIN 1 << 7
#define OPEN_DOOR 1 << 8


unsigned char CardID[5]; 
char key_admin[9] = "30032003";
char key[6] = "123456";
char Unreal_password[6];
char admin_password[9];
int B_button = 1;
int wifi_ap_accept = 0;
int i;
int wrong_pass_time = 1; // số lần nhập sai mật khẩu


// định nghĩa các pin để giao tiếp giữa esp32 với thẻ từ RFID thông qua giao thức SPI
#define PIN_NUM_MISO GPIO_NUM_19
#define PIN_NUM_MOSI GPIO_NUM_23
#define PIN_NUM_CLK  GPIO_NUM_18
#define PIN_NUM_CS   GPIO_NUM_5
#define PIN_NUM_RST  GPIO_NUM_27

/**
 * @brief i2c master initialization
 * Cấu hình cho I2C, chọn chân, tốc độ xung clock.
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_NUM_0;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_21,
        .scl_io_num = GPIO_NUM_22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}
// vang den xanh vang den tim
void callback(char *ssid, char *pass){
    printf("%s \n%s\n",ssid,pass);
    wifi_app_stop();
    wifi_sta_init();
    wifi_stop_sta();
    station_setup = 1;

}

void app_main(void)
{B
    // Khởi tạo flash cho esp32
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NOT_FOUND){
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    // http_set_wifi_callback(callback);
    security = xSemaphoreCreateBinary();

    // Khởi tạo I2C
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    // Khởi tạo keypad 4*4
    ionput_keypad_setup();
    output_set_level(GPIO_NUM_15,0);
    // Khởi tạo LCD
    lcd_init();

    // Khởi tạo điều kiện mở Task_accept_change_block_time
    // Khởi tạo điều kiện mở cho Task_for_wifi_ap
     Block_time =  xEventGroupCreate();
     WIFI_AP = xEventGroupCreate();
     ESP_ERROR_CHECK(esp_netif_init());
     ESP_ERROR_CHECK(esp_event_loop_create_default());
     wifi_app_event_handler_init();
     wifi_app_default_wifi_config();
     wifi_app_softap_config();
     output_set_level(GPIO_NUM_2,1);
    // Khởi tạo các task tương ứng, phân stack
    // Phân cấp độ ưu tiên, cấp độ ưu tiên càng lớn thì task được chạy càng nhiều.
    xTaskCreate(task_for_wifi_ap,"task for wifi access point",2048,NULL,7,&WIFI_button_task_handle);
    http_set_wifi_callback(callback);
    xTaskCreate(Task_B_number,"B_number_entered",4096,NULL,8,&B_number_task_handle);
    xTaskCreate(task_for_security, "security task", 4096,NULL,8,&security_task_handle);
    xTaskCreate(Task_accept_change_block_time,"task accept change block time",2048,NULL,8,&Accept_change_block_time_task_handle);
    xTaskCreate(Task_C_number,"task C number",4096,NULL,8,&C_number_task_handle);
    xTaskCreate(Task_accept_change_password, "task accept change password",4096,NULL,8,&accept_change_password_task_handle);
    xTaskCreate(Task_for_Open_Door, "task_for_open_door",8192, NULL,8, &Open_door_task_handle);
    xTaskCreate(Task_Unreal_password, "Unreal",2048,NULL,7,&Unreal_password_handle);
    xTaskCreate(Task_for_lcd_enter_password,"task_lcd",8192 ,NULL,7,&lcd_dislay_password_task_handle); 
    xTaskCreate(nfc_task, "nfc_task", 8192, NULL, 7, &RFID_card_task_handle);
    xTaskCreate(task_for_https_operation,"task_for_https",4096,NULL,6,NULL);
    
}

// các biến toàn cục
char get_character_1;
char get_character_2;
char get_character_3;
char get_character_4;
int the_position = 0;
char compare[20];
 char password[17];
 int condition = 0;

/**
 * @brief: Đây là task dùng để kiểm tra các nút chức năng , hiện ký tự mật khẩu lên màn hình LCD
 * @param: NULL
 * @other: Các biến character_1,2,3,4 đều là kiểu ký tự, liên tục được kiểm tra 
 * để nhận các ký tự. 
 * get_character_1 : Nhận ký tự hàng 1
 * get_character_2: Nhận ký tự hàng 2
 * ....tương tự
 * @other: Một số hàm của LCD
 * @function: lcd_clear() : Sử dụng để xóa toàn màn hình LCD
 * @function: lcd_put_cur(x,y): Đặt kí tự tại một vị trí bất kỳ trong LCD
 * trong đó x = {0,1} với 0 là hàng 1, 1 là hàng 2, y = 0:16 là vị trí cột
 * @function: lcd_send_data(data) : Gửi một ký tự lên màn hình 
 * @function: lcd_send_string(string): Gửi một chuỗi ký tự lên màn hình
 * @parameter: the_position : là biến thể hiện vị trí ở cột hiện tại
 * @array: Mảng password[17] dùng để lưu các ký tự đã được nhập trên màn hình
*/
void Task_for_lcd_enter_password(void *parameter){
    int i,j;
      while(1){
        vTaskSuspend(RFID_card_task_handle);
        // check xem có nút nào được ấn không 
        get_character_1 = check_key_pad_col_1();
        get_character_2 = check_key_pad_col_2();
        get_character_3 = check_key_pad_col_3();
        get_character_4 = check_key_pad_col_4();
        // Kết thúc check
        
        // Nếu nút được ấn thuộc hàng 1
        if(get_character_1 != NO_ENTER ){
            // Nếu nút được ấn khác nút A ( chỉ nhận số 1 2 3)
            if(get_character_1 != 'A' ){
                // Tiến hành đếm ngược 10 giây - kể từ thời điểm ký tự đầu tiên được nhập 
                // nếu sau 10 giây không ấn enter, tiến hành reset toàn bộ mật khẩu đã nhập
                xSemaphoreGive(security); // Chi tiết tại task_for_security
                // Mảng password lưu ký tự
                password[the_position] = get_character_1; 
                // Đặt lên hàng 2 của màn hình
                lcd_put_cur(1,the_position);
                lcd_send_data('*');
                // vị trí được cộng 1
                the_position++;
        }
        }

         if(get_character_2 != NO_ENTER ){
            if( get_character_2 != 'B'){
         // Tiến hành đếm ngược 10 giây - kể từ thời điểm ký tự đầu tiên được nhập
        xSemaphoreGive(security); 
        password[the_position] = get_character_2;
        lcd_put_cur(1,the_position);
        lcd_send_data('*');
        the_position++;
        }
         }

         if(get_character_3 != NO_ENTER ){
            if(get_character_3 != 'C'){
        // Tiến hành đếm ngược 10 giây- kể từ thời điểm ký tự đầu tiên được nhập
        xSemaphoreGive(security); 
        password[the_position] = get_character_3;
        lcd_put_cur(1,the_position);
        lcd_send_data('*');
        the_position++;
         }
        }

         if(get_character_4 != NO_ENTER ){
            if(get_character_4 != '*' && get_character_4 != '#' && get_character_4 != 'D'){
        // Tiến hành đếm ngược 10 giây- kể từ thời điểm ký tự đầu tiên được nhập
        xSemaphoreGive(security); 
        password[the_position] = get_character_4;
        lcd_put_cur(1,the_position);
        lcd_send_data('*');
        the_position++;
        }
         }
        
        // Nút bấm D được dùng để xóa một ký tự * trên màn hình, xóa một phần tử lớn nhất trong 
        //mảng password
        if( get_character_4 == 'D'){ 
        // Chỉ có tác dụng nếu vị trí hiện tại > 0
        if(the_position > 0){
            the_position--; // trừ đi một vị trí
        }
            // cho ký tự lớn nhất bằng rỗng
            password[the_position] = '\0';
            lcd_put_cur(1,the_position);
            lcd_send_data(' ');
            lcd_put_cur( 0, 0);
            lcd_send_string("-ENTER PASSWORD->");
        }
 
        /*Xóa toàn bộ ký tự trên màn hình 
         và Xóa toàn bộ mảng password hiện tại*/
        if(get_character_1 == 'A'){ 
          lcd_clear();
          lcd_put_cur(0,0);
          lcd_send_string("-ENTER PASSWORD-");
          int k = 0;
          for( k = 0; k < 17 ; k++){
            password[k] = '\0';
          }
          the_position = 0;
           for( i = 0 ; i < 7; i++){
               Unreal_password[i] = '\0';
            }
        }

        // Chức năng nút B
        /* Nút B được dùng để thay đổi thời gian khóa cửa nếu ấn sai quá nhiều 
                             Được thiết lập bởi admin
        */
        if(get_character_2 == 'B'){
          lcd_clear();
          lcd_put_cur(0,0);
          vTaskDelay(3000/portTICK_PERIOD_MS); // Thực hiện dịch toàn màn hình sang phải 20 lần, Mỗi lần 500ms
          // Thực hiện xóa toàn bộ password đã lưu, làm trống mảng Unreal_password
          int i = 0;
          for( i = 0 ; i < 17; i++){
            password[i] = '\0';
          }
          for( i = 0; i < 7; i++){
            Unreal_password[i] = '\0';
          }
            vTaskDelay(1000/portTICK_PERIOD_MS);  
            lcd_clear();
            lcd_put_cur(0,0);
            lcd_send_string("-ADMIN PASSWORD->");
            // Đặt con trỏ về vị trí ban đầu
            the_position  = 0;

            // Mở khóa task chức năng nút B ( Task_B_number)
            vTaskResume(B_number_task_handle);
            // Set bit BUTTON_B_ENTERRED ( -> Đi tới Task_B_number )
            xEventGroupSetBits(Block_time, Button_B_ENTERED);  // nhả bit Button_B_ENTERED
            vTaskDelay(1000/portTICK_PERIOD_MS);
           }

    if(get_character_3 == 'C'){
        vTaskSuspend(Unreal_password_handle);
        lcd_clear();
        lcd_put_cur(0,0);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        vTaskResume(Unreal_password_handle);
         int i = 0;
          for( i = 0 ; i < 17; i++){
            password[i] = '\0';
          }
          for( i = 0; i < 7; i++){
            Unreal_password[i] = '\0';
          }
          lcd_clear();
        lcd_put_cur(0,0);
        lcd_send_string("-ADMIN PASSWORD-");
            // Đặt con trỏ về vị trí ban đầu
        the_position  = 0;

            // Mở khóa task chức năng nút B ( Task_B_number)
        vTaskResume(C_number_task_handle);
            // Set bit BUTTON_B_ENTERRED ( -> Đi tới Task_B_number )
        xEventGroupSetBits(Block_time, Button_C_ENTERED);  // nhả bit Button_B_ENTERED
        vTaskDelay(1000/portTICK_PERIOD_MS);
        

    }

     // Nếu người dùng đã nhập quá nhiều *       
     if(strlen(password) >= 16){
          lcd_put_cur(0,0);
          lcd_send_string("Enter '#' OR 'D' ");
            }

    // Nếu nút * được ấn 5 lần , kích hoạt wifi accesspoint trong 30 giây
    if(get_character_4 == '*'){
        wifi_ap_accept++;
        if(wifi_ap_accept == 6){
            vTaskResume(WIFI_button_task_handle);
            xEventGroupSetBits(WIFI_AP,WIFI_AP_EV);  // nhả bit WIFI_AP_EV, -> task_for_wifi_ap
        }
    }
    

    if(get_character_4 == '#'){                                      
        xEventGroupSetBits(Block_time,OPEN_DOOR);
        }

    vTaskResume(RFID_card_task_handle);
    
      }
}

void Task_for_Open_Door(void *parameter){
    int i,j;
    while(1){
        EventBits_t event_open_door = xEventGroupWaitBits(Block_time,OPEN_DOOR,pdTRUE,pdFALSE,portMAX_DELAY);
        if(event_open_door & OPEN_DOOR){
           if(strlen(Unreal_password) == 6 ) { 
            vTaskSuspend(Unreal_password_handle);
            vTaskSuspend(lcd_dislay_password_task_handle);
            vTaskSuspend(RFID_card_task_handle);
            // if - Nếu độ dài của mảng Unreal_password bằng 6 nghĩa là 6 ký tự liên tiếp giống mật khẩu                                
            lcd_clear();
            wrong_pass_time = 1; // Khi này đã nhập đúng mật khẩu, số lần sai được đặt bằng 1
            output_set_level(GPIO_NUM_2, 0);  // Mở cửa 
            if(station_setup == 1){
            xEventGroupSetBits(WIFI_AP,HTTPS_EVENT);
            }
            
            lcd_clear();
            lcd_put_cur(0,1);
            lcd_send_string("Open door");
            lcd_put_cur(1,1);
            lcd_send_string("Successfully");
            vTaskDelay(5000/portTICK_PERIOD_MS);
            lcd_clear();
            lcd_put_cur(0,0);
            int second = 0;
            char count_second[20];
            lcd_send_string("Need lock door");
            // Thực hiện hiển thị lên màn hình đếm lùi x giây
                for(second = time_success_opendoor ; second > 0; second --){
                    sprintf(count_second,"After %d s",second);
                    output_toggle_pin(GPIO_NUM_15);
                    lcd_put_cur(1,2);
                    lcd_send_string(count_second);
                    vTaskDelay(1000/portTICK_PERIOD_MS);
                 }
            output_set_level(GPIO_NUM_2, 1);
            output_set_level(GPIO_NUM_15,0); // nhả khóa để có thể đóng cửa
            
            if(station_setup == 1){
            wifi_stop_sta();
            }
            // Tiến hành reset toàn bộ
                for( i = 0 ; i < 17 ; i++){
                    password[i] = '\0'; 
                }
                for( j = 0 ; j < 7; j++){
                 Unreal_password[j] = '\0';
                }
            the_position =  0;
            lcd_clear();
            lcd_put_cur(0,0);
            lcd_send_string("-ENTER PASSWORD-");
            lcd_put_cur(1,0);
            vTaskResume(Unreal_password_handle);
            vTaskResume(lcd_dislay_password_task_handle);   
            vTaskResume(RFID_card_task_handle);
            }
            else{
                // Nếu khi ấn enter nhưng số ký tự liên tiếp đúng mật khẩu không bằng 6
                if(strlen(password) < 16 && strcmp(Unreal_password,key) != 0){
                if(wrong_pass_time != 5){ // Nếu chưa sai 5 lần 
                vTaskSuspend(Unreal_password_handle);
                vTaskSuspend(lcd_dislay_password_task_handle);
                vTaskSuspend(RFID_card_task_handle);
                char wrong_pass_string[20];
                // Hiển thị số lần sai
                sprintf(wrong_pass_string,"Wrong %d/5", wrong_pass_time);
                output_set_level(GPIO_NUM_15,1);
                lcd_clear();
                lcd_put_cur(0,2);
                lcd_send_string(wrong_pass_string);
                vTaskDelay(2000/portTICK_PERIOD_MS);
                wrong_pass_time++; // Cộng 1 lần ,mở khóa sai trên 5 lần
                output_set_level(GPIO_NUM_15,0);
                the_position = 0;
                lcd_clear();
                lcd_put_cur(0,0);
                lcd_send_string("-ENTER PASSWORD-");
                 for( i = 0 ; i < 17 ; i++){
                    password[i] = '\0'; 
                                         }
                    for( i = 0 ; i < 7; i++){
                         Unreal_password[i] = '\0';
                    }
                vTaskResume(Unreal_password_handle);
                vTaskResume(lcd_dislay_password_task_handle);
                vTaskResume(RFID_card_task_handle);
                }
                else{
                    vTaskSuspend(RFID_card_task_handle);
                    vTaskSuspend(lcd_dislay_password_task_handle);
                    vTaskSuspend(Unreal_password_handle);
                    // Nếu đã sai 5 lần 
                    wrong_pass_time = 1;
                    lcd_clear();
                    lcd_put_cur(0,0);
                    lcd_send_string("wrong 5 times");
                    vTaskDelay(5000/portTICK_PERIOD_MS);
                    char block_door[25];
                     lcd_clear();
                     // Biến block là số giây dừng hoạt động của khóa cửa 
                    int temporary = block;
                    while(temporary > 0){
                        lcd_put_cur(0,0);
                        lcd_send_string("Stop Operate");
                        output_toggle_pin(GPIO_NUM_15);
                        sprintf(block_door,"in %d second",temporary);
                        lcd_put_cur(1,2);
                        lcd_send_string(block_door);
                        
                        vTaskDelay(1000/portTICK_PERIOD_MS);
                        temporary--;

                    }
                    output_set_level(GPIO_NUM_15,0);
                    // Thực hiện reset toàn bộ
                    
                    lcd_clear();
                    the_position = 0;
                    lcd_put_cur(0,0);
                    lcd_send_string("-ENTER PASSWORD-");
                     for( i = 0 ; i < 17 ; i++){
                    password[i] = '\0'; 
                            }
                    for( i = 0 ; i < 7; i++){
                    Unreal_password[i] = '\0';
                        }  
                    vTaskResume(Unreal_password_handle);
                    vTaskResume(lcd_dislay_password_task_handle);
                    vTaskResume(RFID_card_task_handle);        
                }
            }
            }
        }
    }
}

/**
 * @xsemaphoretake: Là macro dùng để lấy semaphore ngay khi semaphore xuất hiện 
 * @brief: Nếu kể từ thời điểm ký tự đầu tiên được nhập, sau 10 giây chưa nhập xong, 
 * sẽ tiến hành reset toàn bộ mật khẩu 
*/
void task_for_security(void *parameter){
    int k = 0;
    while(1){
        if(xSemaphoreTake(security, portMAX_DELAY) == pdTRUE){
                if(wrong_pass_time != 5){
                    for( k = 0; k < 10 ; k++){
                         vTaskDelay(1000/portTICK_PERIOD_MS);
                       }
                lcd_clear();
                lcd_put_cur(0,0);
                lcd_send_string("-ENTER PASSWORD-");
                for( k = 0; k < 17 ; k++){
                    password[k] = '\0';
                   }
                for(k = 0; k < 7; k++){
                   Unreal_password[k] = '\0';
                }
              the_position = 0;
        }
    }
}
}

/**
 * @brief: Đây là task chạy song song với Task_for_lcd_enter_password để liên tục so sánh 
 * số ký tự được nhập vào với mật khẩu thật
 * @paramter: password[17] như ở trên, là mảng lưu số ký tự được nhập vào
 * @param: key[6] = "123456" đây là mật khẩu của cửa, có thể thiết lập bởi admin
 * @param : Unreal_password[6] : là mảng lưu giá trị từ thời điểm một ký tự đầu tiên 
 * của password bằng key. 
 * @param: wrong_pass_time: Số lần nhập mật khẩu sai
*/
void Task_Unreal_password(void *parameter){
      int i,j;
      lcd_clear();
      lcd_put_cur(0,0);
      lcd_send_string("-ENTER PASSWORD-");
     while(1){
        vTaskSuspend(RFID_card_task_handle);
        // i chạy tới 11 do chỉ có 16 cột, chỉ lấy 6 ký tự mật khẩu
        for( i = 0 ; i < 11; i++){
            if(password[i] == key[0]){
                Unreal_password[0] = password[i]; // 1 ký tự liên tiếp giống mật khẩu
                  if(password[i + 1] == key[1]){
                     Unreal_password[1] = password[i + 1]; // 2 ký tự liên tiếp giống mật khẩu
                        if(password[i + 2] == key[2]){
                            Unreal_password[2] = password[i + 2]; // 3 ký tự liên tiếp giống mật khẩu
                               if(password[i + 3] == key[3]){
                                   Unreal_password[3] = password[i + 3]; // 4 ký tự liên tiếp giống mật khẩu
                                      if(password[i + 4] == key[4]){
                                           Unreal_password[4] = password[i + 4];  // 5 ký tự liên tiếp giống mật khẩu
                                          if(password[i + 5] == key[5]){
                                             Unreal_password[5] = password[i + 5];  // 6 ký tự liên tiếp giống mật khẩu
                                          }
                                      }
                               }
                        }
                  }
            }
        }

         
        vTaskResume(RFID_card_task_handle);
  
    // Đây là nút ấn enter
     }
}

        
/**
 * @brief : Chức năng nút B , Khi nút B được bấm, cấm task hiển thị mật khẩu, thẻ từ, wifi ap 
 * để tránh gây xung đột 
 * @param: event là biến được tạo thực hiện lấy bit button_B_ENTERED được nhả ở dòng 302 nếu được bấm
*/

void Task_B_number(void* parameter){
   while(1){
    EventBits_t event = xEventGroupWaitBits(Block_time,Button_B_ENTERED,pdFALSE,pdFALSE,portMAX_DELAY);
    if(event & Button_B_ENTERED){ 
    vTaskSuspend(Unreal_password_handle);
    vTaskSuspend(lcd_dislay_password_task_handle);
    vTaskSuspend(RFID_card_task_handle);
    vTaskSuspend(WIFI_button_task_handle);
    lcd_put_cur(1,0);
    lcd_send_string("");
    
    // Phần này tương tự ở trên khi mà các biến get liên tục check xem nút nào được bấm không
    get_character_1 = check_key_pad_col_1();
    get_character_2 = check_key_pad_col_2();
    get_character_3 = check_key_pad_col_3();
    get_character_4 = check_key_pad_col_4();
    
    if(get_character_1 != NO_ENTER ){
            if(get_character_1 != 'A' ){
        admin_password[the_position] = get_character_1;
        lcd_put_cur(1,the_position);
        lcd_send_data('*');
        the_position++;
        }
        }
    if(get_character_2 != NO_ENTER ){
            if(get_character_1 != 'B' ){
        admin_password[the_position] = get_character_2;
        lcd_put_cur(1,the_position);
        lcd_send_data('*');
        the_position++;
        }
        }

     if(get_character_3 != NO_ENTER ){
            if(get_character_1 != 'C' ){
        admin_password[the_position] = get_character_3;
        lcd_put_cur(1,the_position);
        lcd_send_data('*');
        the_position++;
        }
        }

     if(get_character_4 != NO_ENTER ){
            if(get_character_4 != 'D'  && get_character_4 != '*' && get_character_4 != '#' ){
        admin_password[the_position] = get_character_4;
        lcd_put_cur(1,the_position);
        lcd_send_data('*');
        the_position++;
        }
        }

     if( get_character_4 == 'D'){ // delete a character
        if(the_position > 0){
            the_position--;
        }
            admin_password[the_position] = '\0';
            lcd_put_cur(1,the_position);
            lcd_send_data(' ');
            lcd_put_cur(0, 0);
            lcd_send_string("-ADMIN PASSWORD->");
        }
         if(get_character_1 == 'A'){ // assistant
          lcd_clear();
          lcd_put_cur(0,0);
          lcd_send_string("-ADMIN PASSWORD->");
          int k = 0;
          for( k = 0; k < 17 ; k++){
            password[k] = '\0';
          }
          the_position = 0;
        }

        // Sau khi đã nhập các ký tự mật khẩu admin, thực hiện ấn nút *- enter -
        // Mật khẩu admin chỉ được quyền nhập 1 lần, nếu nhập sai sẽ gây khóa cửa 1 phút
        if(get_character_4 == '*'){
        if(strcmp(admin_password, key_admin) == 0){
        // Nếu mật khẩu admin đúng     
           lcd_clear();
           lcd_put_cur(0,0);
           lcd_send_string("ACCEPT CHANGE BLOCK TIME ");
           
           vTaskDelay(3000/portTICK_PERIOD_MS);
           lcd_clear();
           
            the_position = 0;
           xEventGroupSetBits(Block_time,ACCEPT_CHANGE_BY_ADMIN); // nhả bit ACCEPT_CHANGE_BY_ADMIN
           xEventGroupClearBits(Block_time,Button_B_ENTERED);
        }
        else{
        lcd_clear();
        lcd_put_cur(0,0);
        lcd_send_string("BLOCK DOOR 1 minutes");
        lcd_go_right(10);
        vTaskDelay(10000/portTICK_PERIOD_MS);
        lcd_clear();
        lcd_put_cur(0,0);
        lcd_send_string("-ENTER PASSWORD-");
        the_position = 0;
        vTaskResume(lcd_dislay_password_task_handle);
        vTaskResume(RFID_card_task_handle);
        vTaskResume(WIFI_button_task_handle);
        vTaskResume(Unreal_password_handle);
        xEventGroupClearBits(Block_time,Button_B_ENTERED);
    }  
    }     
   }
}
}

void Task_C_number(void *parameter){
while(1){
    EventBits_t event_bit = xEventGroupWaitBits(Block_time,Button_C_ENTERED,pdFALSE,pdFALSE,portMAX_DELAY);
    if(event_bit &Button_C_ENTERED){
        vTaskSuspend(lcd_dislay_password_task_handle);
        vTaskSuspend(security_task_handle);
        vTaskSuspend(RFID_card_task_handle);
        vTaskSuspend(Unreal_password_handle);

    
    // Phần này tương tự ở trên khi mà các biến get liên tục check xem nút nào được bấm không
    get_character_1 = check_key_pad_col_1();
    get_character_2 = check_key_pad_col_2();
    get_character_3 = check_key_pad_col_3();
    get_character_4 = check_key_pad_col_4();
    
    if(get_character_1 != NO_ENTER ){
            if(get_character_1 != 'A' ){
        admin_password[the_position] = get_character_1;
        lcd_put_cur(1,the_position);
        lcd_send_data('*');
        the_position++;
        }
        }
    if(get_character_2 != NO_ENTER ){
            if(get_character_2 != 'B' ){
        admin_password[the_position] = get_character_2;
        lcd_put_cur(1,the_position);
        lcd_send_data('*');
        the_position++;
        }
        }

     if(get_character_3 != NO_ENTER ){
            if(get_character_3 != 'C' ){
        admin_password[the_position] = get_character_3;
        lcd_put_cur(1,the_position);
        lcd_send_data('*');
        the_position++;
        }
        }

     if(get_character_4 != NO_ENTER ){
            if(get_character_4 != 'D'  && get_character_4 != '*' && get_character_4 != '#' ){
        admin_password[the_position] = get_character_4;
        lcd_put_cur(1,the_position);
        lcd_send_data('*');
        the_position++;
        }
        }

     if( get_character_4 == 'D'){ // delete a character
        if(the_position > 0){
            the_position--;
        }
            admin_password[the_position] = '\0';
            lcd_put_cur(1,the_position);
            lcd_send_data(' ');
            lcd_put_cur(0, 0);
            lcd_send_string("-ADMIN PASSWORD->");
        }
         if(get_character_1 == 'A'){ // assistant
          lcd_clear();
          lcd_put_cur(0,0);
          lcd_send_string("-ADMIN PASSWORD->");
          int k = 0;
          for( k = 0; k < 17 ; k++){
            password[k] = '\0';
          }
          for( k = 0 ; k < 6; k++){
            admin_password[k] = '\0';
          }
          the_position = 0;
        }

        if(get_character_4 == '*'){
        if(strcmp(admin_password, key_admin) == 0){
        // Nếu mật khẩu admin đúng     
           lcd_clear();
           lcd_put_cur(0,0);
           lcd_send_string("ACCEPT CHANGE PASSWORD ");
           
           vTaskDelay(1000/portTICK_PERIOD_MS);
           lcd_go_right(12);
           lcd_clear();
           
            the_position = 0;
           xEventGroupSetBits(Block_time,ACCEPT_CHANGE_PW_BY_ADMIN); // nhả bit ACCEPT_CHANGE_BY_ADMIN
           xEventGroupClearBits(Block_time,Button_C_ENTERED);
        }
        else{
        lcd_clear();
        lcd_put_cur(0,0);
        lcd_send_string("BLOCK DOOR 1 minutes");
        lcd_go_right(10);
        vTaskDelay(10000/portTICK_PERIOD_MS);
        lcd_clear();
        lcd_put_cur(0,0);
        lcd_send_string("-ENTER PASSWORD-");
        the_position = 0;
        vTaskResume(lcd_dislay_password_task_handle);
        vTaskResume(RFID_card_task_handle);
        vTaskResume(Unreal_password_handle);
        vTaskResume(security_task_handle);
        xEventGroupClearBits(Block_time,Button_C_ENTERED);
    }  
    }     

    }
}
}

void Task_accept_change_password(void *parameter){
    char save_password_change[6];
     while(1){
        EventBits_t bit_pw = xEventGroupWaitBits(Block_time,ACCEPT_CHANGE_PW_BY_ADMIN,pdFALSE,pdFALSE,portMAX_DELAY);
        if(bit_pw & ACCEPT_CHANGE_PW_BY_ADMIN){
            vTaskSuspend(C_number_task_handle);
            lcd_put_cur(0,0);
            lcd_send_string("Only six number");
            get_character_1 = check_key_pad_col_1();
            get_character_2 = check_key_pad_col_2();
            get_character_3 = check_key_pad_col_3();
            get_character_4 = check_key_pad_col_4();
    
    if(get_character_1 != NO_ENTER ){
            if(get_character_1 != 'A' ){
        save_password_change[the_position] = get_character_1;
        lcd_put_cur(1,the_position);
        lcd_send_data(get_character_1);
        the_position++;
        }
        }
    if(get_character_2 != NO_ENTER ){
            if(get_character_2 != 'B' ){
        save_password_change[the_position] = get_character_2;
        lcd_put_cur(1,the_position);
        lcd_send_data(get_character_2);
        the_position++;
        }
        }

     if(get_character_3 != NO_ENTER ){
            if(get_character_3 != 'C' ){
        save_password_change[the_position] = get_character_3;
        lcd_put_cur(1,the_position);
        lcd_send_data(get_character_3);
        the_position++;
        }
        }

     if(get_character_4 != NO_ENTER ){
            if(get_character_4 != 'D'  && get_character_4 != '*' && get_character_4 != '#' ){
        save_password_change[the_position] = get_character_4;
        lcd_put_cur(1,the_position);
        lcd_send_data(get_character_4);
        the_position++;
        }
        }

     if( get_character_4 == 'D'){ // delete a character
        if(the_position > 0){
            the_position--;
        }
            save_password_change[the_position] = '\0';
            lcd_put_cur(1,the_position);
            lcd_send_data(' ');
            lcd_put_cur( 0, 0);
            lcd_send_string("Only six number");
  }

  // chỉ nhận hai số, nếu nhập 3 số không thể ấn * [enter]
    if(the_position == 6){
    if(get_character_4 == '*'){
        wifi_ap_accept = 0;
        lcd_clear();
        lcd_put_cur(0,0);
        lcd_send_string("Changed");
        vTaskDelay(3000/portTICK_PERIOD_MS);
        strcpy(key,save_password_change);
        // như đề cập ở trên, block là thời gian cửa dừng hoạt động nếu bấm sai 5 lần
        the_position = 0;
        xEventGroupClearBits(Block_time,ACCEPT_CHANGE_PW_BY_ADMIN); // xóa bit
   // chạy lại các task
   vTaskResume(C_number_task_handle);
   vTaskResume(Unreal_password_handle);
   vTaskResume(lcd_dislay_password_task_handle);
   lcd_clear();
   lcd_put_cur(0,0);
   lcd_send_string("-ENTER PASSWORD-");
   vTaskResume(RFID_card_task_handle);
    }

        }
     }
}
}

/**
 * @brief : task chấp nhận thay đổi thời gian dừng hoạt động cửa khi đã nhập
 * đúng mật khẩu admin
 * @param: save_block_time: là mảng chứa các ký tự thời gian do admin thiết lập.
 * Dù khi bấm nút save_block_time vẫn luôn nhận các con số nhưng 
 * biến block ( thời gian cửa dừng hoạt động ) chỉ dược lưu nếu độ dài save_block_time 
 * là 2
*/
void Task_accept_change_block_time(void * parameter){
  char save_block_time[2];
           
  while(1){
    // lấy bit ACCEPT_CHANGE_BY_ADMIN thuộc dòng 579
    EventBits_t accept_change = xEventGroupWaitBits(Block_time,ACCEPT_CHANGE_BY_ADMIN,pdFALSE, pdFALSE,portMAX_DELAY);
    if(accept_change & ACCEPT_CHANGE_BY_ADMIN){
        // cấm các task khác tránh xung đột
           vTaskSuspend(B_number_task_handle);
           vTaskSuspend(WIFI_button_task_handle);
   
    lcd_put_cur(0,0);
    lcd_send_string("Only two number");
    
    // tương tự các phần trên, số cần nhập nằm trong khoảng từ ô 00 - 99
    get_character_1 = check_key_pad_col_1();
    get_character_2 = check_key_pad_col_2();
    get_character_3 = check_key_pad_col_3();
    get_character_4 = check_key_pad_col_4();
    
    if(get_character_1 != NO_ENTER ){
            if(get_character_1 != 'A' ){
        save_block_time[the_position] = get_character_1;
        lcd_put_cur(1,the_position);
        lcd_send_data(get_character_1);
        the_position++;
        }
        }
    if(get_character_2 != NO_ENTER ){
            if(get_character_2 != 'B' ){
        save_block_time[the_position] = get_character_2;
        lcd_put_cur(1,the_position);
        lcd_send_data(get_character_2);
        the_position++;
        }
        }

     if(get_character_3 != NO_ENTER ){
            if(get_character_3 != 'C' ){
        save_block_time[the_position] = get_character_3;
        lcd_put_cur(1,the_position);
        lcd_send_data(get_character_3);
        the_position++;
        }
        }

     if(get_character_4 != NO_ENTER ){
            if(get_character_4 != 'D'  && get_character_4 != '*' && get_character_4 != '#' ){
        save_block_time[the_position] = get_character_4;
        lcd_put_cur(1,the_position);
        lcd_send_data(get_character_4);
        the_position++;
        }
        }

     if( get_character_4 == 'D'){ // delete a character
        if(the_position > 0){
            the_position--;
        }
            save_block_time[the_position] = '\0';
            lcd_put_cur(1,the_position);
            lcd_send_data(' ');
            lcd_put_cur( 0, 0);
            lcd_send_string("Only two number");
  }
  
    // chỉ nhận hai số, nếu nhập 3 số không thể ấn * [enter]
    if(the_position == 2){
    if(get_character_4 == '*'){
        wifi_ap_accept = 0;
        lcd_clear();
        lcd_put_cur(0,0);
        lcd_send_string("Changed");
        vTaskDelay(3000/portTICK_PERIOD_MS);
        
        // như đề cập ở trên, block là thời gian cửa dừng hoạt động nếu bấm sai 5 lần
        block = atoi(save_block_time); // lưu giá trị mới 
        the_position = 0;
        xEventGroupClearBits(Block_time,ACCEPT_CHANGE_BY_ADMIN|Button_B_ENTERED); // xóa bit
   // chạy lại các task
   vTaskResume(B_number_task_handle);
   vTaskResume(Unreal_password_handle);
   vTaskResume(lcd_dislay_password_task_handle);
   lcd_clear();
   lcd_put_cur(0,0);
   lcd_send_string("-ENTER PASSWORD-");
   vTaskResume(RFID_card_task_handle);
   vTaskResume(WIFI_button_task_handle);
    }
  else{
  }
    }
}
}
}

// polling -> while(1){



/**
 * @brief Task kích hoạt chế độ wifi access point trong 60 giây
 * @param wifi_access_time[20] là mảng ký tự hiện lên màn hình thể hiện số thời gian còn lại mở wifi accesspoint
*/
void task_for_wifi_ap(void *parameter){
    int z;
    char wifi_access_time[20];
    while(1){
    // Chờ bit WIFI_AP_EV
    EventBits_t button_bit = xEventGroupWaitBits(WIFI_AP,WIFI_AP_EV,pdTRUE,pdFALSE,portMAX_DELAY);
    // nhả bit WIFI_AP_EV tại dòng 317
    if(button_bit & WIFI_AP_EV){
        // đây là khi đã ấn 5 lần nút '*'
        // Thực hiện cấm các task khác tránh xung đột
        vTaskSuspend(B_number_task_handle);
        vTaskSuspend(Unreal_password_handle);
        vTaskSuspend(Accept_change_block_time_task_handle);
        vTaskSuspend(RFID_card_task_handle);
        vTaskSuspend(lcd_dislay_password_task_handle);
        ESP_LOGI("WIFI_AP","ACCESSING WIFI AP");
        // bật wifi accesspoint
        wifi_app_start();
        lcd_clear();
        lcd_put_cur(0,0);
        lcd_send_string("ACCESSING WIFI");
        lcd_clear();
        lcd_put_cur(0,0);
        lcd_send_string("CANCEL AFTER");
        // Đếm ngược 60 giây, hiện lên màn hình LCD
        for(z = 60; z > -1; z--){
            lcd_put_cur(1,4);
           sprintf(wifi_access_time,"%d second",z);
           lcd_send_string(wifi_access_time);
           vTaskDelay(1000/portTICK_PERIOD_MS);
        }
        // Dừng chế độ wifi_accesspoint
        wifi_app_stop();
        lcd_clear();
        lcd_put_cur(0,0);
        lcd_send_string("-ENTER PASSWORD-");
        the_position = 0;
        // Mở khóa các task
         vTaskResume(lcd_dislay_password_task_handle);
         vTaskResume(Unreal_password_handle);
        vTaskResume(RFID_card_task_handle);
        vTaskResume(B_number_task_handle);
        vTaskResume(Accept_change_block_time_task_handle);
        wifi_ap_accept = 1;
    }
    }
}
/**
 * @brief task để kiểm tra thông tin thẻ từ 
*/
void nfc_task(void *pvParameter)
{
  spi_init(PIN_NUM_CLK, PIN_NUM_MOSI, PIN_NUM_MISO);  // Init Driver SPI
    MFRC522_Init(PIN_NUM_RST, PIN_NUM_CS); // Init MFRC522
    while (1)
    {
        vTaskSuspend(lcd_dislay_password_task_handle);
        vTaskDelay(50/portTICK_PERIOD_MS);
             if (MFRC522_Check(CardID) == MI_OK){
                vTaskSuspend(Unreal_password_handle);
    
            if(CardID[0] == 0x80 && CardID[1] == 0x45 && CardID[2] == 0x74 && CardID[3] == 0x11 && CardID[4] ==0xa0 ){
               // Nếu so sánh thấy thẻ từ đúng 
                output_set_level(GPIO_NUM_2,0); // Mở cửa 
                lcd_clear();
                lcd_put_cur(0,0);
                lcd_send_string("Opened by card");
                vTaskDelay(2000/portTICK_PERIOD_MS);
                lcd_clear();
                lcd_put_cur(0,0);
                int second = 0;
                char count_second[20];
                lcd_send_string("Need lock door");
                for(second = time_success_opendoor ; second > 0; second --){
                    output_toggle_pin(GPIO_NUM_15);
                sprintf(count_second,"After %d s",second);
                lcd_put_cur(1,2);
                lcd_send_string(count_second);
                
                vTaskDelay(1000/portTICK_PERIOD_MS);
                }
                output_set_level(GPIO_NUM_15,0);
                
                lcd_clear();
                lcd_put_cur(0,0);
                lcd_send_string("-ENTER PASSWORD-");
                output_set_level(GPIO_NUM_2,1); // Đặt lại chế độ có thể khóa cửa
                int i;
            for( i = 0 ; i < 7; i++){
               password[i] = '\0';
            }
            the_position = 0;
            if(station_setup == 1){
            wifi_stop_sta();
            }
            }
             else{
                lcd_clear();
                lcd_put_cur(0,0);
                lcd_send_string("Not available");
                output_set_level(GPIO_NUM_15,1);
                vTaskDelay(2000/portTICK_PERIOD_MS);
                output_set_level(GPIO_NUM_15,0);
                lcd_clear();
                lcd_put_cur(0,0);
                lcd_send_string("-ENTER PASSWORD-");
                int i;
            for( i = 0 ; i < 7; i++){
               password[i] = '\0';
            }
            the_position = 0;
            }
             vTaskDelay(1000/portTICK_PERIOD_MS);
             vTaskResume(Unreal_password_handle);
            }
            
            vTaskResume(lcd_dislay_password_task_handle);
    }
}

void task_for_https_operation(void *parameter){
      int second;
      while(1){
        EventBits_t https_bit = xEventGroupWaitBits(WIFI_AP,HTTPS_EVENT,pdTRUE,pdFALSE,portMAX_DELAY);
        if(https_bit & HTTPS_EVENT){
                wifi_start_sta();
                client_post_rest_function();
        for(second = 0; second < 15; second++){
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }
        wifi_stop_sta();
        }
      }
}
// ----------------------------------------------------END-------------------------------------




//MFRC: [80-45-74-11-a0]
// can thuc su chu y cac doan co con tro. That ky la khi ma no khong notify bat ky loi gi, luon can than. 






