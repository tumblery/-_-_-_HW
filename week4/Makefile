
all: app install

app: 
	arm-none-linux-gnueabi-gcc -static -o execute_4 code.c
install:
	cp -a execute_4 /nfsroot
clean:
	rm excute
