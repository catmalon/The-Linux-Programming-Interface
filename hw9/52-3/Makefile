all: server client

server: pmsg_file_server.c
	gcc pmsg_file_server.c -o server -lrt

client: pmsg_file_client.c
	gcc pmsg_file_client.c -o client -lrt

clean:
	rm server client