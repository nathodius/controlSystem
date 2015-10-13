#include "debug.h"
int lastTime;
int msTime;
int sensor_lastTime;
int sensor_msTime;

///*
void debugCharInit()
{
	PLIB_PORTS_DirectionOutputSet(PORTS_ID_0, PORT_CHANNEL_E, 0x00FF);
}

//output character
void debugChar(int PinByteSel)
{
	PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_E, (PinByteSel & 0x00FF));
}
//*/

//USART0 Driver is connected to USART ID 2
void initDebugU()
{
	DRV_USART0_Initialize();
}

void debugU(char* debugMessage)
{
	int i = 0;
	char getChar = ' ';
	while(i < debugMessage[i] != 0)
	{
		getChar = debugMessage[i];
		DRV_USART0_WriteByte(getChar);
		i++;
	}		
//	DRV_USART0_WriteByte('\r');
}

void debugUChar(char theChar)
{
	DRV_USART0_WriteByte(theChar);
	DRV_USART0_WriteByte('\r');	
}

void debugUInt(int number)
{
	char str[12];
	sprintf(str, "%d", number);
	debugU(str);
	DRV_USART0_WriteByte('\r');
}

void crash(char* debugMessage)
{
	debugU("CRASHED: ");
	debugU(debugMessage);
}

void debugTimerInit()
{
	DRV_TMR0_Initialize();	
	DRV_TMR0_CounterClear();
	DRV_TMR0_Start();
	lastTime = 0;
	msTime = 0;
}

void debugTimerTick()
{
	msTime++;
}


int debugGetTime()
{
	lastTime = msTime;
	msTime = 0;
	return lastTime;
}

void sensor_debugTimerInit()
{
	DRV_TMR0_Initialize();	
	DRV_TMR0_CounterClear();
	DRV_TMR0_Start();
	sensor_lastTime = 0;
	sensor_msTime = 0;
}

void sensor_debugTimerTick()
{
	sensor_msTime++;
}


int sensor_debugGetTime()
{
	sensor_lastTime = sensor_msTime;
	sensor_msTime = 0;
	return sensor_lastTime;
}