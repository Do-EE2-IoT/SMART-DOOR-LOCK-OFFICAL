#include "driver/gpio.h"

#include "input_iot_no_count.h"
#include "output_iot.h"
#include "keypad.h"
// ak sth use in nhieu file thi k nên khai báo bên file .h
const char Keypad_Button_Values[4][4] =  {    {'1', '2', '3', 'A'},
											  {'4', '5', '6', 'B'},
											  {'7', '8', '9', 'C'},
											  {'*', '0', '#', 'D'},
                                         };


char check;
char check_key_pad_col_1(){
     output_set_level(GPIO_NUM_14,0); // for row 1 collect to 0V
    output_set_level(GPIO_NUM_16,1); 
    output_set_level(GPIO_NUM_17,1); 
    output_set_level(GPIO_NUM_32,1);
    check = test_col(1);
    if(check){
    return check;
    }
    return NO_ENTER;
}
char check_key_pad_col_2(void){
   output_set_level(GPIO_NUM_14,1); // for row 1 collect to 0V
    output_set_level(GPIO_NUM_16,0); 
    output_set_level(GPIO_NUM_17,1); 
    output_set_level(GPIO_NUM_32,1);
    check = test_col(2);
    if(check){
    return check;
    }
    return NO_ENTER;
}

char check_key_pad_col_3(void){
    output_set_level(GPIO_NUM_14,1); // for row 1 collect to 0V
    output_set_level(GPIO_NUM_16,1); 
    output_set_level(GPIO_NUM_17,0); 
    output_set_level(GPIO_NUM_32,1);
    check = test_col(3);
    if(check){
    return check;
    }
    return NO_ENTER;
}


char check_key_pad_col_4(void){

     output_set_level(GPIO_NUM_14,1); // for row 1 collect to 0V
    output_set_level(GPIO_NUM_16,1); 
    output_set_level(GPIO_NUM_17,1); 
    output_set_level(GPIO_NUM_32,0);
    check = test_col(4);
    if(check){
    return check;
    }

return NO_ENTER;
}

char test_col(int row){
    
        if(gpio_get_level(GPIO_NUM_25) == 0){
            while(gpio_get_level(GPIO_NUM_25) == 0);
            return Keypad_Button_Values[row - 1][0];
        }

       else if(gpio_get_level(GPIO_NUM_4) == 0){
            while(gpio_get_level(GPIO_NUM_4) == 0);
            return Keypad_Button_Values[row - 1][1];
        }

       else  if(gpio_get_level(GPIO_NUM_26) == 0){
            while(gpio_get_level(GPIO_NUM_26) == 0);
            return Keypad_Button_Values[row - 1][2];
        }

       else if(gpio_get_level(GPIO_NUM_13) == 0){
            while(gpio_get_level(GPIO_NUM_13) == 0);
            return Keypad_Button_Values[row - 1][3];
        }
return NO_ENTER;
}



void ionput_keypad_setup(void){
    input_io_create_no_count(GPIO_NUM_25,GPIO_INTR_falling);
    input_io_create_no_count(GPIO_NUM_4, GPIO_INTR_falling);
    input_io_create_no_count(GPIO_NUM_26, GPIO_INTR_falling);
    input_io_create_no_count(GPIO_NUM_13, GPIO_INTR_falling);
    output_io_create(GPIO_NUM_14);
    output_io_create(GPIO_NUM_16);
    output_io_create(GPIO_NUM_17);
    output_io_create(GPIO_NUM_32);
    output_io_create(GPIO_NUM_2);
    output_io_create(GPIO_NUM_12);
    output_io_create(GPIO_NUM_15);
   
}
