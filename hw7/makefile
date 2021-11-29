all: sigwaitinfo sigsuspend System_V_functions

sigwaitinfo: sig_speed_sigwaitinfo.c
	gcc sig_speed_sigwaitinfo.c -o sigwaitinfo

sigsuspend: sig_speed_sigsuspend.c
	gcc sig_speed_sigsuspend.c -o sigsuspend

System_V_function: System_V_functions.c
	gcc System_V_functions.c -o System_V_functions

clean: 
	rm sigwaitinfo sigsuspend System_V_functions