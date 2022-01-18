all: server client

server: fifo_server_nonblock.c 
	gcc fifo_server_nonblock.c -o server

client: fifo_client_block.c
	gcc fifo_client_block.c -o client


clean:
	rm server client