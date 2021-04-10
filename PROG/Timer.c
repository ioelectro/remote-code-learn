/*
Copyright (C) Mohammad Mazarei Zeus @ Sisoog.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  
02111-1307, USA.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <Timer.h>



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


void Stop_Timer(void)
{
	TCCR1B=0x00;
}


unsigned int Read_timer(void)
{
        unsigned char sreg;
       unsigned int val;

        sreg = SREG;
        cli();
        val = TCNT1;
        SREG = sreg;

        return val;
}

unsigned int Stop_Read_timer(void)
{
        unsigned char sreg;
         unsigned int val;
        TCCR1B=0x00;
        sreg = SREG;
        cli();
        val = TCNT1;
        SREG = sreg;

        return val;
}

