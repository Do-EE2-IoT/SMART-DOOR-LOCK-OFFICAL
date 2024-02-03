#ifndef __KEYPAD_H_
#define __KEYPAD_H_

#define NO_ENTER 'x'



/**
 * @brief set up for keypad
*/
void ionput_keypad_setup(void);

char test_col(int row);

char check_key_pad_col_1();
char check_key_pad_col_2();
char check_key_pad_col_3();
char check_key_pad_col_4();

#endif