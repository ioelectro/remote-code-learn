/*
    By Liyanboy74
    https://github.com/liyanboy74
*/

#include <mega8.h>
#include <stdio.h>
#include <delay.h>

#include "Timer.h"
#include "EV17xx_Decoder.h"

#define MAX_MEM_SIZE 170

#define LED PORTC.5
#define KEY PINB.0

//Remots Code Bank
eeprom unsigned char My_Remote_Data[MAX_MEM_SIZE][3];

//Remots Index
eeprom int Remote=0;

//Tim1 Overflow
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
}

void main(void)
{
    //Buffer
    unsigned char Code[4];
    int i=0;

    //GPIO Initial
    DDRC = 0x7f;
    DDRB.0=0;
    PORTB.0=1;
    PORTC.4=1;
    PORTC.3=1;

    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: 1000.000 kHz
    // Mode: Normal top=0xFFFF
    // OC1A output: Disconnected
    // OC1B output: Disconnected
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer Period: 65.502 ms
    // Timer1 Overflow Interrupt: On
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off
    TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
    TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
    TCNT1H=0x00;
    TCNT1L=0x22;
    ICR1H=0x00;
    ICR1L=0x00;
    OCR1AH=0x00;
    OCR1AL=0x00;
    OCR1BH=0x00;
    OCR1BL=0x00;

    // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (1<<TOIE1) | (0<<TOIE0);

    // External Interrupt(s) initialization
    // INT0: On
    // INT0 Mode: Rising Edge
    // INT1: Off
    GICR|=(0<<INT1) | (1<<INT0);
    MCUCR=(0<<ISC11) | (0<<ISC10) | (1<<ISC01) | (1<<ISC00);
    GIFR=(0<<INTF1) | (1<<INTF0);

    // USART initialization
    // Communication Parameters: 8 Data, 1 Stop, No Parity
    // USART Receiver: On
    // USART Transmitter: On
    // USART Mode: Asynchronous
    // USART Baud Rate: 9600
    UCSRA=(0<<RXC) | (0<<TXC) | (0<<UDRE) | (0<<FE) | (0<<DOR) | (0<<UPE) | (0<<U2X) | (0<<MPCM);
    UCSRB=(0<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (1<<RXEN) | (1<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);
    UCSRC=(1<<URSEL) | (0<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<UCPOL);
    UBRRH=0x00;
    UBRRL=0x33;

    //Debug
    puts("Runing...\r\n");

    //if EEPROM file not programmed on chip!
    if(Remote<0)Remote=0;

    // Global enable interrupts
    #asm("sei")

    while(1)
    {
        //Continue Process for find Valid Remote Codes  
        if(IS_Recive_Valid_Remote())
        {
            //Read Code
            Get_Remote_Code(Code); 

            //Debug - Print Code  
            for( i=0;i<24;i++)
                putchar(Remode_Data[i]+48);
            puts("\r\n");
            printf("%X%X%X\r\n",Code[0],Code[1],Code[2]&0xf0);  

            //Search Code       
            for(i=0;i<Remote;i++)
            {
                //Compare codes  
                if(My_Remote_Data[i][0]==Code[0]&&My_Remote_Data[i][1]==Code[1]&&(My_Remote_Data[i][2]&0xf0)==(Code[2]&0xf0))
                {
                    //Debug 
                    printf("Remot Matched! Your ID is: %d\r\n",i+1);

                    //Triger Output
                    switch(Code[2]&0xf)
                    {
                        case 0b0001:PORTC.4=0;puts("OUT1 = ON\r\n");break; 
                        case 0b0010:PORTC.4=1;puts("OUT1 = OFF\r\n");break;
                        case 0b0100:PORTC.3=0;puts("OUT2 = ON\r\n");break;
                        case 0b1000:PORTC.3=1;puts("OUT2 = OFF\r\n");break;          
                    }

                    //Index  
                    i=Remote;

                    //Blink 
                    LED=1;
                    delay_ms(200);
                    LED=0; 
                } 
            }   
            Revice_Flag=0;      
        } 

        //Button Process           
        if(KEY==0)
        {
            //LED ON
            PORTC.5=1;

            //Debug Print 
            puts("Press any key to save addres ,Or Hold Key for 5S to Delete All Remot:\r\n");  

            //delay 10*500 ms 
            for(i=0;i<10;i++)
            { 
                if(IS_Recive_Valid_Remote())
                {
                    //Process for find Valid Remote Codes   
                    Get_Remote_Code(Code); 

                    //Check memory Size 
                    if(Remote<MAX_MEM_SIZE)
                    {
                        //Save Codes to EEPROM at `Remote` Index
                        My_Remote_Data[Remote][0]=Code[0];    
                        My_Remote_Data[Remote][1]=Code[1];
                        My_Remote_Data[Remote][2]=Code[2];  

                        //Add Index
                        Remote++;

                        //Debug Print
                        printf("Remot Adress (%X%X%X) Saved At ID (%d)\r\n",Code[0],Code[1],Code[2]&0xf0,Remote);

                        //Blink LED
                        LED=1;
                        delay_ms(2000);
                        LED=0;
                    } 
                    else
                    {
                        //Debug Print
                        puts("Memo is full!\r\n"); 

                        //Blink LED  
                        LED=1;
                        delay_ms(1000);
                        LED=0;
                    }

                    //Break 
                    i=10;
                }
                delay_ms(500);

                //Toggle LED
                LED=~LED;
            }  

            //if key was pressed , remove all stored codes!       
            if(KEY==0)
            {
                for(i=0;i<MAX_MEM_SIZE;i++)
                {
                    My_Remote_Data[i][0]=0;    
                    My_Remote_Data[i][1]=0;
                    My_Remote_Data[i][2]=0;

                    //Toggle LED 
                    LED=~LED; 
                    delay_ms(20); 
                }
                Remote=0; 
                puts("All Remot Removed!\r\n");  
            }

            LED=0;  
            delay_ms(500);
            Revice_Flag=0; 
        }
        delay_ms(10);
    }
}
