
builddir = ./build
srcdir = ./src
CC = gcc
CCFLAGS = -g

iRTX-G19: keyboard crt rtx

rtx: rtx.o	
	$(CC) -g -lrt -o $(builddir)/rtx \
	$(builddir)/kernelAPI.o \
	$(builddir)/rtx_main.o \
	$(builddir)/init.o \
	$(builddir)/queue.o \
	$(builddir)/iproc.o \
	$(builddir)/userProcesses.o \
	$(builddir)/userAPI.o \
	
rtx.o:
	$(CC) $(CCFLAGS) -c  $(srcdir)/rtx_main.c -o $(builddir)/rtx_main.o
	$(CC) $(CCFLAGS) -c  $(srcdir)/init.c -o $(builddir)/init.o
	$(CC) $(CCFLAGS) -c  $(srcdir)/queue.c -o $(builddir)/queue.o
	$(CC) $(CCFLAGS) -c  $(srcdir)/iproc.c -o $(builddir)/iproc.o
	$(CC) $(CCFLAGS) -c  $(srcdir)/userProcesses.c -o $(builddir)/userProcesses.o
	$(CC) $(CCFLAGS) -c  $(srcdir)/userAPI.c -o $(builddir)/userAPI.o
	$(CC) $(CCFLAGS) -c  $(srcdir)/kernelAPI.c -o $(builddir)/kernelAPI.o

keyboard: keyboard.o
	$(CC) -g -lrt -o $(builddir)/keyboard $(builddir)/keyboard.o

keyboard.o: 
	$(CC) $(CCFLAGS) -c $(srcdir)/keyboard.c -o $(builddir)/keyboard.o

crt: crt.o
	$(CC) -g -lrt -o $(builddir)/crt $(builddir)/crt.o

crt.o:
	$(CC) $(CCFLAGS) -c $(srcdir)/crt.c -o $(builddir)/crt.o
	
clean:
	rm -r -f $(builddir)/*




