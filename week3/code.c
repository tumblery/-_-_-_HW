/*********************************************************************
*
*	date : 2018.04.09
*	write: team08
*	hw : week03
	---> 1's push switch
			1 down LED on
			2 up LED on
			3 all LED on
			4 all LED off
	---> 2's push switch
			1 fnd : 2018
			2 fnd off
	---> 3's push switch
			1 dot matrix : 8
			2 dot matrix : off
	---> 4's push switch
			1 text lcd :  2017.03.16 \n Team 08
			2 text lex off
	---> 5's push switch 
			reset
***********************************************************************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include<termios.h>

#define FPGA_BASEADDR 0x08000000
#define LED_OFFSET 0x16
#define FND_OFFSET 0x4
#define DOT_OFFSET 0x210
#define TLCD_OFFSET 0x90
#define PUSH_SWITCH_OFFSET 0x50

void LED_CONTRL(unsigned short* addr, int leds);
void FND_CONTRL(unsigned short* addr, int val);
void DOT_CONTRL(unsigned short* addr, int loc, int val);
void TLCD_CONTRL(unsigned short* add, int loc, int val);

int main(void){
	int i;
	int fd;
	unsigned short* addr_fpga,*addr_led,*addr_fnd,*addr_dot,*addr_tlcd,*addr_push_switch;
	int btn_led,btn_fnd,btn_dot,btn_tlcd;
	int btn_flag=0;
//for init
	if((fd=open("/dev/mem",O_RDWR|O_SYNC))<0){
		perror("mem open error\n");
		exit(1);
	}
	
	addr_fpga = (unsigned short*)mmap(NULL,4096,PROT_WRITE|PROT_READ, MAP_SHARED, fd, FPGA_BASEADDR);
	addr_led = addr_fpga + LED_OFFSET/sizeof(unsigned short);
	addr_fnd = addr_fpga + FND_OFFSET/sizeof(unsigned short);
	addr_dot = addr_fpga + DOT_OFFSET/sizeof(unsigned short);
	addr_tlcd = addr_fpga + TLCD_OFFSET/sizeof(unsigned short);
	addr_push_switch = addr_fpga + PUSH_SWITCH_OFFSET/sizeof(unsigned short);

	if( *addr_led == (unsigned short)-1){
		printf("mmap error: led\n");
		exit(1);
	}
	if(*addr_fnd == (unsigned short)-1){
		printf("mmap error: fnd\n");
		exit(1);
	}
	if(*addr_dot == (unsigned short)-1){
		printf("mmap error: dot matrix\n");
		exit(1);
	}
	if(*addr_tlcd == (unsigned short)-1){
		printf("mmap error: text lcd\n");
		exit(1);
	}
	if(*addr_push_switch == (unsigned short) -1){
		printf("mmap error: push switch\n");
		exit(1);
	}
	btn_led = btn_fnd = btn_dot = btn_tlcd =0;
	printf("[+] PRESS CTRL+C for exit\n");
	while(1){
		if(btn_flag ==0){
		if(*addr_push_switch == 0x01){//1's push
			btn_flag = 1;
			btn_led++;
			btn_led = btn_led%4;
			printf("[+] 1' Push btn(LED) : %d's action\n",btn_led);
			if(btn_led == 1){//up on;
				LED_CONTRL(addr_led,0x0f);
			}else if(btn_led == 2){//down on;
				LED_CONTRL(addr_led,0xf0);
			}else if(btn_led == 3){//all on;
				LED_CONTRL(addr_led,0xff);
			}else if(btn_led == 0){//all off;
				LED_CONTRL(addr_led,0x00);
			}
			
		}
		else if(*(addr_push_switch+1) == 1){//2's push
			btn_flag = 1;
			btn_fnd++;
			btn_fnd = btn_fnd%2;
			printf("[+] 2' Push btn(FND) : %d's action\n",btn_fnd);
			if(btn_fnd ==1){
				FND_CONTRL(addr_fnd,0x2018);	
			}
			else if(btn_fnd ==0){
				FND_CONTRL(addr_fnd,0x0000);
			}
		}
		else if(*(addr_push_switch+2) == 1){//3's push
			btn_flag = 1;
			btn_dot++;
			btn_dot = btn_dot%2;
			printf("[+] 3' Push btn(DOT MATRIX) : %d's action\n",btn_dot);
			if(btn_dot == 1){
				DOT_CONTRL(addr_dot,0,0x00);
				DOT_CONTRL(addr_dot,1,0x1c);
				DOT_CONTRL(addr_dot,2,0x22);
				DOT_CONTRL(addr_dot,3,0x22);
				DOT_CONTRL(addr_dot,4,0x1c);
				DOT_CONTRL(addr_dot,5,0x3e);
				DOT_CONTRL(addr_dot,6,0x41);
				DOT_CONTRL(addr_dot,7,0x41);
				DOT_CONTRL(addr_dot,8,0x41);
				DOT_CONTRL(addr_dot,9,0x3e);
			}
			else if(btn_dot == 0){
				for(i=0;i<10;i++)
					DOT_CONTRL(addr_dot,i,0x00);
			}
		}
		else if(*(addr_push_switch+3) == 1){//4's push
			btn_flag = 1;
			btn_tlcd++;
			btn_tlcd = btn_tlcd%2;
			printf("[+] 1' Push btn(text lcd) : %d's action\n",btn_tlcd);
			if(btn_tlcd == 1){
				char* upper="2018.04.10";
				char* lower="Team 08";
				TLCD_CONTRL(addr_tlcd,0,0x2020);
				TLCD_CONTRL(addr_tlcd,1,0x2032);
				TLCD_CONTRL(addr_tlcd,2,0x3031);
				TLCD_CONTRL(addr_tlcd,3,0x382e);
				TLCD_CONTRL(addr_tlcd,4,0x3034);
				TLCD_CONTRL(addr_tlcd,5,0x2e31);
				TLCD_CONTRL(addr_tlcd,6,0x3020);
				TLCD_CONTRL(addr_tlcd,7,0x2020);
				TLCD_CONTRL(addr_tlcd,8,0x2020);
				TLCD_CONTRL(addr_tlcd,9,0x2020);
				TLCD_CONTRL(addr_tlcd,10,0x2054);
				TLCD_CONTRL(addr_tlcd,11,0x6561);
				TLCD_CONTRL(addr_tlcd,12,0x6d30);
				TLCD_CONTRL(addr_tlcd,13,0x3820);
				TLCD_CONTRL(addr_tlcd,14,0x2020);
				TLCD_CONTRL(addr_tlcd,15,0x2020);
			}
			else if(btn_tlcd == 0){
				for(i=0;i<16;i++)
					TLCD_CONTRL(addr_tlcd,i,0x2020);
			}
		}
		else if(*(addr_push_switch+4) == 1){//5's push
			btn_flag = 1;
			printf("[+] 5' Push btn(RESET)\n");
			LED_CONTRL(addr_led,0x00);
			FND_CONTRL(addr_fnd,0x0000);
			for(i=0;i<16;i++)
				TLCD_CONTRL(addr_tlcd,i,0x2020);
			for(i=0;i<10;i++)
				DOT_CONTRL(addr_dot,i,0x00);
		}}
		for(i = 0;i<1000;i++);
		btn_flag =0;
	}
			
	munmap(addr_fpga,4096);
	close(fd);
	return 0;
}
void LED_CONTRL(unsigned short* addr,int val){
	*addr = val;
}
void FND_CONTRL(unsigned short* addr,int val){
	*addr = val;
}
void DOT_CONTRL(unsigned short* addr,int loc, int val){
	*(addr+loc) = val;
}
void TLCD_CONTRL(unsigned short* addr,int loc,int val){
	*(addr+loc) = val;
}
