all: usock_server usock_client

usock_server: usock_server.c lookup5.c
	gcc usock_server.c lookup5.c -o usock_server

usock_client: main.c lookup7.c
	gcc main.c lookup7.c -o usock_client

clean:
	rm usock_server usock_client