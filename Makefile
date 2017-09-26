#gcc -Wall server.c -o server -pthread 
CC	=gcc
CFLAGS	=-I/usr/lib/x86_64-linux-gnu/gstreamer-1.0/include -I/usr/include/glib-2.0 \
	-I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/gstreamer-1.0 \
	-I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lglib-2.0 \
	-Wall \
	-pthread
LIBS	=-lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lgstrtspserver-1.0 -lglib-2.0

all : clean
	$(CC) server.c -o server $(CFLAGS) $(LIBS)

clean :
	rm -f server.o server

run: server
	./server
