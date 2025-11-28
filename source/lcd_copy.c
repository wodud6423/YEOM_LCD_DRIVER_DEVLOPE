/* 
==================================================
Name : lcd_copy.c
Version : 1.0
Copyright : 

 * Copyright (C) 2025 JeaYoung YEOM
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 
Description : 
 * This file provides basic LCD control functions for the lcd_app_copy application.
 * Functions include:
 *   - lcd_init / lcd_deinit: Initialize or reset the LCD.
 *   - lcd_enable: Latch data to DDRAM (supports 4-bit interface).
 *   - lcd_data_send_4bit: Send data using 4-bit mode (high nibble -> latch -> low nibble -> latch).
 *   - EXEC_CALLBACK_CON: Configure RW/RS pins before data transmission (used as callbacks).
 *   - config_control_*: Callback handlers for RW/RS states (4 cases).
 *
 * Refer to README.md for usage details of remaining functions.

===================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <pthread.h>

#include "gpio_copy.h"
#include "lcd_copy.h"
#include "char_copy.h"

pthread_spinlock_t cb_lock;
struct callback_entry callback_lcd_control = {0}; 
void init_callback_lock(void)
{
	if(pthread_spin_init(&cb_lock,PTHREAD_PROCESS_PRIVATE) != 0)
	{
		perror("pthread_spin_init failed! \n");
		exit(1);
	}
}

// When process end, destroy spinlock
void destroy_callback_lock(void)
{
	pthread_spin_destroy(&cb_lock);
}

// Callback Register Func
void register_callback_control(callback_t cb_func,void* data)
{
    pthread_spin_lock(&cb_lock);
    callback_lcd_control.func = cb_func;
    callback_lcd_control.data = data;
    pthread_spin_unlock(&cb_lock);
}

// Callback Unregister Func
void unregister_callback_control(void)
{
    pthread_spin_lock(&cb_lock);
    callback_lcd_control.func = NULL;
    callback_lcd_control.data = NULL;
    pthread_spin_unlock(&cb_lock);
}
/*
	LCD Deinit Setting
	Description : LCD Display and Cursor Setting init
*/
void lcd_deinit(void)
{
	lcd_display_clear();
	lcd_display_return_home();
}
/*
	LCD Init Setting
	Description : LCD First initialize config
	Refer to : LCD-1602A data sheet
	(4-bit interface)
*/
void lcd_init(void)
{
	/* Wait 30ms (Recommend : more than 15ms)*/
	usleep(30 * 1000);
	/*
		RS R/W	DB7 DB6 DB5 DB4
		0	0	0	0	1	1	
	*/
	gpio_write_value(GPIO_LCD_DA5,HIGH_VALUE);
	gpio_write_value(GPIO_LCD_DA4,HIGH_VALUE);
	lcd_enable();
	/* Wait 9ms (Recommend : more than 4.1ms) */
	usleep(9 * 1000);
	/*
		RS R/W	DB7 DB6 DB5 DB4
		0	0	0	0	1	1	
	*/
	gpio_write_value(GPIO_LCD_DA5,HIGH_VALUE);
	gpio_write_value(GPIO_LCD_DA4,HIGH_VALUE);
	lcd_enable();
	/* Wait 10ms (Recommend : more than 100us )*/
	usleep(200);
	/*
		RS R/W	DB7 DB6 DB5 DB4
		0	0	0	0	1	1	
	*/
	gpio_write_value(GPIO_LCD_DA5,HIGH_VALUE);
	gpio_write_value(GPIO_LCD_DA4,HIGH_VALUE);
	lcd_enable();
	usleep(10*1000);
	/*
		RS R/W	DB7 DB6 DB5 DB4
		0	0	0	0	1	0	
	*/
	gpio_write_value(GPIO_LCD_DA5,HIGH_VALUE);
	gpio_write_value(GPIO_LCD_DA4,LOW_VALUE);
	
	lcd_enable();
	
	/*
		LCD Function Setting
		1. 2LINE MODE
		2. FONT TYPE 5x16
	
	*/
	lcd_data_send_4bit(LCD_FUNC_SET | LCD_2LINE_MODE | LCD_FONT_TYPE_5X16);
	
	/* Display Off */
	lcd_data_send_4bit(LCD_DIS_OFF);

	/* Display Clear */
	lcd_data_send_4bit(LCD_DIS_CLEAR);
	
	/* 
		Entry Mode setting 
		1. LCD_INCRE_MODE
		2. LCD_ENTRY_MOVE	
	*/
	lcd_data_send_4bit(LCD_ENTRY_SET | LCD_INCRE_MODE | LCD_ENTRY_MOVE);

	usleep(100);

	printf("Initialize LCD Finish! \n");
	return ;
}

