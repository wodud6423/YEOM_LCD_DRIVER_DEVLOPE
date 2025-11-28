/*
=====================================================
Name	:	lcd_app.c
Author	:	Yeom <wodud043@naver.com>
Version	:	None
Copyright	:	No Have (GPL License)
Description	:	This application prints your message on the 16x2 LCD along with ip address of BBB
*/

// printf , scanf , stdin 와 같은 입출력 라이브러리
#include <stdio.h>

// 표준 라이브러리, atoi , strtol 과 같은 라이브러리
#include <stdlib.h>

// string 관련 라이브러리 , strlen , strcpy, strncpy, strcmp, strcat
#include <string.h>

// 표준 에러 코드 관련 라이브러리 -ENOMEM, -EINVAL
#include <errno.h>

// POSIX 표준의 저수준 입출력 라이브러리
#include <unistd.h>

// File 저수준 입출력 관련 옵션 상수 라이브러리
#include <fcntl.h>

// 다수 입출력을 관리하기 위한 poll() 라이브러리
#include <poll.h>

// 고정 폭 정수 타입 제공 라이브러리
#include <stdint.h>

// 날짜 , 시간 관련 타입의 라이브러리
#include <time.h>

// 수학 함수
#include <math.h>

// 시스템 레벨에서 쓰이는 다양한 타입의 라이브러리
#include <sys/types.h>

// 소켓 API 관련 타입 상수 함수 선언
#include <sys/socket.h>

// 디바이스 / 소켓 등 파일 디스크립터에 대해 입출력 제어 명령을 수행하는 ioctl
#include <sys/ioctl.h>

// 인터넷 주소 체계관련 라이브러리
#include <netinet/in.h>

// 네트워크 인터페이스 관련 구조체 / 매크로
#include <net/if.h>

// 텍스트<->바이너리 ip 주소 변환과 관련된 함수 제공
#include <arpa/inet.h>

#include "lcd_copy.h"
#include "gpio_copy.h"

/*
=========================================================
BBB_expansion_header_pins   GPIO_num    16x2 LCD pins   Purpose
P8.7    gpio2.2 4(RS)   Register selection(Charater(H) vs command(L))
P8.46                             gpio2.7          5(RW)           Read/write 
P8.43                             gpio2.8          6(EN)           Enable
P8.44                             gpio2.9          11(D4)          Data line 4
P8.41                             gpio2.10         12(D5)          Data line 5
P8.42                             gpio2.11         13(D6)          Data line 6
P8.39                             gpio2.12         14(D7)          Data line 7 
P9.7(GND)                                          15(BKLTA)       Backlight anode(+)
P9.1(sys_5V supply)                                16(BKLTK)       Backlight cathode(-)

P9.1 (GND)                        ----             1(VSS/GND)      Ground
P9.7(sys_5V supply)               ----             2(VCC)          +5V  

========================================================
*/
int init_gpios(void)
{
    /* Configure the direction for LCD pins */
    gpio_configure_dir(GPIO_LCD_RS,PIN_DIR_OUTPUT);
    gpio_configure_dir(GPIO_LCD_RW,PIN_DIR_OUTPUT);
    gpio_configure_dir(GPIO_LCD_EN,PIN_DIR_OUTPUT);

    gpio_configure_dir(GPIO_LCD_DA4,PIN_DIR_OUTPUT);
    gpio_configure_dir(GPIO_LCD_DA5,PIN_DIR_OUTPUT);
    gpio_configure_dir(GPIO_LCD_DA6,PIN_DIR_OUTPUT);
    gpio_configure_dir(GPIO_LCD_DA7,PIN_DIR_OUTPUT);

    /* Reset All Pin by 0 */
    gpio_write_value(GPIO_LCD_RS,LOW_VALUE);
    gpio_write_value(GPIO_LCD_RW,LOW_VALUE);
    gpio_write_value(GPIO_LCD_EN,LOW_VALUE);
    
    gpio_write_value(GPIO_LCD_DA4,LOW_VALUE);
    gpio_write_value(GPIO_LCD_DA5,LOW_VALUE);
    gpio_write_value(GPIO_LCD_DA6,LOW_VALUE);
    gpio_write_value(GPIO_LCD_DA7,LOW_VALUE);
}
/*
    Transition Graphics 
    Description : 
        This Function for 5x16 Font Type. 
*/

void transition_graphics(void)
{
	sleep(1);

	lcd_set_cursor(1,1);
    
    lcd_display_clear();

    for(uint8_t n = 0 ; n < 2; n++)
    {
        for(uint8_t i = 0 ; i < 16; i++)
        {
            lcd_print_char('*');
            usleep(75*1000);
        }
        lcd_set_cursor(2,1);
        lcd_send_command(LCD_ENTRY_SET | LCD_INCRE_MODE);
    }
    
    lcd_set_cursor(1,1);
    lcd_send_command(LCD_ENTRY_SET | LCD_INCRE_MODE);
    usleep(450 * 1000);

    lcd_display_clear();
}

void print_time_and_date(void)
{
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime); // receive time by secs
    timeinfo = localtime( &rawtime); // receive time by struct tm

    /* First Line DDRAM */
    lcd_send_command(LCD_SET_DDRAM_ADD_FIRST);

    lcd_printf("%d-%d-%d",1900+timeinfo->tm_year,timeinfo->tm_mon + 1,timeinfo->tm_mday);

    /* Second Line DDRAM */
    lcd_send_command(LCD_SET_DDRAM_ADD_SECOND);
    
    lcd_printf("%d:%d:%d",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    
}

void print_ip_addr(void)
{
    int fd;
    struct ifreq ifr;

    char iface[] = "usb0";

    fd = socket(AF_INET,SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;

    strncpy(ifr.ifr_name,iface,IFNAMSIZ-1);

    // Receive IP_ADDR by ifreq.ifr_addr
    ioctl(fd,SIOCGIFADDR,&ifr);

    // 1. print the interface name on the first line
    lcd_send_command(LCD_SET_DDRAM_ADD_FIRST);
    lcd_printf((char*)iface);
    lcd_send_command(LCD_SET_DDRAM_ADD_SECOND);
    lcd_printf((char*)inet_ntoa( ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr ));
    
    return ;
}

int main(int argc, char* argv[])
{
    char msg_buf[81];
    int value;

    init_callback_lock();

    printf("This Application prints your message on the 16x2 LCD! \n");
    
    // Init gpio - Direction and Default Value(LOW) Config
    init_gpios();

    printf("Enter your message(Max. 80 char) : \n");
    scanf("%80[^\n]s",msg_buf);

    int msg_len = strlen(msg_buf);

    printf("Message %d: %s\n",msg_len,msg_buf);

    /* Init of LCD */
    lcd_init();

    sleep(1);

    while(1)
    {
        lcd_display_clear();

        lcd_printf(msg_buf);

        sleep(1);

        /* if message length is > 16, do left shift of message ! */
        
        if(msg_len > 16)
        {
            for(int i = 0 ; i < msg_len-16; i++)
            {
                // LCD command to left shift the display
                lcd_send_command(LCD_DIS_LEFT_SHT);
                usleep(500*1000);
            }
        }

        /* Clear display */
        lcd_display_clear();

        /* Return home */
        lcd_display_return_home();

        transition_graphics();
        /* Print IP address of BBB */
        print_ip_addr();
        sleep(1);
        transition_graphics();
        /* Print data and time info */
        print_time_and_date();
        sleep(1);
        transition_graphics();
    }

    lcd_deinit();

    destroy_callback_lock();
    return 0;

}
