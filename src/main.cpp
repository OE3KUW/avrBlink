/***************************************************************************************
                              a v r.c

                                                                   қuran dez 2022
**************************************************************************************/
// 328p16m_programmer meldet häufig Fehler - doch das PGM ist korrekt im ATmega ... 
#include <avr/io.h>
#include <avr/interrupt.h>
// #include <util/delay.h>

#define TIMER_START_VALUE_0     0x80
#define COUNT_NR                20000
#define TRUE                    1
#define FALSE                   0

volatile int flag;

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
//        _delay_ms(500);
//        PORTB = 0;
//        _delay_ms(500);

        if (flag == TRUE)
        {
            flag = FALSE; 

            PORTB ^= 0xff; // toggle 
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
