/*******************************************************************************
 System Interrupts File

  File Name:
    system_int.c

  Summary:
    Raw ISR definitions.

  Description:
    This file contains a definitions of the raw ISRs required to support the
    interrupt sub-system.

  Summary:
    This file contains source code for the interrupt vector functions in the
    system.

  Description:
    This file contains source code for the interrupt vector functions in the
    system.  It implements the system and part specific vector "stub" functions
    from which the individual "Tasks" functions are called for any modules
    executing interrupt-driven in the MPLAB Harmony system.

  Remarks:
    This file requires access to the systemObjects global data structure that
    contains the object handles to all MPLAB Harmony module objects executing
    interrupt-driven in the system.  These handles are passed into the individual
    module "Tasks" functions to identify the instance of the module to maintain.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2011-2014 released Microchip Technology Inc.  All rights reserved.

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

#include <xc.h>
#include <sys/attribs.h>
#include "communication.h"
#include "motor.h"
#include "sensorcommunication.h"
#include "system_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: System Interrupt Vector Functions
// *****************************************************************************
// *****************************************************************************

void IntHandlerDrvTmrInstance0(void)

{
    debugTimerTick();
    sensor_debugTimerTick();
    PLIB_TMR_Counter16BitClear(TMR_ID_5);
    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_5);
}
 
//IntHandlerDrvTmrInstance1(void)
//
//{
//    sensor_debugTimerTick();
//    PLIB_TMR_Counter16BitClear(TMR_ID_4);
//    PLIB_INT_SourceFlagClear(INT_ID_0,INT_SOURCE_TIMER_4);
//}
 
void IntHandlerDrvUsartInstance0(void)
{

    /* TODO: Add code to process interrupt here */

    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_2_TRANSMIT);
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_2_RECEIVE);
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_2_ERROR);

}
 
 
 


void IntHandlerDrvUsartInstance1(void)
{

    /* TODO: Add code to process interrupt here */
	if(PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_RECEIVE))
	{
		while(!DRV_USART1_ReceiverBufferIsEmpty()) //grab everyhting in the buffer
		{
			unsigned char msg = DRV_USART1_ReadByte(); // read received byte
			sensorcommunication_sendmsgISR(msg,1);
		}
	}
	if(PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT))
	{
 		//check if queue is not empty first...
		while(!sensorcommunication_sensorIntQueueEmptyISR())
		{
			unsigned char txChar;
			txChar = sensorcommunication_getByteISR();
			DRV_USART1_WriteByte(txChar);
            debugU("transmit interrupt...");
       
#ifdef DEBUG
			debugU("COM tx: ");
			debugUInt(txChar);
#endif
		}

		PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);	//disable int due to empty xmit
	}
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_ERROR);

}
 
 
 
// rover
void IntHandlerDrvUsartInstance2(void)
{

    /* TODO: Add code to process interrupt here */
	if(PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_4_RECEIVE))
	{
		while(!DRV_USART2_ReceiverBufferIsEmpty()) //grab everyhting in the buffer
		{
			unsigned char msg = DRV_USART2_ReadByte(); // read received byte
            //debugUInt(msg);
			communication_sendmsgISR(msg,1);
		}
	}
	if(PLIB_INT_SourceFlagGet(INT_ID_0, INT_SOURCE_USART_4_TRANSMIT))
	{
		//check if queue is not empty first...
		while(!communication_IntQueueEmptyISR())
		{
			unsigned char txChar;
			txChar = communication_getByteISR();
			DRV_USART2_WriteByte(txChar);
            PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_4_TRANSMIT);	//disable int due to empty xmit
        }
    /* Clear pending interrupt */
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_4_TRANSMIT);
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_4_RECEIVE);
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_4_ERROR);
    }
}
 
 
 

 

 
 
  
/*******************************************************************************
 End of File
*/

