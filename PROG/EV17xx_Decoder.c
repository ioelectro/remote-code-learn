/*
 * EV17xx_Decoder.c
 *
 *  Created on: Jan 17, 2014
 *      Author: Mohammad.Mazarei@gmail.com
 *      Web	  :	www.Agape.ir
 */

#include <mega8.h>
#include "EV17xx_Decoder.h"

#include <avr/interrupt.h>
#include "Timer.h"

#include <stdio.h>

#define IS_Valid_Pulse_Len(T1,T2)		((T1 > Min_Pulse_Len) && (T1 < Max_Pulse_Len) && (T2 < Max_Pulse_Len) && (T2 > Min_Pulse_Len))
#define IS_Sync_Start_Pulse(T1,T2)		(T2 > (T1*25))
#define Bit_IS_Zero(T1,T2)				(T2 > (T1*2))

 unsigned char 		Remode_Data[24],i;
unsigned char	Revice_Flag = 0;


interrupt [EXT_INT0] void ext_int0_isr(void)	// Intrupt Of Micro
{
	static unsigned int	Time_Falling=0;
	static unsigned int	Time_Rising=0;
	static unsigned int	Start_Sync = 0;
	static unsigned char	Bit_Index = 0;

	if(MCUCR==0x03) // Rising Edge
	{
		MCUCR=0x02; // Make As Falling Edge

			Time_Falling =Stop_Read_timer();

			if(IS_Valid_Pulse_Len(Time_Rising,Time_Falling)) /*If Valid Pulse Len*/
			{

				if(IS_Sync_Start_Pulse(Time_Rising,Time_Falling))
				{
					Start_Sync = 1;

				}
				else
				{
					if(Start_Sync==1)	// Start Sended
					{
						if(Bit_Index < 24)
						{
							Remode_Data[Bit_Index] = !Bit_IS_Zero(Time_Rising,Time_Falling);
							Bit_Index++;
						}
						else
						{	// All Bit Recive
							Bit_Index = 0;
							Start_Sync = 0;
							Revice_Flag = 1;

						}
					}	// End of Start Sync Send
				}

			}
			else	// Not Valid Pulse Len
			{
				Start_Sync = 0;
				Bit_Index = 0;
				//printf("No Sync:R->%i,F->%i\r",Time_Rising,Time_Falling);
			}
			Start_Timer();
	}
	else	//Falling Edge
	{
		MCUCR=0x03; // Make As Rising Edge
		Time_Rising = Stop_Read_timer() ;
		Start_Timer();
	}
}


unsigned char	IS_Recive_Valid_Remote(void)
{
	return (Revice_Flag==1);
}

void Get_Remote_Code(unsigned char *Code)
{
	Code[0] = Code[1]= Code[2] = 0;

	for( i=0;i<24;i++)
	{
		Code[i/8] |= Remode_Data[i]<<(7-(i%8));
	}

	Revice_Flag = 0;
}
