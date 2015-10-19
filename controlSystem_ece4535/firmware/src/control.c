#define CONTROLQUEUESIZE 30
#define STARTBYTE 0x80

/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    control.c

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

#include "control.h"

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

CONTROL_DATA controlData;

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


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void CONTROL_Initialize ( void )

  Remarks:
    See prototype in control.h.
 */

void CONTROL_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    controlData.state = CONTROL_STATE_INIT;
    controlData.rxByteCount = 0;
    controlData.RxMsgSeq = 0x00;
    controlData.theQueue = xQueueCreate(CONTROLQUEUESIZE, sizeof(CONTROL_MESSAGE)); //sizeof(communicationData.rxMessage));
    
    if(controlData.theQueue == 0)
	{
		crash("\nE: Comm msgQ");
	}
}


/******************************************************************************
  Function:
    void CONTROL_Tasks ( void )

  Remarks:
    See prototype in control.h.
 */

void CONTROL_Tasks ( void )
{
    while(1)
	{
        //check if queue exists
        if(controlData.theQueue != 0)	
        {
            //receive a message and store into rxMessage ... block 5 ticks if empty queue
            if(xQueuePeek(controlData.theQueue, (void*)&(controlData.rxMessage), portMAX_DELAY ))
            {
                xQueueReceive(controlData.theQueue, (void*)&(controlData.rxMessage), portMAX_DELAY);
                //received messageQ message will tell use the state which will be used
                controlData.state = controlData.rxMessage.type;
                //communicationData.state = 1;
                //run the state according to the message's state
                switch ( controlData.state )
                {
                    /* Application's initial state. */
                    case COMMUNICATION_STATE_INIT:	//0
                        break;

                    case COMMUNICATION_STATE_RECEIVE:	//1
                        if(controlData.rxMessage.msg == STARTBYTE)	//received start transmit bit
                        {
                            if(controlData.rxByteCount > 0)	//we had part of a previous message...
                            {
                                debugU("mixed messages sent to control thread\n");
                                //NACK;
                                controlData.rxByteCount = 0;
                            }
                            controlData.rxBuffer[0] = controlData.rxMessage.msg;
                            controlData.rxByteCount = 1;
                        }
                        else if( controlData.rxByteCount > 0)	//continuing message
                        {
                            controlData.rxBuffer[controlData.rxByteCount] = controlData.rxMessage.msg;
                            controlData.rxByteCount++;
                            if(controlData.rxByteCount == 10)
                            {
                                //check start byte
                                if(controlData.rxBuffer[0] == STARTBYTE)
                                {
                                    //###CHECK seq number
                                    int i = 0;
                                    while(controlData.rxBuffer[1] != controlData.RxMsgSeq)
                                    {
                                        controlData.RxMsgSeq++;
                                        debugU("control thread: Lost a seq number \t");
                                        debugUInt(controlData.RxMsgSeq);
                                        i++;
                                        if(i == 10)
                                        {
                                            crash("E: COM too many lost rx seqnum\n");
                                        }
                                    }

                                    int field1 = 0;
                                    field1 += CharToInt(controlData.rxBuffer[2]) * 1000;
                                    field1 += CharToInt(controlData.rxBuffer[3]) * 100;
                                    field1 += CharToInt(controlData.rxBuffer[4]) * 10;
                                    field1 += CharToInt(controlData.rxBuffer[5]);
                                    int field2 = 0;
                                    field2 += CharToInt(controlData.rxBuffer[6]) * 1000;
                                    field2 += CharToInt(controlData.rxBuffer[7]) * 100;
                                    field2 += CharToInt(controlData.rxBuffer[8]) * 10;
                                    field2 += CharToInt(controlData.rxBuffer[9]);

                                    //reset
                                    controlData.rxByteCount = 0;
                                    controlData.RxMsgSeq++;
                                    if(controlData.RxMsgSeq == 0x7F)
                                        controlData.RxMsgSeq = 0x00;
                                    controlData.state = 0;
                                } // end start byte check
                            } // end byte count check
                        }
                        else	//failed to receive start bit and catch all... nack and reset message
                        {
                            //NACK
                            debugU("\nrover NACK");
                            controlData.rxByteCount = 0;	//no start byte and bytecount != 0, so unknown char drop it
                        }
                        break;
                    default:
                    {
                        /* TODO: Handle error in application's state machine. */
                        break;
                    }
                }
            }
        }
        else	//attempt to create the queue again if it doesn't exist
        {
            //something terrible happens normally so have it exit rather than recreate
            crash("E: No Comm Q");
            //run exit interrupt to OS... or forever loop message
            return;
            //communicationData.theQueue = xQueueCreate(10, sizeof(APP_STATES));
        }
    }
}
 

/*******************************************************************************
 End of File
 */
