all: write reader

write: pshm_xfr_write.c
	gcc pshm_xfr_write.c -o write -lrt

reader: pshm_xfr_reader.c
	gcc pshm_xfr_reader.c -o reader -lrt

clean:
	rm write reader