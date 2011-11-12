
GROUP NO: 19

MEMBERS:
Jake Park - 20340649
Jawad Ateeq - 20327861
Yifei Cheng - 20344112
Zhuojun Li - 20359456

FILE DESCRIPTIONS:

rtx.h: contains all constants, struct definitions and kernel data fields
init.h: contains function prototypes of initialization routines
iproc.h: contains function prototypes of iprocesses
kernelAPI.h: contains prototypes for all kernel routines that will run indivisibly
userAPI.h: contains prototypes for user process callable versions of the kernel api routines
userProcesses.h: contains function prototypes of user processes
queue.h: contains prototypes for all queue strucutres implemented in the RTX

rtx_main.c: contains code to set signal handlers and to fork keyboard and child processes
crt.c: contains code for the CRT user process
keyboard.c: contains code for the keyboard user process
queue.c: contains implementations for all kernel queues
kernelAPI.c: contains implementation of all kernel api functions
iproc.c: contians implementations of all iprocesses
init.c: contians implementations of all routines used by the kernel to initialize itself
userAPI.c: contains implementations of all userapi functions
userProcesses.c: contains implementation of user process P


