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

void _wait_64_usec(void);
void delay(int msec);

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

void _wait_64_usec(void)
{
    int i;
    for(i = 0; i < 200; i++) {asm volatile ("nop"); }
}

void delay(int msec)
{
int long x;
    x = msec << 4;

    for (; x > 0 ; x--) _wait_64_usec();
}
