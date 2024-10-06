/***************************************************************************************
                                a v r   B l i n k
                                                                      қuran oct 2024
**************************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>

#define TRUE                           1
#define FALSE                          0
#define HIGH                           1
#define LOW                            0
#define ON_BOARD_LED                   0x20   // PORT B:    ..x. .... 
#define KEY                            0x01   // PORT B:    .... ...x
#define TIMER_START_VALUE              0x80
#define TEN_MSEC                       19 // 151
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
    DDRB = 0xff;
    DDRB &= ~KEY;
    PORTB |= KEY; // internal pull-up-resistor
    DDRD = 0xfe;  // PORTD-0 = 0 to read! to receive serial commands!
    flag = flagOneSec = FALSE;

    // Timer Interrupt:
    TIMSK0 = (1 << TOIE0);
	TCCR0B = (1 << CS00);
	TCCR0B |= (1 << CS01);
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
            writeText("Elektronik und Technische Informatik!!!"); 
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
    static unsigned char ramp = 0;

    TCNT0 = TIMER_START_VALUE;
    count++;
    PORTD = ramp++;

  
    
    if (count > TEN_MSEC) 
    {
        count = 0;
        countOneSec++;


        newKey = PINB & KEY;  

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
Für delay() 
entweder selbst programmieren, zB mit asm volatile ("nop"); 
oder 
#include <util/delay.h>

Zum Programm:

An PORTB - Pin 0 sollte ein Taster angeschlossen werden. 
Mit dem kann die On-Board-Led ein / aus geschalten werden. 

Ebenfalls möglich: im Seriellen Monitor x eingeben. 

Port D kann dazu verwendet werden an einem R2R - Netzwerk 
(siehe: https://de.wikipedia.org/wiki/R2R-Netzwerk)
eine Rampe auszugeben. 

Achtung: PORTD - Pin 0 darf nicht verwendet werden 
- sonst gibts kein Serielles Empfangen mehr. Siehe init!
PORTD Pin 1 (senden) verursacht Störungen des Rampen-Signals
*/