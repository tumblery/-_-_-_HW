#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <string.h>

#include "./fpga_dot_font.h"

#define MAX_BUTTON 9
#define TLCD_LINE 16
#define TLCD_BUFF 32

#define TLCD_DEV "/dev/fpga_text_lcd"
#define LED_DEV "/dev/fpga_led"
#define PUSH_SWITCH_DEV "/dev/fpga_push_switch"
#define FND_DEV "/dev/fpga_fnd"
#define DOT_DEV "/dev/fpga_dot"

#define led_down 15
#define led_up 	240
#define led_all 255
unsigned char quit = 0;
void user_signal1(int sig){
        quit = 1;
}
int main(void){
        int i,led_chk,led_btn,led_ret,fnd_btn,fnd_ret,dot_btn,dot_ret,tlcd_btn,tlcd_ret;
        int push_dev,led_dev,fnd_dev,dot_dev,tlcd_dev;
        int butn_size,dot_num,dot_size,tlcd_str_size;
        unsigned char push_butn[MAX_BUTTON],tlcd_string[TLCD_BUFF],tlcd_NULL[TLCD_BUFF];
	char* tlcd_tmp;
	char fnd_str[4] = "2018";//2018
	char fnd_null[4]= "0000";
        if((push_dev = open(PUSH_SWITCH_DEV,0,O_RDWR))<0){
                printf("[-] ERROR: Device open %s\n", PUSH_SWITCH_DEV);
                close(push_dev);
                return -1;
        }
	if((led_dev=open(LED_DEV,O_RDWR)) <0){
		printf("[-] ERROR: device open %s\n",LED_DEV);
		close(led_dev);
		return -1;
	}
	if((fnd_dev= open(FND_DEV,O_RDWR))<0){
		printf("[-] ERROR: device open %s\n",FND_DEV);
		close(fnd_dev);
		return -1;
	}
	if((dot_dev = open(DOT_DEV,O_WRONLY)) <0){
		printf("[-] ERROR: device open %s\n",DOT_DEV);
		close(dot_dev);
		return -1;
	}
	if((tlcd_dev = open(TLCD_DEV,O_WRONLY))<0){
		printf("[-] ERROR: device open %s\n",TLCD_DEV);
		return -1;
	}
        (void)signal(SIGINT,user_signal1);
        butn_size = sizeof(push_butn);
	for(i=0;i<4;i++){
		fnd_str[i] = fnd_str[i]-0x30;
		fnd_null[i] = fnd_null[i]-0x30;
	}
	dot_num = 8;
	dot_size = sizeof(fpga_number[dot_num]);
	tlcd_tmp = "   2018.04.19   ";
	strncat(tlcd_string,tlcd_tmp,16);
	tlcd_tmp = "     Team08     ";
	strncat(tlcd_string+16,tlcd_tmp,16);
	memset(tlcd_NULL,' ',TLCD_BUFF);
	led_btn = fnd_btn = dot_btn = tlcd_btn =0;
        printf("Press <ctrl+c> to quit.\n");
        while(!quit){
                usleep(400000);
                read(push_dev,&push_butn,butn_size);
                if(push_butn[0] ==1){//if push button 1 about LED
			printf("[+] PRESSED 1's btn : %d\n",led_btn);
			switch(led_btn){
				case 0://led button first down led on
					led_chk = led_down;
					break;				
				case 1://led seconde up on
					led_chk =led_up;
					break;
				case 2://led third all on
					led_chk = led_all;
					break;
				case 3://led off;
					led_chk = 0;
					break;
				}
			if((led_ret = write(led_dev,&led_chk,1)) <0){
				printf("[-] ERROR: device write led\n");
				break;
			}
			led_btn = (++led_btn)%4;	
                        }
                else if(push_butn[1]==1){//if push button 2 about fnd
			printf("[+] PRESSED 2's btn : %d\n",fnd_btn);
			switch(fnd_btn){
				case 0://2018 
					if((fnd_ret=write(fnd_dev,&fnd_str,4))<0){
						printf("[-] ERROR: device write fnd\n");
						return -1;
					}
					break;
				case 1://off
					if((fnd_ret=write(fnd_dev,&fnd_null,4))<0){
                                                printf("[-] ERROR: device wirte fnd\n");
                                                return -1;
                                        }
                                        break;
				}
			fnd_btn = (++fnd_btn)%2;
                        }
                else if(push_butn[2] == 1){//if push button 3 about dot matrix
			printf("[+]pressed 3's btn : %d\n",dot_btn);
			switch(dot_btn){
				case 0:
					if((dot_ret=write(dot_dev,fpga_number[dot_num],dot_size))<0){
						printf("[-] ERROR: device write dot\n");
						return -1;
					}
					break;
				case 1:
					if((dot_ret=write(dot_dev,fpga_set_blank,dot_size))<0){
                                                printf("[-] ERROR: device write dot\n");
                                                return -1;
                                        }
                                        break;
				}	
			 dot_btn = (++dot_btn)%2;
                        }
                else if(push_butn[3] ==1){//if push button 4 about text lcd
			printf("[+]pressed 4's btn : %d\n",tlcd_btn);
			switch(tlcd_btn){
				case 0:
					if((tlcd_ret=write(tlcd_dev,tlcd_string,TLCD_BUFF)) < 0){
						printf("[-] ERROR: device write tlcd\n");
						return -1;
					}
					break;
				case 1:
					if((tlcd_ret=write(tlcd_dev,tlcd_NULL,TLCD_BUFF)) < 0){
                                                printf("[-] ERROR: device write tlcd\n");
                                                return -1;
                                        }
                                        break;

			}
			tlcd_btn = (++tlcd_btn)%2;
		
                }
                else if(push_butn[4]==1){//if push button 5 about reset
			printf("[+]pressed init btn\n");
			/*init LED*/
			led_chk = 0;
			if((led_ret = write(led_dev,&led_chk,1)) <0){
                                printf("[-] ERROR: device write led\n");
                                break;
                        }
			/*init FND*/
			if((fnd_ret=write(fnd_dev,&fnd_null,4))<0){
                                printf("[-] ERROR: device wirte fnd\n");
                                return -1;
                         }

			/*init dot*/
			if((dot_ret=write(dot_dev,fpga_set_blank,dot_size))<0){
                                printf("[-] ERROR: device write dot\n");
                                return -1;
                         }
			/*init tlcd*/
			 if((tlcd_ret=write(tlcd_dev,tlcd_NULL,TLCD_BUFF)) < 0){
                                printf("[-] ERROR: device write tlcd\n");
                                return -1;
                         }


                }
		usleep(200000);
        }
	led_chk =0;
	if((led_ret = read(led_dev,&led_chk,1))<0){
		printf("[-] EROOR: device read led\n");
	}
	close(tlcd_dev);
	close(dot_dev);
	close(fnd_dev);
	close(led_dev);
        close(push_dev);
	return 0;
}

