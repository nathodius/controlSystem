#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"
#include "communication_public.h"
//#include "test.h"

#define TEST

typedef struct
{
	unsigned char rxBuffer[10];
    unsigned char TxBuffer[10];
    unsigned int count;
} COMMUNICATION_TEST;

/*
typedef struct
{
	char seqNum;
	char type;
	int command;
	int duration;
} COMMUNICATION_MESSAGE;
//*/

typedef struct
{
	int type;
	unsigned char msg;
} COMMUNICATION_MESSAGE;


typedef enum
{
	/* Application's state machine's initial state. */
	COMMUNICATION_STATE_INIT=0,
	COMMUNICATION_STATE_RECEIVE=1,
	COMMUNICATION_STATE_LEFT=2,
    COMMUNICATION_STATE_RIGHT=3,
    
} COMMUNICATION_STATES;


typedef struct
{
    COMMUNICATION_STATES state;
	unsigned char rxChar;
	QueueHandle_t theQueue;
	QueueHandle_t IntQueue;
	char TxMsgSeq;
	char RxMsgSeq;
	int IntTxMsgSeq;
	int IntRxMsgSeq;
	int txField1;
	int txField2;
	unsigned char rxBuffer[10];
	int rxByteCount;
	COMMUNICATION_MESSAGE rxMessage;
	COMMUNICATION_MESSAGE txMessage;
} COMMUNICATION_DATA;


void communication_UartTx(char* string);
void COMMUNICATION_Initialize ( void );
void communication_UartTxChar( char theChar );
char IntToChar(int theInt);
int CharToInt(char theChar);

void COMMUNICATION_Tasks( void );


#endif /* _APP_H */
