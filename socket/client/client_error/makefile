src = $(wildcard ./*.c)  
obj = $(patsubst ./%.c, ./%.o, $(src)) 

ALL:client server

wrap.o:wrap.c
	gcc -c wrap.c -o wrap.o -Wall -g

client:client.o wrap.o
	gcc client.o wrap.o -o client -Wall -g
client.o:client.c
	gcc -c client.c -o client.o -Wall -g

server:server.o wrap.o
	gcc server.o wrap.o -o server -Wall -g
server.o:server.c
	gcc -c server.c -o server.o -Wall -g

clean:
		-rm -rf $(obj) $(target)

.PHONY: clean ALL