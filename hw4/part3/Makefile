all: convert file_lookup

convert: convert.c
	gcc convert.c -o convert

file_lookup: main.c lookup1.c
	gcc main.c lookup1.c -o file_lookup

clean:
	rm convert file_lookup