#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

void USART_Trans(uint8_t data[], uint16_t size);

uint8_t RX_UART1[100] = {0};
volatile uint16_t counter = 0;
uint8_t crc1[2] = {0};
uint16_t CRC = 0;

uint16_t crc_16(uint8_t *buffer, uint16_t buffer_size)
{
	uint8_t temp = 0;
	uint16_t crc = 0xFFFF;

	for(uint16_t byte = 0; byte < buffer_size; byte++)
	{
		crc = crc ^ buffer[byte];

		for(uint8_t j = 0; j < 8; j++)
		{
			temp = crc & 0x0001;
			crc = crc >> 1;

			if(temp)
			{
				crc = crc ^ 0xA001;
			}
		}
	}

	temp = crc & 0x00FF;
	crc = (crc >> 8) | (temp << 8);

	return crc;
}



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
	CRC = crc_16(RX_UART1, counter);
	crc1[0] = (uint8_t)(CRC >> 8);
	crc1[1] = (uint8_t)(CRC & 0x00FF);
	USART_Trans(crc1, 2);
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
	
	sei();
	
	while (1)
	{
	}
}


