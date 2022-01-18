all: server client

server: fifo_server_nonblock.c 
	gcc fifo_server_nonblock.c -o server

client: fifo_client.c
	gcc fifo_client.c -o client


clean:
	rm server client