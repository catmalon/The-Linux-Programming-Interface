all: udp_server udp_client

udp_server: udp_server.c lookup5.c
	gcc udp_server.c lookup5.c -o udp_server

udp_client: main.c lookup9.c
	gcc main.c lookup9.c -o udp_client

clean:
	rm udp_server udp_client