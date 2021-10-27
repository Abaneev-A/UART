#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "lib/Modbus.h"

Modbus master(0x11);

enum param {
    POWER_1,
    FREQ_1,
    POWER_2,
    POWER_3,
    PWM_3,
    POWER_4,
    PWM_4
};

uint16_t storage[100] = {0};
uint8_t RX_UART1[100] = {0};
volatile uint16_t counter = 0;
volatile uint16_t count = 0;
unsigned char pwm_state = 0;

void USART_Trans(uint8_t data[], uint16_t size)
{
    for (uint16_t i = 0; i < size; i++)
    {
        while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = data[i];
    }
}

ISR(USART_RX_vect)
{
    TIFR0 = 0x01; //!!!!
    RX_UART1[counter] = UDR0;
    counter++;
    TCNT0 = 56;
    if ((TIMSK0 & (1 << TOIE0)) == 0) {
        TIMSK0 = 1 << TOIE0;
    }
    
    if (counter == 100) counter = 0;
}

ISR(TIMER0_OVF_vect)
{   UCSR0B &= ~(1 << RXCIE0);
    TIMSK0 &= ~(1 << TOIE0);
    uint8_t TX_UART1[100] = {0};
    uint16_t m;
    master.parsing(RX_UART1, TX_UART1, storage, counter, m);
    USART_Trans(TX_UART1, m);
    UCSR0B |= 1 << RXCIE0;
    counter = 0;
}

ISR(TIMER2_OVF_vect)
{
    
    count++;
    if (count >= storage[FREQ_1]) {
        if ((PINC & (1 << PINC0)) > 0)
        {
            PORTC &= ~(1 << PORTC0);
        } else
        {
            PORTC |= 1 << PORTC0;
        }

        count = 0;
    }
    
    TCNT2 = 56;
}

ISR(TIMER1_OVF_vect)
{ 
    if (pwm_state == 0)
    {
        OCR1BL++;
        
        if (OCR1BL > storage[PWM_4])
        {
            pwm_state = 1;
        }
    }

    if (pwm_state == 1)
    {
        OCR1BL--;

        if (OCR1BL < 1)
        {
            pwm_state = 0;
        }
    }
}



int main(void)
{
    UBRR0L = 0x10;
    UCSR0A = 0x02;
    UCSR0B = 1 <<  RXEN0 | 1 <<  TXEN0 | 1 << RXCIE0;
    
    TCCR0B = 1 << CS01 | 1 << CS00;
    TCNT0 = 56;
    
    // установки для 1 светодиода
    
    DDRC = 0b00011111;
    TCCR2B |= 1 << CS21;
    TCNT2 = 56;
    
    // установки для 3 светодиода
    
    DDRB |= 1 << DDB1;
    
    TCCR1A |= 1 << WGM10 | 1 <<  COM1A1;

    TCCR1B |= 1 << CS12 | 1 << WGM12;
    
    // установки для 4 светодиода
    
    DDRB |= 1 << DDB2;
    
    TCCR1A |= 1 << COM1B1;
    
    TIMSK1 |= 1 << TOIE1;
    
    OCR1BL = 0;
    /////////////////////////////////
          
    storage[POWER_1] = 0;
    storage[FREQ_1] = 5000;
    storage[POWER_2] = 0;
    storage[POWER_3] = 0;
    storage[PWM_3] = 100;
    storage[POWER_4] = 0;
    storage[PWM_4] = 200;
    
    sei();
    
    while (1)
    {
        if(storage[POWER_1] == 1) TIMSK2 |= 1 << TOIE2;
        if(storage[POWER_1] == 0) {TIMSK2 &= ~(1 << TOIE2); PORTC &= ~(1 << PORTC0);}
        
        if(storage[POWER_2] == 1) PORTC |= 1 << PORTC1;
        if(storage[POWER_2] == 0) PORTC &= ~(1 << PORTC1);
        
        if(storage[POWER_3] == 1) DDRB |= 1 << DDB1;
        if(storage[POWER_3] == 0) DDRB &= ~(1 << DDB1);
        OCR1AL = storage[PWM_3];
        
        if(storage[POWER_4] == 1) DDRB |= 1 << DDB2;
        if(storage[POWER_4] == 0) DDRB &= ~(1 << DDB2);  
            
    }
}


