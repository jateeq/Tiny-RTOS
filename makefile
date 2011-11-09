all: rtx_main keyboard crt

rtx_main: rtx_main.o init.o iproc.o kernelAPI.o  userAPI.o queue.o userProcesses.o 
	gcc -g -lrt -o rtx_main rtx_main.o init.o iproc.o kernelAPI.o userAPI.o queue.o userProcesses.o 

keyboard: keyboard.o
	gcc -g -lrt -o keyboard keyboard.o

crt: crt.o 
	gcc -g -lrt -o crt crt.o 

rtx_main.o : rtx_main.c rtx.h  init.c queue.c kernelAPI.c  kernelAPI.c iproc.c
	gcc -g -c rtx_main.c

iproc.o : iproc.c rtx.h queue.c kernelAPI.c 	
	gcc -g -c iproc.c

init.o :init.c queue.c iproc.c userProcesses.c 
	gcc -g -c init.c
 
kernelAPI.o : kernelAPI.c rtx.h queue.c init.c userProcesses.c
	gcc -g -c kernelAPI.c

crt.o : crt.c init.c iproc.c queue.c userProcesses.c
	gcc -g -c crt.c

keyboard.o : keyboard.c rtx.h queue.c 
	gcc -g -c keyboard.c

userAPI.o : userAPI.c kernelAPI.c rtx.h
	gcc -g -c userAPI.c

queue.o : queue.c queue.h
	gcc -g -c queue.c

userProcesses.o : userProcesses.c userAPI.c
	gcc -g -c userProcesses.c
	
clean:
	rtx_main.o init.o iproc.o kernelAPI.o crt.o keyboard.o userAPI.o queue.o userProcesses.o rtx_main keyboard crt 
