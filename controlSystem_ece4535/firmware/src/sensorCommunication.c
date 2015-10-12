/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    sensorcommunication.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "sensorcommunication.h"
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

#define COMMUNICATIONQUEUESIZE 15
#define STARTBYTE 0x80

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

SENSORCOMMUNICATION_DATA sensorcommunicationData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback funtions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary local functions.
*/

void sensorcommunication_sendmsg(unsigned char msg, int type)
{
	COMMUNICATION_MESSAGE theMsg;
	theMsg.msg = msg;
	theMsg.type = type;
	if(sensorcommunicationData.sensortheQueue != 0)
	{
		xQueueSend(sensorcommunicationData.sensortheQueue, (void*)&(theMsg), 0);
	}
}
void sensorcommunication_sendmsgISR(unsigned char msg, int type)
{
	COMMUNICATION_MESSAGE theMsg;
	theMsg.msg = msg;
	theMsg.type = type;
	if(sensorcommunicationData.sensortheQueue != 0)
	{
		xQueueSendFromISR(sensorcommunicationData.sensortheQueue, (void*)&(theMsg), 0);
	}
}

//void sensorcommunication_send(int left, int right)
//{
//	sensorcommunicationData.txField1 = left;
//	sensorcommunicationData.txField2 = right;
//}

void sensorcommunication_sendIntMsg(int left, int right)
{
	SENSORCOMMUNICATION_MESSAGE theMessage;
	//send message sequence byte
	theMessage.type = sensorcommunicationData.sensorIntTxMsgSeq;
	theMessage.msg = STARTBYTE;
	if(xQueueSend(sensorcommunicationData.sensorIntQueue, (void*)&(theMessage), 0) == pdTRUE)
	{
		sensorcommunicationData.sensorIntTxMsgSeq++;
	}
	//send from rover byte
	theMessage.type = sensorcommunicationData.sensorIntTxMsgSeq;
	theMessage.msg = sensorcommunicationData.sensorTxMsgSeq;	
	if(xQueueSend(sensorcommunicationData.sensorIntQueue, (void*)&(theMessage), 0) == pdTRUE)
	{
		sensorcommunicationData.sensorIntTxMsgSeq++;
	}
	//send left 1 of 4
	if(left > 9999)
		left = 9999;
	if(right > 9999)
		right = 9999;
	int leftxmit[4];
	int rightxmit[4];
	leftxmit[3] = left % 10;
	left /= 10;
	leftxmit[2] = left % 10;
	left /= 10;
	leftxmit[1] = left % 10;
	left /= 10;
	leftxmit[0] = left % 10;
	rightxmit[3] = right%10;
	right /= 10;
	rightxmit[2] = right%10;
	right /= 10;
	rightxmit[1] = right%10;
	right /= 10;
	rightxmit[0] = right%10;
	theMessage.type = sensorcommunicationData.sensorIntTxMsgSeq;
	theMessage.msg = IntToChar(leftxmit[0]);	
	if(xQueueSend(sensorcommunicationData.sensorIntQueue, (void*)&(theMessage), 0) == pdTRUE)
	{
		sensorcommunicationData.sensorIntTxMsgSeq++;
	}
	//send left 2 of 4
	theMessage.type = sensorcommunicationData.sensorIntTxMsgSeq;
	theMessage.msg = IntToChar(leftxmit[1]);
	if(xQueueSend(sensorcommunicationData.sensorIntQueue, (void*)&(theMessage), 0) == pdTRUE)
	{
		sensorcommunicationData.sensorIntTxMsgSeq++;
	}
	//send left 3 of 4
	theMessage.type = sensorcommunicationData.sensorIntTxMsgSeq;
	theMessage.msg = IntToChar(leftxmit[2]);	
	if(xQueueSend(sensorcommunicationData.sensorIntQueue, (void*)&(theMessage), 0) == pdTRUE)
	{
		sensorcommunicationData.sensorIntTxMsgSeq++;
	}
	//send 4 of 4
	theMessage.type = sensorcommunicationData.sensorIntTxMsgSeq;
	theMessage.msg = IntToChar(leftxmit[3]);	
	if(xQueueSend(sensorcommunicationData.sensorIntQueue, (void*)&(theMessage), 0) == pdTRUE)
	{
		sensorcommunicationData.sensorIntTxMsgSeq++;
	}
	//send right 1 of 4
	theMessage.type = sensorcommunicationData.sensorIntTxMsgSeq;
	theMessage.msg = IntToChar(rightxmit[0]);	
	if(xQueueSend(sensorcommunicationData.sensorIntQueue, (void*)&(theMessage), 0) == pdTRUE)
	{
		sensorcommunicationData.sensorIntTxMsgSeq++;
	}
	//send right 2 of 4
	theMessage.type = sensorcommunicationData.sensorIntTxMsgSeq;
	theMessage.msg = IntToChar(rightxmit[1]);	
	if(xQueueSend(sensorcommunicationData.sensorIntQueue, (void*)&(theMessage), 0) == pdTRUE)
	{
		sensorcommunicationData.sensorIntTxMsgSeq++;
	}
	//send right 3 of 4
	theMessage.type = sensorcommunicationData.sensorIntTxMsgSeq;
	theMessage.msg = IntToChar(rightxmit[2]);		
	if(xQueueSend(sensorcommunicationData.sensorIntQueue, (void*)&(theMessage), 0) == pdTRUE)
	{
		sensorcommunicationData.sensorIntTxMsgSeq++;
	}
	//send right 4 of 4
	theMessage.type = sensorcommunicationData.sensorIntTxMsgSeq;
	theMessage.msg = IntToChar(rightxmit[3]);		
	if(xQueueSend(sensorcommunicationData.sensorIntQueue, (void*)&(theMessage), 0) == pdTRUE)
	{
		sensorcommunicationData.sensorIntTxMsgSeq++;
	}
	sensorcommunicationData.sensorTxMsgSeq++;
	int temp = (int)sensorcommunicationData.sensorTxMsgSeq;
	if(sensorcommunicationData.sensorTxMsgSeq == 0x7F)
		sensorcommunicationData.sensorTxMsgSeq = 0x00;
	PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_3_TRANSMIT);	//ENABLE TX INTERRUPT
}

