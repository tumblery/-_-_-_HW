all: app install

app: 
	arm-none-linux-gnueabi-gcc -static -o execute_3 code.c
install:
	cp -a execute_3 /nfsroot
clean:
	rm execute_3
