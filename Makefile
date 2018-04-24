make: server.c client.c
	gcc -pthread -g -Wall -Werror -fsanitize=address -o server server.c 

clean: server
	rm -f server