unsigned char sensorcommunication_getByteISR()
{
	COMMUNICATION_MESSAGE theMessage;
	if(xQueueReceiveFromISR(sensorcommunicationData.sensorIntQueue, (void*)&(theMessage), 0))
	{
		int i = 0;
		if( theMessage.type == sensorcommunicationData.sensorIntRxMsgSeq)
		{
			sensorcommunicationData.sensorRxMsgSeq++;
			return theMessage.msg;
		}
		else
		{
			while( theMessage.type != sensorcommunicationData.sensorIntRxMsgSeq)
			{
				i++;																		
				if(i == COMMUNICATIONQUEUESIZE)
				{
					xQueueReset(sensorcommunicationData.sensorIntQueue);	//clear queue. we dropped the packet
				}
				xQueueSendToBackFromISR(sensorcommunicationData.sensorIntQueue, (void*)&(theMessage), 0);	//send to back
				xQueueReceiveFromISR(sensorcommunicationData.sensorIntQueue, (void*)&(theMessage), 0 );	//get another
			}
			debugU("COM message loaded\n");
			return theMessage.msg;
		}
	}
}

bool sensorcommunication_sensorIntQueueEmptyISR()
{
	BaseType_t retval;
	retval = xQueueIsQueueEmptyFromISR(sensorcommunicationData.sensorIntQueue);
	if(retval == pdTRUE)
		return true;
	else
		return false;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void SENSORCOMMUNICATION_Initialize ( void )

  Remarks:
    See prototype in sensorcommunication.h.
 */

void SENSORCOMMUNICATION_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
	//initDebugU();
    sensorcommunicationData.state = SENSORCOMMUNICATION_STATE_INIT;
	sensorcommunicationData.sensorrxByteCount = 0;
	sensorcommunicationData.sensorTxMsgSeq = 0x00;
	sensorcommunicationData.sensorRxMsgSeq = 0x00;
	sensorcommunicationData.sensortheQueue = xQueueCreate(COMMUNICATIONQUEUESIZE, sizeof(COMMUNICATION_MESSAGE)); //sizeof(communicationData.rxMessage));
	if(sensorcommunicationData.sensortheQueue == 0)
	{
		crash("\nE: Comm msgQ");
	}
	sensorcommunicationData.sensorIntQueue = xQueueCreate(COMMUNICATIONQUEUESIZE, sizeof(COMMUNICATION_MESSAGE)); //sizeof(communicationData.rxMessage));
	if(sensorcommunicationData.sensorIntQueue == 0)
	{
		crash("\nE: Int msgQ");
	}
	DRV_USART2_Initialize();
}


/******************************************************************************
  Function:
    void SENSORCOMMUNICATION_Tasks ( void )

  Remarks:
    See prototype in sensorcommunication.h.
 */

