/***************************************************************************************
                              a v r.c

                                                                   қuran dez 2023
**************************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
// #include <util/delay.h>

#define TIMER_START_VALUE_0     0x80
#define COUNT_NR                20000
#define TRUE                    1
#define FALSE                   0

#define ON_BOARD_LED            0x20   /*..x. ....*/

volatile int flag;
void delay(int t);

int main(void)
{
    //DDRB = (1 << DDB5);
    DDRB = 0xff;
    flag = FALSE;

    TIMSK0 = (1 << TOIE0);
	TCNT0  =  TIMER_START_VALUE_0;
	TCCR0B = (1 << CS00);
	TCCR0B = (1 << CS01);

    sei();
    
    while(1)
    {
//        PORTB = 0xff; 
//        _delay_ms(500);millis ist nicht vorhanden! 
//        PORTB = 0;
//        _delay_ms(500);

        if (flag == TRUE)
        {
            flag = FALSE; 
            PORTB ^= ON_BOARD_LED; // toggle 
        }

    }
    return 0;
}

ISR(TIMER0_OVF_vect)
{
    static int count = 0; 
    TCNT0 = TIMER_START_VALUE_0;

    count++; 

    if (count > COUNT_NR)
    {
        count = 0;
        flag = TRUE; 
    }
}


void delay(int t)
{
    for(t = 0; t < 200; t++) {asm volatile ("nop"); }
}