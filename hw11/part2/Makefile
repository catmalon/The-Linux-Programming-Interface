all: isock_server isock_client

isock_server: isock_server.c lookup5.c
	gcc isock_server.c lookup5.c -o isock_server

isock_client: main.c lookup8.c
	gcc main.c lookup8.c -o isock_client

clean:
	rm isock_server isock_client