# Makefile for demo Software
#
# To run the makefile, type "make"
#
# To learn more about makefiles, see the info under uwinfo:
#	 uwinfo / facilities / computers / supported software / unix
# or read the man pages
#


### User Modifiable Macros ###
#
# CC represents the compiler, LINK represents the linker
# CFLAGS represents the compiler command line parameters
# 	-c means compile only, i.e. create an object file
#	-g means add symbol table information for debugging purposes
# LNFLAGS represents the linker command line parameters
#
CC = gcc
CFLAGS = -c -g
LINK = gcc
LNFLAGS = -g


### Program list ###
##  list all of the executables to be created with the makefile



# default target: create all of the programs listed
#
all:	demo keyboard

#   programs
#   these commands link the object files and libraries into executables 

demo:demo.o 
	$(LINK) $(LNFLAGS) -o $@ demo.o -lrt
		

keyboard: keyboard.o 
	$(LINK) $(LNFLAGS) -o $@  keyboard.o -lrt

# compile the source code into object files

demo.o: demo.c demo.h
	$(CC) $(CFLAGS) demo.c

keyboard.o: keyboard.c demo.h
	$(CC) $(CFLAGS) keyboard.c 


