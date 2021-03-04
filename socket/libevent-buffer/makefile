src = $(wildcard ./*.c)  
obj = $(patsubst ./%.c, ./%.o, $(src)) 

ALL:client server

client:client.o
	gcc client.o -o client -Wall -g -levent
client.o:client.c
	gcc -c client.c -o client.o -Wall -g -levent

server:server.o
	gcc server.o -o server -Wall -g -levent
server.o:server.c
	gcc -c server.c -o server.o -Wall -g -levent

clean:
		-rm -rf $(obj) $(target)

.PHONY: clean ALL