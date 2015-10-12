/* 
 * File:   sensorCommunication_public.h
 * Author: nathanielhughes
 *
 * Created on October 11, 2015, 10:52 PM
 */

#ifndef SENSORCOMMUNICATION_PUBLIC_H
#define	SENSORCOMMUNICATION_PUBLIC_H

#ifdef	__cplusplus
extern "C" {
#endif

void sensorcommunication_sendmsg(unsigned char msg, int type);
void sensorcommunication_sendmsgISR(unsigned char msg, int type);
void sensorcommunication_incrementTxMsgSeq();

void sensorcommunication_sendIntMsg(int left, int right);
unsigned char sensorcommunication_getByteISR();
bool sensorcommunication_IntQueueEmptyISR();


#ifdef	__cplusplus
}
#endif

#endif	/* SENSORCOMMUNICATION_PUBLIC_H */

