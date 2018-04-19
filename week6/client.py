from socket import *

sock = socket(AF_INET,SOCK_STREAM)
sock.connect(("164.125.70.173",6666))

print sock.recv(1024)
while 1:
	buf = raw_input()
	if buf == 'C':
		break	
	sock.send(buf)
	print sock.recv(1024)
	
sock.close()
