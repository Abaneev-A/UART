#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "lib/Modbus.h"

Modbus master(0x11);

enum param {
	PIN_STATE,
	VERSION,
	CONFIGURATION = 0x03
};

uint16_t storage[100] = { 0 };
uint8_t RX_UART1[100] = {0};
volatile uint16_t counter = 0;

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
{   UCSR0B &=  ~(1 << RXCIE0);
	TIMSK0 &= ~(1 << TOIE0);
	uint8_t TX_UART1[100] = {0};
	uint16_t m;
	master.parsing(RX_UART1, TX_UART1, storage, counter, m);
	USART_Trans(TX_UART1, m);
	UCSR0B |= 1 << RXCIE0;
	counter = 0;
}




int main(void)
{
	UBRR0L = 0x10;
	UCSR0A = 0x02;
	UCSR0B = 1 <<  RXEN0 | 1 <<  TXEN0 | 1 << RXCIE0;
	
	TCCR0B = 1 << CS01 | 1 << CS00;
	TCNT0 = 56;
	
	storage[PIN_STATE] = 300;
	storage[VERSION] = 345;
	storage[VERSION + 1] = 654;
	storage[CONFIGURATION] = 3;
	
	sei();
	
	while (1)
	{
	}
}


