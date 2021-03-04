src = $(wildcard ./*.c)  
obj = $(patsubst ./%.c, ./%.o, $(src)) 

ALL:client server

client:client.o
	gcc client.o -o client -Wall -g
client.o:client.c
	gcc -c client.c -o client.o -Wall -g
server:server.o
	gcc server.o -o server -Wall -g
server.o:server.c
	gcc -c server.c -o server.o -Wall -g
clean:
		-rm -rf $(obj) $(target)

.PHONY: clean ALL