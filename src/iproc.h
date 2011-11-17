/* 
 * File:   iproc.h
 * Author: Jake
 *
 * Created on November 4, 2011, 1:01 PM
 */

#ifndef IPROC_H
#define	IPROC_H

#ifdef	__cplusplus
extern "C" {
#endif

void timer_iproc();
void kb_iproc();
void crt_iproc();
void signal_handler(int signum);


#ifdef	__cplusplus
}
#endif

#endif	/* IPROC_H */

