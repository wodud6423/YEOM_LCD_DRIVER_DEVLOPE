/*

lcd_app_copy,lcd_copy Code refer this Header File

Created on : 
Author : Yeom

*/
#ifndef __LCD_DRIVER_H
#define __LCD_DRIVER_H

#include <pthread.h>

#define NAME_MAX_BYTES 100

/* Labels For BBB GPIO numbers used to connect with LCD pins */

/* LCD Control pins Label */
#define GPIO_LCD_RS "gpio2.2"
#define GPIO_LCD_RW "gpio2.7"
#define GPIO_LCD_EN "gpio2.8"

/* LCD Data pins Label */
#define GPIO_LCD_DA4    "gpio2.9"
#define GPIO_LCD_DA5    "gpio2.10"
#define GPIO_LCD_DA6    "gpio2.11"
#define GPIO_LCD_DA7    "gpio2.12"

/* LCD Commands 
Refer to : eone-1602a1.pdf(1602a lcd Datasheet)
Commands Lists : 
    1) LCD_CMD_DIS_CLEAR : 0x01
    2) LCD_CMD_RETURN_HOME : 0x02
    3) LCD_CMD_SHIFT_LEFT : 0x07
    4) LCD_CMD_SHIFT_RIGHT : 0x05
    5) LCD_DIS_CON : 0x08 
    |---    5 - 1 ) LCD_DIS_ON : 0x04
    |---    5 - 2 ) LCD_DIS_CUR_ON : 0x02
    |---    5 - 3 ) LCD_DIS_CUR_BLK_ON : 0x01
    6 CMD is different to 5 CMD by "Without writing or reading display data, You can Control shift display or cursor"
    6) LCD_CUR_OR_DIS_SHT : 0x10
    |---    6 - 1 ) LCD_CUR_LEFT : 0x00
    |---    6 - 2 ) LCD_CUR_RIGHT : 0x04
    |---    6 - 3 ) LCD_DIS_LEFT_SHT : 0x08
    |---    6 - 4 ) LCD_DIS_RIGHT_SHT : 0x0C
    
    !!! Function Set !!!
    7) LCD_FUNC_SET : 0x20
    |---    7 - 1 ) LCD_4BIT_BUS_MODE : 0x00 <---> LCD_8BIT_BUS_MODE : 0x10
    |---    7 - 2 ) LCD_2LINE_MODE : 0x08 <---> LCD_1LINE_MODE : 0x00
    |---    7 - 3 ) LCD_FONT_TYPE_5X16 0x04 <---> LCD_FONT_TYPE_5X8 0x00
    --->
    8) LCD_SET_CGRAM_ADD    0x40
    9) LCD_SET_DDRAM_ADD    0x80
    
    Input Setting --> Cursor move 
    10) LCD_ENTRY_SET      0x04 /
    |---    10 - 1 ) LCD_INCRE_MODE 0x02 / LCD_DECRE_MODE LCD_ENTRY_SET
    |---    10 - 2 ) LCD_ENTRY_MOVE 0x01

-- Info about DDRAM address info when 1-line / 2-line display mode --
|    When 1-line display mode (N=0), DDRAM address is from "00H" to "4FH"
|    When 2-line display mode (N=1), DDRAM address in the 1st line is 
|from "00H" to "27H" , and DDRAM addresss in the 2nd line is from "40H" 
|to "67H"
----------------------------------------------------------------------
    */
#define LCD_DIS_CLEAR   0x01
#define LCD_CMD_RETURN_HOME 0x02
#define LCD_CMD_SHIFT_LEFT  0x07
#define LCD_CMD_SHIFT_RIGHT 0x05

/* LCD Display Control Flags */
#define LCD_DIS_CON 0x08

#define LCD_DIS_ON  (LCD_DIS_CON | 0x04)
#define LCD_DIS_CUR_ON  (LCD_DIS_CON | 0x02)
#define LCD_DIS_CUR_BLK_ON  (LCD_DIS_CON | 0x01)

