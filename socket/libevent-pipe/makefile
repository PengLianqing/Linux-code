src = $(wildcard ./*.c)  
obj = $(patsubst ./%.c, ./%.o, $(src)) 

ALL:write read

read:read.o
	gcc read.o -o read -Wall -g -levent
read.o:read.c
	gcc -c read.c -o read.o -Wall -g -levent

write:write.o
	gcc write.o -o write -Wall -g -levent
write.o:write.c
	gcc -c write.c -o write.o -Wall -g -levent

clean:
		-rm -rf $(obj) $(target)

.PHONY: clean ALL