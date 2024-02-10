/***************************************************************************************
                                TimerInterrupt 3xHELS
                                                                      қuran feb 2024
**************************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>

#define TRUE                           1
#define FALSE                          0
#define HIGH                           1
#define LOW                            0
#define ON_BOARD_LED                   0x20   // PORT B:    ..x. .... 
#define KEY                            0x04   // PORT D:    .... .x..
#define TIMER_START_VALUE              0x80
#define TEN_MSEC                       151
#define ONE_SEC                        100    // 1 sec = ONE_SEC x 10 msec
#define BOUD_RATE_9600                 103



volatile int flag;
volatile int flagOneSec;
volatile char recChar;
void serial_send(char c);
void writeText(const char * s);

int main(void)
{
    // Setup:
    DDRD = DDRB = 0xff;
    DDRD &= ~KEY;
    PORTD |= KEY;
    flag = flagOneSec = FALSE;

    // Timer Interrupt:
    TIMSK0 = (1 << TOIE0);
	TCCR0B = (1 << CS00);
	TCCR0B = (1 << CS01);
	TCNT0  =  TIMER_START_VALUE;

    // UART:  
    UCSR0B = (1 << RXCIE0) | (1 << RXEN0)  | (1 << TXEN0); // frame format: 8 data, 
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Use 8-bit character sizes
    UBRR0H = 0;
    UBRR0L = BOUD_RATE_9600;


    sei();


    serial_send('s');
    serial_send('t');
    serial_send('a');
    serial_send('r');
    serial_send('t');
    serial_send('!');
 
    // endless Loop: 
    for(;;)
    {
        if (flag)
        {
            flag = FALSE;
            PORTB ^= ON_BOARD_LED; // toggle 
        }

        if (flagOneSec)
        {
            flagOneSec = FALSE; 
            writeText("Elektronik und Technische Informatik ist super!!!"); 
        }
    }
    
    return 0;  
}

void serial_send(char c)
{
     while ((UCSR0A & (1<<UDRE0))==0);  // wait as long, as the last sending is finished!
     UDR0 = c;
}

void writeText(const char * s)
{
    const char * p = s; 

    while (*p != '\0') 
    {
        serial_send(*p);
        p++;
    }
}



ISR(TIMER0_OVF_vect)
{
    static int count = 0; 
    static int countOneSec = 0;
    static char lastKey = HIGH, newKey = HIGH;

    TCNT0 = TIMER_START_VALUE;
    count++;
    
    if (count > TEN_MSEC) 
    {
        count = 0;
        countOneSec++;

        newKey = PIND & KEY;  

        if ((newKey == LOW) && (lastKey != LOW)) // left slope found!
        {
            flag = TRUE;
        }

        lastKey = newKey;
    }

    if (countOneSec >= ONE_SEC)
    {
        countOneSec = 0; 
        flagOneSec = TRUE; 
    }

}


ISR(USART_TX_vect) // Senden abgeschlossen
{
    // do nothing
}

ISR(USART_RX_vect) // Empfang abgeschlossen
{
    recChar = UDR0;
    if (recChar == 'x')   flag = TRUE;
}


/*
Falls es jemand wagen sollte hier ein delay einbauen zu wollen, indem er ganz einfach das Include:

#include <util/delay.h>

einbaut, dann habe ich zumindest noch diese beiden Funktionen für ihn vorbereitet!

Besser selbst geschrieben ..... 

Hier:

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

wehe, das verwendet jemand!!! 
:-)

*/