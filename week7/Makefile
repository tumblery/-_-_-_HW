all: app install

app: 
	arm-none-linux-gnueabi-gcc -static -o execute_7 code.c
install:
	cp -a execute_7 /nfsroot
clean:
	rm execute_7