#define LCD_DIS_OFF         LCD_DIS_CON
#define LCD_DIS_CUR_OFF     LCD_DIS_CON
#define LCD_DIS_CUR_BLK_OFF LCD_DIS_CON

/* LCD Display Cursor or Display Control without No change data */
#define LCD_CUR_OR_DIS_SHT  0x10
#define LCD_CUR_LEFT (LCD_CUR_OR_DIS_SHT | 0x00)
#define LCD_CUR_RIGHT (LCD_CUR_OR_DIS_SHT |  0x04)
#define LCD_DIS_LEFT_SHT (LCD_CUR_OR_DIS_SHT |  0x08)
#define LCD_DIS_RIGHT_SHT (LCD_CUR_OR_DIS_SHT |  0x0C)

/* LCD Function Setting Flags */
#define LCD_FUNC_SET    0x20

#define LCD_4BIT_BUS_MODE (LCD_FUNC_SET |  0x00)  
#define LCD_8BIT_BUS_MODE (LCD_FUNC_SET |  0x10)

#define LCD_2LINE_MODE (LCD_FUNC_SET |  0x08) 
#define LCD_1LINE_MODE (LCD_FUNC_SET |  0x00)

#define LCD_FONT_TYPE_5X16 (LCD_FUNC_SET | 0x04) 
#define LCD_FONT_TYPE_5X8 (LCD_FUNC_SET | 0x00)

/* Setting CGRAM And DDRAM Flags */
#define LCD_SET_CGRAM_ADD    0x40
#define LCD_SET_DDRAM_ADD    0x80

#define LCD_SET_DDRAM_ADD_FIRST     (LCD_SET_DDRAM_ADD | 0x00)
#define LCD_SET_DDRAM_ADD_SECOND    (LCD_SET_DDRAM_ADD | 0x40)

/* Entry Mode */
#define LCD_ENTRY_SET   0x04

#define LCD_INCRE_MODE  ( LCD_ENTRY_SET | 0x02 )
#define LCD_ENTRY_MOVE  ( LCD_ENTRY_SET | 0x01 )

#define LCD_DECRE_MODE  LCD_ENTRY_SET

/* 
    Callback for LCD CONTROL value before sending data.
*/
//#########################################################

// Define callback func type
typedef void (*callback_t)(void* data);

struct callback_entry {
    callback_t func;
    void * data;
};

// Declare callback func 
extern struct callback_entry callback_lcd_control;
extern pthread_spinlock_t cb_lock;
// Use spinlock parm for when callback func register , 
// doing atomic condition.

void init_callback_lock(void);
// When process end, destroy spinlock
void destroy_callback_lock(void);
// Register callback_func
void register_callback_control(callback_t cb_func,void* data);
// Callback Unregister Func
void unregister_callback_control(void);
// Callback Execute Mecro 
#define EXEC_CALLBACK_CON(cb,data) \
    do{                       \
        if (cb)               \
            cb(data);             \
    }while(0)               

// ##################################################
// Declare lcd_copy.c Funcs
void lcd_deinit(void);
void lcd_init(void);
void lcd_enable(void);
void lcd_data_send_4bit(uint8_t write_val);
void lcd_display_clear(void);
void lcd_printf(char* pr_str,...);
void lcd_send_command(uint8_t cmd);
void lcd_display_return_home(void);
void lcd_print_char(uint8_t chr);
void lcd_set_cursor(int row,int column);

void config_control_cmd(void* data);
void config_control_read_bf(void* data);
void config_control_write_data(void* data);
void config_control_read_data(void* data);
// #####################################################

// #######################################################
// Declare lcd_app_copy.c Func
int init_gpios(void);
void transition_graphics(void);
void print_time_and_date(void);
void print_ip_addr(void);
//##########################################################
#endif /* __LCD_DRIVER_H */