/*
	LCD Data letch by EN HIGH -> LOW
	Refer to : 1602A LCD datasheet

	Description : 
	Output Display by DDRAM Data Letch. 
	Letch Cycle is 1000ns(1us).
	if we send data or cmd, we need to config EN value by HIGH -> LOW
	Then send data. 
*/
void lcd_enable(void)
{
	usleep(1);
	gpio_write_value(GPIO_LCD_EN,HIGH_VALUE);
	usleep(1);
	gpio_write_value(GPIO_LCD_EN,LOW_VALUE);
	usleep(1);
}

/*
	LCD data send by 4 bit interface
	Refer to : 1602A LCD datasheet

	Description :
	Input parameter is 8bit value. And we send higher 4bit -> lower 4bit
*/
void lcd_data_send_4bit(uint8_t write_val)
{
	/* Control Register Setting by Callback func */
	EXEC_CALLBACK_CON(callback_lcd_control.func,callback_lcd_control.data);
	/* First , Send to Higher 4bit */
	gpio_write_value(GPIO_LCD_DA4,(write_val >> 4) & 1);
	gpio_write_value(GPIO_LCD_DA5,(write_val >> 5) & 1);
	gpio_write_value(GPIO_LCD_DA6,(write_val >> 6) & 1);
	gpio_write_value(GPIO_LCD_DA7,(write_val >> 7) & 1);
	/* Data Letch */
	lcd_enable();

	EXEC_CALLBACK_CON(callback_lcd_control.func,callback_lcd_control.data);
	/* Second , send to Lower 4bit */
	gpio_write_value(GPIO_LCD_DA4,(write_val >> 0) & 1);
	gpio_write_value(GPIO_LCD_DA4,(write_val >> 1) & 1);	gpio_write_value(GPIO_LCD_DA4,(write_val >> 2) & 1);	gpio_write_value(GPIO_LCD_DA4,(write_val >> 3) & 1);

	/* Data Letch */
	lcd_enable();

	/* Unregister Callback func */
	unregister_callback_control();
}
/* 
###################################################################
Control Config pins Write values Callback Funcs
###################################################################
*/
void config_control_cmd(void* data)
{
	gpio_write_value(GPIO_LCD_RS,LOW_VALUE);
	gpio_write_value(GPIO_LCD_RS,LOW_VALUE);
}

void config_control_read_bf(void* data)
{
	gpio_write_value(GPIO_LCD_RS,LOW_VALUE);
	gpio_write_value(GPIO_LCD_RW,HIGH_VALUE);
}

void config_control_write_data(void* data)
{
	gpio_write_value(GPIO_LCD_RS,HIGH_VALUE);
	gpio_write_value(GPIO_LCD_RW,LOW_VALUE);
}

void config_control_read_data(void* data)
{
	gpio_write_value(GPIO_LCD_RS,HIGH_VALUE);
	gpio_write_value(GPIO_LCD_RW,HIGH_VALUE);
}
//################################################################

void lcd_display_clear(void)
{
	register_callback_control(config_control_cmd,NULL);
	lcd_data_send_4bit(LCD_DIS_CLEAR);
	usleep(1);
}

void lcd_printf(char * pr_str,...)
{
	va_list args;
	va_start(args,pr_str);

	int i = 0,size_str;
	char message_buf[32];

	size_str = vsprintf(message_buf,pr_str,args);

	while(i < size_str)
	{
		/* need to config RS  = 1 */
		// gpio_write_value(GPIO_LCD_RS,HIGH_VALUE);
		// lcd_data_send_4bit((uint8_t)pr_str[i]);	
		if(message_buf[i] < LF)
		{
			break;
		}
		else{
			if(message_buf[i] >= ZERO && message_buf[i] <= ASCII_END)
			{
				lcd_print_char(message_buf[i]);
				usleep(1);
			}
		}
		i++;
	}
	printf("Successful Print Stinrg to LCD ! \n");
	
	va_end(args);
	
	return ;

}

void lcd_send_command(uint8_t cmd)
{
	register_callback_control(config_control_cmd,NULL);
	/* IR write */
	lcd_data_send_4bit(cmd);
	usleep(1);
}

void lcd_display_return_home(void)
{
	register_callback_control(config_control_cmd,NULL);
	lcd_data_send_4bit(LCD_CMD_RETURN_HOME);
	usleep(1);
}

void lcd_print_char(uint8_t chr)
{
	register_callback_control(config_control_write_data,NULL);
	lcd_data_send_4bit(chr);
	usleep(1);
}

void lcd_set_cursor(int row, int column)
{
	uint8_t row_v, col_v;
	switch(row)
	{
		case 1 : 
			row_v = 0x00; // 1st Row 
			break;
		case 2 :
			row_v = 0x40; // 2nd Row
			break;
		default:
			perror("Not valied Row value! \n");
			return;
	}
	if(column < 0x00 || column > 0x0F)
	{
		perror("Not valied Column value \n");
		return;
	}
	col_v = column;
	
	lcd_send_command(LCD_SET_DDRAM_ADD | col_v | row_v );
	
	return;
}

