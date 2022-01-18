all: server client

server: svmsg_seqnum_server.c
	gcc svmsg_seqnum_server.c -o server

client: svmsg_seqnum_client.c
	gcc svmsg_seqnum_client.c -o client

clean:
	rm server client