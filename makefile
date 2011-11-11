all: build/rtx_main build/keyboard build/crt

build/rtx_main: rtx_main.o init.o iproc.o kernelAPI.o  userAPI.o queue.o userProcesses.o 
	gcc -g -lrt -o rtx_main rtx_main.o init.o iproc.o kernelAPI.o userAPI.o queue.o userProcesses.o 

build/keyboard: keyboard.o
	gcc -g -lrt -o keyboard keyboard.o

build/crt: crt.o 
	gcc -g -lrt -o crt crt.o 

build/rtx_main.o : rtx_main.c rtx.h  init.c queue.c kernelAPI.c  kernelAPI.c iproc.c
	gcc -g -c rtx_main.c

build/iproc.o : iproc.c rtx.h queue.c kernelAPI.c 	
	gcc -g -c iproc.c

build/init.o :init.c queue.c iproc.c userProcesses.c 
	gcc -g -c init.c
 
build/kernelAPI.o : kernelAPI.c rtx.h queue.c init.c userProcesses.c
	gcc -g -c kernelAPI.c

build/crt.o : crt.c init.c iproc.c queue.c userProcesses.c
	gcc -g -c crt.c

build/keyboard.o : keyboard.c rtx.h queue.c 
	gcc -g -c keyboard.c

build/userAPI.o : userAPI.c kernelAPI.c rtx.h
	gcc -g -c userAPI.c

build/queue.o : queue.c queue.h
	gcc -g -c queue.c

build/userProcesses.o : userProcesses.c userAPI.c
	gcc -g -c userProcesses.c
	
#clean:
#	build/rtx_main.o build/init.o build/iproc.o build/kernelAPI.o build/crt.o build/keyboard.o build/userAPI.o build/queue.o build/userProcesses.o build/rtx_main build/keyboard build/crt 

#clean:
#	rtx_main.o init.o iproc.o kernelAPI.o crt.o keyboard.o userAPI.o queue.o userProcesses.o rtx_main keyboard crt 

clean:
	rm -r *.o
	rm *.exe
	rm in_buf
	rm out_buf
	rm rtx_main.exe.stackdump