
#macros
builddir = ./build
srcdir = ./src
CC = gcc
CCFLAGS = -g

#make all target
iRTX-G19: keyboard crt rtx

#link the rtx objects
rtx: rtx.o	
	$(CC) -g -lrt -o $(builddir)/rtx \
	$(builddir)/kernelAPI.o \
	$(builddir)/rtx_main.o \
	$(builddir)/init.o \
	$(builddir)/queue.o \
	$(builddir)/iproc.o \
	$(builddir)/userProcesses.o \
	$(builddir)/userAPI.o \

#build the rtx object from all rtx source files	
rtx.o:
	$(CC) $(CCFLAGS) -c  $(srcdir)/rtx_main.c -o $(builddir)/rtx_main.o
	$(CC) $(CCFLAGS) -c  $(srcdir)/init.c -o $(builddir)/init.o
	$(CC) $(CCFLAGS) -c  $(srcdir)/queue.c -o $(builddir)/queue.o
	$(CC) $(CCFLAGS) -c  $(srcdir)/iproc.c -o $(builddir)/iproc.o
	$(CC) $(CCFLAGS) -c  $(srcdir)/userProcesses.c -o $(builddir)/userProcesses.o
	$(CC) $(CCFLAGS) -c  $(srcdir)/userAPI.c -o $(builddir)/userAPI.o
	$(CC) $(CCFLAGS) -c  $(srcdir)/kernelAPI.c -o $(builddir)/kernelAPI.o

#link the keyboard objects
keyboard: keyboard.o
	$(CC) -g -lrt -o $(builddir)/keyboard $(builddir)/keyboard.o

#build the keyboard object
keyboard.o: 
	$(CC) $(CCFLAGS) -c $(srcdir)/keyboard.c -o $(builddir)/keyboard.o

#link the crt objects
crt: crt.o
	$(CC) -g -lrt -o $(builddir)/crt $(builddir)/crt.o

#build the crt object
crt.o:
	$(CC) $(CCFLAGS) -c $(srcdir)/crt.c -o $(builddir)/crt.o

#clean everything in the build folder
clean:
	rm -r -f $(builddir)/*




