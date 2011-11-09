/* 
 * File:   init.h
 * Author: Jake
 *
 * Created on November 4, 2011, 12:55 PM
 */

#ifndef INIT_H
#define	INIT_H

#ifdef	__cplusplus
extern "C" {
#endif

int initialize_data();
void initialize_IT();
int initialize_user_process();
int initialize_iprocess();
int init_keyboard_process();
int init_crt_process();


#ifdef	__cplusplus
}
#endif

#endif	/* INIT_H */

