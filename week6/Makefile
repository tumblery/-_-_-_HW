all: app install

app: 
	arm-none-linux-gnueabi-gcc -static -o execute_6 code.c -pthread
install:
	cp -a execute_6 /nfsroot
clean:
	rm execute_6
