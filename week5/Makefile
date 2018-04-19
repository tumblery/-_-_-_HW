
all: app install

app: 
	arm-none-linux-gnueabi-gcc -static -o execute_5 code.c
install:
	cp -a execute_5 /nfsroot
clean:
	rm excute