void SENSORCOMMUNICATION_Tasks ( void )
{
	while(1)
	{
		//check if queue exists
		if(sensorcommunicationData.sensortheQueue != 0)	
		{
               
			//receive a message and store into rxMessage ... block 5 ticks if empty queue
			if(xQueuePeek(sensorcommunicationData.sensortheQueue, (void*)&(sensorcommunicationData.rxMessage), portMAX_DELAY ))
			{
				xQueueReceive(sensorcommunicationData.sensortheQueue, (void*)&(sensorcommunicationData.rxMessage), portMAX_DELAY);
				//received messageQ message will tell use the state which will be used
				sensorcommunicationData.state = sensorcommunicationData.rxMessage.type;
				//communicationData.state = 1;
				
				//run the state according to the message's state
				switch ( sensorcommunicationData.state )
				{
					/* Application's initial state. */
					case SENSORCOMMUNICATION_STATE_INIT:	//0
						break;

					case SENSORCOMMUNICATION_STATE_RECEIVE:	//1
						//debugU("COM rx: ");
						//debugUInt(communicationData.rxMessage.msg);
						if(sensorcommunicationData.rxMessage.msg == STARTBYTE)	//received start transmit bit
						{
							if(sensorcommunicationData.sensorrxByteCount > 0)	//we had part of a previous message...
							{
								debugU("NACK");
								//NACK;
							}
							sensorcommunicationData.sensorrxBuffer[0] = sensorcommunicationData.rxMessage.msg;
							sensorcommunicationData.sensorrxByteCount = 1;	
						}
						else if( sensorcommunicationData.sensorrxByteCount > 0)	//continuing message
						{
							sensorcommunicationData.sensorrxBuffer[sensorcommunicationData.sensorrxByteCount] = sensorcommunicationData.rxMessage.msg;
							sensorcommunicationData.sensorrxByteCount++;
							if(sensorcommunicationData.sensorrxByteCount == 10)
							{
								//check start byte
								if(sensorcommunicationData.sensorrxBuffer[0] == STARTBYTE)
								{
									//###CHECK seq number
									int i = 0;
									while(sensorcommunicationData.sensorrxBuffer[1] != sensorcommunicationData.sensorRxMsgSeq)
									{
										sensorcommunicationData.sensorRxMsgSeq++;
                                        sensorcommunicationData.msgErr++;
										debugU("Lost a seq number\r # dropped: ");
                                        debugUInt(sensorcommunicationData.msgErr);
										i++;
										if(i == 10)
										{
											crash("E: COM too many lost rx seqnum");
										}
									}
									int x = 0;
									x += CharToInt(sensorcommunicationData.sensorrxBuffer[2]) * 1000;
									x += CharToInt(sensorcommunicationData.sensorrxBuffer[3]) * 100;
									x += CharToInt(sensorcommunicationData.sensorrxBuffer[4]) * 10;
									x += CharToInt(sensorcommunicationData.sensorrxBuffer[5]);
									int y = 0;
									y += CharToInt(sensorcommunicationData.sensorrxBuffer[6]) * 1000;
									y += CharToInt(sensorcommunicationData.sensorrxBuffer[7]) * 100;
									y += CharToInt(sensorcommunicationData.sensorrxBuffer[8]) * 10;
									y += CharToInt(sensorcommunicationData.sensorrxBuffer[9]);

									debugU("Sensor1:");
									char msg[12];
									sprintf(msg, "%d", x);
									debugU(msg);
									debugU("Sensor2:");
									sprintf(msg, "%d", y);
									debugU(msg);
                                    
                                    //communication_sendIntMsg(x,y);
                                    
									sensorcommunicationData.sensorrxByteCount = 0;
									sensorcommunicationData.sensorRxMsgSeq++;
									if(sensorcommunicationData.sensorRxMsgSeq == 0x7F)
                                        sensorcommunicationData.sensorRxMsgSeq = 0x00;
									//ACK
									debugU("\nsensor comm ACK\n");
//#ifdef TEST
//                                    testData.count++; // Increment message count
//                                    int j =0;
//                                    for (j; j<10; j++)
//                                    {
//                                        testData.sensorrxBuffer[j] = sensorcommunicationData.sensorrxBuffer[j];
//                                    }
//                                    // Receive buffer contents recorded.
//                                    
//                                    communication_sendIntMsg(x, y);
//#else
//                                    if(sensorcommunicationData.sensorRxMsgSeq % 4 == 0)
//                                        communication_sendIntMsg(3, 100);
//                                    else if(sensorcommunicationData.sensorRxMsgSeq % 4 == 2)
//                                        communication_sendIntMsg(2, 100);
//#endif
                                    sensorcommunicationData.state = 0;
								}//end if check start byte
							}//end if bytecount == 10
						}//end if bytecount > 0
						else	//failed to receive start bit and catch all... nack and reset message
						{
							//NACK
							debugU("\nsensor NACK");
							sensorcommunicationData.sensorrxByteCount = 0;	//no start byte and bytecount != 0, so unknown char drop it
						}
						break;

					/* The default state should never be executed. */
					default:
					{
						/* TODO: Handle error in application's state machine. */
						break;
					}
				}//end of case
			}//end of receive message
		}//end of check message queue != 0
		else	//attempt to create the queue again if it doesn't exist
		{
			//something terrible happens normally so have it exit rather than recreate
			crash("E: No Comm Q");
			//run exit interrupt to OS... or forever loop message
			return;
			//communicationData.sensortheQueue = xQueueCreate(10, sizeof(APP_STATES));
		}
	}//end of while(1)
}
 

/*******************************************************************************
 End of File
 */
