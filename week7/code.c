/********************************************************/
/*			week 7 hw			*/
/*   		1's push switch CCTV			*/
/*		2's push switch capture			*/
/*		write by team 08			*/
/*		2018.04.13				*/
/********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#define MAX_BUTTON 9

#define PS_DEV "/dev/fpga_push_switch"

char* timeTostr(struct tm *t){
	static char s[20];
	sprintf(s,"%04d_%02d_%02d_%02d_%02d_%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
	return s;
}
void CCTV(int sig){
	if(sig == 1){
	system("./mxc_v4l2_overlay.out -ow 1920 -oh 1080");
	}
}

void capture(int  sig){
	if(sig ==1){
		char cmd[256];
		struct tm *t;
		time_t timer;
		
		timer = time(NULL);
		t = localtime(&timer);
		printf("[+][+]%s\n",timeTostr(t));
		sprintf(cmd,"ffmpeg -vcodec rawvideo -f rawvideo -pix_fmt yuyv422 -s 640x480 -i still.yuv -f image2 -vcodec png %s.png",timeTostr(t));
		system("./mxc_v4l2_still.out");
		system(cmd);
		printf("Capture!!!!\n");
	}
}
int main(void){
	pid_t btn_pid,act_pid[2];
	int btn_size,btn_dev;
	int btn_1_flag,btn_2_flag;
	unsigned char push_btn[MAX_BUTTON];
	int status;
	if((btn_dev = open(PS_DEV,0,O_RDWR))<0){
		printf("[-] ERROR: Device open push_switch_dev\n");
		close(btn_dev);	
		return -1;
	}
	btn_size = sizeof(push_btn);
	btn_1_flag = 0;
	btn_2_flag = 0;
	while(1){
		read(btn_dev,&push_btn,btn_size);
		if(push_btn[0] == 1 && btn_1_flag == 0){
			act_pid[0] = fork();//btn 1's proccess action
			printf("button 1\n");
			btn_1_flag = 1;
			if(act_pid[0] <0){
				printf("[+] can't fork CCTV proccess\n");
				return -1;
			}else if(act_pid[0] == 0){
					CCTV(1);
					exit(0);
			}
			usleep(250000);	 
		}
		if(push_btn[1] == 1 && btn_2_flag == 0){
			act_pid[1] = fork();
			printf("button 2\n");
			btn_2_flag = 1;
			if(act_pid[1] <0){
				printf("[+] can't fork capture proccess\n");
				return -1;
			}else if(act_pid[1] == 0){
				 capture(1);
                               	 exit(0);
			}
			usleep(250000);
		}
		btn_1_flag = btn_2_flag = 0;
	}
	
	return 0;
}
