make: server.c client.c
	gcc -pthread -g  -fsanitize=address -o myServer server.c 
	gcc -g -o myClient client.c

clean: server
	rm -f server
