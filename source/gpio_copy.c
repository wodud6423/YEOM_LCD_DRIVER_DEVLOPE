/* 
==================================================
Name : gpio_copy.c
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
* This file contains low-level GPIO input/output methods for LCD data transfer, utilized in lcd_copy.c.
* 
* For more details, please refer to the README.md file.
* 
===================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "gpio_copy.h"

/* 
    GPIO configure direction Function
    dir_value : 1 means "output" , 0 means "input"
*/
int gpio_configure_dir(char* label, uint8_t dir_value)
{
    int fd,ret;
    char buf[NAME_MAX_BYTES];

    snprintf(buf,sizeof(buf),SYSFS_GPIO_PATH "/%s/direction",label);
    
    fd = open(buf,O_WRONLY | O_SYNC);
    if(fd < 0)
    {
        perror("gpio direction configure ! \n");
        return fd;
    }

    /* 
        if dir_value = 1 , direction = in 
        if dir_value = 0 , direction = out
        else , -EINVAL
    */
    if(dir_value == 1)
    {
        ret = write(fd,"in",3); // i + n + \n
    }
    else if(dir_value == 0){
        ret = write(fd,"out",4); // 
    }
    else{
        // Default Config!
        printf("Not Valied direction value ! \n out : 0 / in : 1 \n");
        return -EINVAL;
    }
    close(fd);

    return ret;
}
/* 
    GPIO write value
    val : can be either 0 or 1
*/
int gpio_write_value(char* label, uint8_t val)
{
    int fd,ret;
    char buf[NAME_MAX_BYTES];

    sprintf(buf,SYSFS_GPIO_PATH "/%s/value",label);

    fd = open(buf,O_WRONLY | O_SYNC);
    
    if(val == 1)
        ret = write(fd,"1",2);
    else if (val == 0)
        ret = write(fd,"0",2);
    else{
        printf("Failed to write value by Valid Input value! \n");
        return -EINVAL;
    }

    close(fd);

    return ret;
}

int gpio_value_read(char * label)
{
    int fd,ret;
    char buf[NAME_MAX_BYTES];
    uint8_t value = 0;

    sprintf(buf,SYSFS_GPIO_PATH "/%s/value",label);

    fd = open(buf,O_WRONLY | O_SYNC);
    
    if (fd < 0)
    {
        perror("gpio read value\n");
        return fd;
    }

    read(fd,&value,1);

    value = atoi(&value);

    close(fd);

    return value;
}

int gpio_file_open(char* label)
{
	return 0;
}

int gpio_file_close(int fd)
{
	return 0;
}
