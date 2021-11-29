all: upper mypopen

upper: upper.c
	gcc upper.c -o upper

mypopen: testpopen.c mypopen.c
	gcc testpopen.c mypopen.c -o mypopen

clean:
	rm *.c