CC=gcc
CFLAGS=-std=c99
 
all: tcpserver tcpclient udpserver udpclient  

tcpserver : 
	$(CC) -o tcpserver $(CFLAGS) tcpserver.c

tcplient : 
	$(CC) -o tcplient  $(CFLAGS) tcplient.c

udpserver : 
	$(CC) -o udpserver $(CFLAGS) udpserver.c

udpclient : 
	$(CC) -o udpclient $(CFLAGS) udpclient.c

clean :
	rm tcpserver tcpclient udpserver udpclient
 
