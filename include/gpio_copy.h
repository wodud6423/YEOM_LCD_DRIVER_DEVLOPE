/* 

LCD GPIO Header File

Created on : 007_GPIO_SYSFS_LCD_COPY /  Mecro Parameters  For gpio_copy.c 
Author : Yeom

*/

#ifndef __GPIO_DRIVER_H
#define __GPIO_DRIVER_H

/* 
    Mecro Parameters for Use Low I/O operations.    
This holds path for gpio device(platform device) in sysfs
*/
#define SYSFS_GPIO_PATH "/sys/class/bone-gpios"
#define NAME_MAX_BYTES 100
/* Mecro Parameters for pin directions */
#define PIN_DIR_OUTPUT  0
#define PIN_DIR_INPUT   1

/* Mecro Parameters for gpio pins's value */
#define LOW_VALUE   0
#define HIGH_VALUE  1

int gpio_configure_dir(char* label,uint8_t dir_value);
int gpio_write_value(char * label,uint8_t out_value);
int gpio_read_value(char* label);
int gpio_file_open(char* label);
int gpio_file_close(int fd);

#endif /* __GPIO_DRIVER_H */
