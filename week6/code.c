#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFF_SIZE 256
int circle;
int rect;
int clr;
void menu(int cli_sock){
	char buff[BUFF_SIZE];

	strcpy(buff,"+\t\t\t===menu===\t\t\t+\n|\t\t\t1.Draw circle\t\t\t|\n|\t\t\t2.Draw Rectangle\t\t\t|\n|\t\t3.Clear\t\t\t|\n+\t\t\t==========\t\t\t+\n");
	 write(cli_sock,buff,strlen(buff)+1);
}
void wifi_thread(void *data){
	int server_sock;
	int client_sock;
	int client_size;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	char buff_send[BUFF_SIZE];
	char buff_recv[BUFF_SIZE];
	if((server_sock = socket(AF_INET,SOCK_STREAM,0)) == -1){
		printf("[-] SOCK generate fail\n");
		exit(1);
	}
	memset( &server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(6666);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		//inet_addr("127.0.0.1");

	if(bind(server_sock,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1){
		printf("[-] Bind fail\n");
		exit(1);
	}
	if(listen(server_sock,5) == -1){
                        printf("[-] ERROR : listen\n");
                        exit(1);
                }
        client_size = sizeof(client_addr);
        if((client_sock = accept(server_sock,(struct sockaddr*)&client_addr,&client_size)) == -1){
                        printf("[-] ERROR: fail to connect client\n");
                        exit(1);

	}
	while(1){
/*		if(listen(server_sock,5) == -1){
			printf("[-] ERROR : listen\n");
			exit(1);
		}
		client_size = sizeof(client_addr);
		if((client_sock = accept(server_sock,(struct sockaddr*)&client_addr,&client_size)) == -1){
			printf("[-] ERROR: fail to connect client\n");
			exit(1);
		}*/
		menu(client_sock);
		read(client_sock,buff_recv,BUFF_SIZE);
		if(!strncmp(buff_recv,"1",1)){
			strcpy(buff_send,"[+]Draw Circle\n");
			circle = 1;
		}else if(!strncmp(buff_recv,"2",1)){
			strcpy(buff_send,"[+]Draw Renctangle\n");
			rect = 1;
		}else if(!strncmp(buff_recv,"3",1)){
			strcpy(buff_send,"[+]Clear\n");
			clr = 1;
		}
		write(client_sock,buff_send,sizeof(buff_send)+1);
		memset(buff_send,0,BUFF_SIZE);
		memset(buff_recv,0,BUFF_SIZE);
	}

	close(client_sock);
}
struct fb_var_screeninfo fvs;
typedef unsigned short int U32;
typedef short U16;
U16 makepixel(U32 r, U32 g, U32 b){
        U16 x = (U16)(r >>3);
        U16 y = (U16)(g >>2);
        U16 z = (U16)(b >>3);
        return (z|(x<<11)|(y<<5));
}
void DrawCircle(int fd,int num){
	int x,y,r;
	x = 100;
	y = 100;
	r = 60;
	U32 circle;
	num = num%3;
	switch(num){
		case 1:
			circle = makepixel(255,0,0);
			break;
		case 2:
			circle = makepixel(0,255,0);
			break;
		case 0:
			circle = makepixel(0,0,255);
			break;
	}
	int i,j;
	for(i=0;i<2*r;i++){
		for(j=0;j<2*r;j++){
			if(r*r > (i-r)*(i-r)+(j-r)*(j-r)){
				int off = (y+j)*fvs.xres*(sizeof(circle))+(x+i)*(sizeof(circle));
				if(lseek(fd,off,SEEK_SET)<0){
					printf("[-] ERROR: Lseek\n");
					exit(1);
				}
				write(fd,&circle,sizeof(circle));
			}
		}
	}
	
}

void DrawRectangle(int fd,int num){
	int x,y;
	x = 500;
	y = 500;
	int startx,starty;
	startx=starty = 300;
	U32 rect;
        num = num%3;
        switch(num){
                case 1:
                        rect = makepixel(255,0,0);
                        break;
                case 2:
                        rect = makepixel(0,255,0);
                        break;
                case 0:
                        rect = makepixel(0,0,255);
                        break;
        }
	int i,j;
	for(i = startx;i<startx+x;i++){
		int off = i*fvs.xres*sizeof(rect)+starty*sizeof(rect);
		if(lseek(fd,off,SEEK_SET)<0){
			printf("[-]ERROR: Lseek\n");
			exit(1);
		}
		for(j = starty;j<starty+y;j++){
			write(fd,&rect,sizeof(rect));	

		}
	}
}

void Clear(int fd){
	U32 pixel = makepixel(255,255,255);
	int i,j;
	for(i=0;i<fvs.xres;i++){
		for(j=0;j<fvs.yres;j++){
			int off = j*fvs.xres*sizeof(pixel)+i*sizeof(pixel);
			if(lseek(fd,off,SEEK_SET)<0){
				printf("[-]ERROR : LSEEK\n");
				exit(1);
			}
			write(fd,&pixel,sizeof(pixel));
		}
	}
}

void draw_thread(void* data){
	int check;
	int frame_dev;
	int circle_num,rect_num;
	circle_num = rect_num = 0;
	if((frame_dev=open("/dev/fb0",O_RDWR))<0){
		printf("[-]ERROR: Frame buffer\n");
		exit(1);
	}
	
	if((check = ioctl(frame_dev,FBIOGET_VSCREENINFO,&fvs))<0){
		printf("[-]ERROR: Get info\n");
		exit(1);
	}
	
	if(lseek(frame_dev,0,SEEK_SET)<0){
		printf("[-]ERROR: Lseek\n");
		exit(1);
	}
	Clear(frame_dev);
	while(1){
		if(circle ==1){
			circle_num++;
			DrawCircle(frame_dev,circle_num);
			circle = 0;
		}else if(rect == 1){
			rect_num++;
			DrawRectangle(frame_dev,rect_num);
			rect = 0;
		}else if(clr == 1){
			Clear(frame_dev);
			clr = 0;
		}
	}
}



int main(void){
	circle = rect = clr =0;
	
	pthread_t thread[2];
	int thread_id1;
	int thread_id2;
	int status;
	char data[] = "data";
	thread_id1 = pthread_create(&thread[0],NULL,wifi_thread,(void*)data);
	thread_id2 = pthread_create(&thread[1],NULL,draw_thread,(void*)data);
	
	if(thread_id1 <0){
		printf("[-] ERROR: thread wifi\n");
		return -1;
	}
	if(thread_id2<0){
		printf("[-] ERROR: thread draw\n");
		return -1;
	}
	while(1){

		usleep(2500);
	}

	pthread_join(thread[0],(void**)status);
	pthread_join(thread[1],(void**)status);


	return 0;
}
