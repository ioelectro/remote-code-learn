/*
 * Timer.h
 *
 *  Created on: Jan 17, 2014
 *      Author: Mohammad.Mazarei@gmail.com
 *      Web	  :	www.Agape.ir
 */

#include <mega8.h>
unsigned int Stop_Read_timer(void);
//unsigned int  Read_timer(void);
//void Stop_Timer(void);
void Start_Timer(void);


//------------------------------------------

void Start_Timer(void)
{
    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: 1000.000 kHz
    // Mode: Normal top=0xFFFF
    TCNT1H=0x00;
    TCNT1L=0x00;
    TCCR1A=0x00;
    TCCR1B=0x02;
}


/*void Stop_Timer(void)
{
    TCCR1B=0x00;
} */


/*unsigned int Read_timer(void)
{
       unsigned char sreg;
       unsigned int val;

        sreg = SREG;
        #asm("cli")
        val = TCNT1;
        SREG = sreg;

        return val;
} */

unsigned int Stop_Read_timer(void)
{
        unsigned char sreg;
         unsigned int val;
        TCCR1B=0x00;
        sreg = SREG;
        #asm("cli")
        val = TCNT1;
        SREG = sreg;

        return val;
}

