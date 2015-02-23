/* 	АКВАКОНТРОЛЛЕР 2014 В. 1.0.0.
	
	Библиотека для работы с шиной данных 1-Wire. 
	Была повзаимствована с открытого проекта на сайте www.mikrocontroller.net
	и переработана под собственные нужды (убраны неиспользуемые функции и добавлено 
	указание используемого порта и вывода МК для соответствующего датчика).
	
	По стечению многих лет определить авторство весьма затруднительно.
*/

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#include "crc8.c"
#include "Functions.h"
#include "delay.h"
#include "main.h"

#include <inttypes.h>

// enable extended output via UART by defining:
#define DS18X20_VERBOSE

/* return values */
#define DS18X20_OK          0x00
#define DS18X20_ERROR       0x01
#define DS18X20_START_FAIL  0x02
#define DS18X20_ERROR_CRC   0x03

#define DS18X20_POWER_PARASITE 0x00
#define DS18X20_POWER_EXTERN   0x01

/* DS18X20 specific values (see datasheet) */

#define DS18X20_CONVERT_T	0x44
#define DS18X20_READ		0xBE
#define DS18X20_WRITE		0x4E
#define DS18X20_EE_WRITE	0x48
#define DS18X20_EE_RECALL	0xB8
#define DS18X20_READ_POWER_SUPPLY 0xB4

#define DS18B20_CONF_REG    4
#define DS18B20_9_BIT       0
#define DS18B20_10_BIT      (1<<5)
#define DS18B20_11_BIT      (1<<6)
#define DS18B20_12_BIT      ((1<<6)|(1<<5))

// indefined bits in LSB if 18B20 != 12bit
#define DS18B20_9_BIT_UNDF       ((1<<0)|(1<<1)|(1<<2))
#define DS18B20_10_BIT_UNDF      ((1<<0)|(1<<1))
#define DS18B20_11_BIT_UNDF      ((1<<0))
#define DS18B20_12_BIT_UNDF      0

// conversion times in ms
#define DS18B20_TCONV_12BIT      750
#define DS18B20_TCONV_11BIT      DS18B20_TCONV_12_BIT/2
#define DS18B20_TCONV_10BIT      DS18B20_TCONV_12_BIT/4
#define DS18B20_TCONV_9BIT       DS18B20_TCONV_12_BIT/8
#define DS18S20_TCONV            DS18B20_TCONV_12_BIT

// constant to convert the fraction bits to cel*(10^-4)
#define DS18X20_FRACCONV         625

#define DS18X20_SP_SIZE  9

#define OW_MATCH_ROM	0x55
#define OW_SKIP_ROM	    0xCC
#define	OW_SEARCH_ROM	0xF0

#define	OW_SEARCH_FIRST	0xFF		// start new search
#define	OW_PRESENCE_ERR	0xFF
#define	OW_DATA_ERR	    0xFE
#define OW_LAST_DEVICE	0x00		// last device found

#define OW_CONF_CYCLESPERACCESS 13
#define OW_CONF_DELAYOFFSET ( (uint16_t)( ((OW_CONF_CYCLESPERACCESS)*1000000L) / F_OSC  ) )


uint8_t DS18B20_read_meas_single(uint16_t *meas, uint8_t p, uint8_t c);
uint8_t DS18B20_start_meas( uint8_t with_power_extern, uint8_t p, uint8_t c);

#define OW_PIN_1  	PC3
#define OW_IN_1   	PINC
#define OW_OUT_1  	PORTC
#define OW_DDR_1  	DDRC

uint8_t ow_reset_b(void);

volatile uint8_t cur_p;
volatile uint8_t cur_c;
volatile uint8_t mask;

//------------------------------------------------------------------------
uint8_t OW_GET_IN_B(void)
{	
	switch (cur_p)
	{
		case 'b': return ( PINB & (1 << cur_c)); break;
		case 'c': return ( PINC & (1 << cur_c)); break;
		case 'd': return ( PIND & (1 << cur_c)); break;
		default: return 0;
	};
}

//------------------------------------------------------------------------
void OW_OUT_LOW_B(void) 	
{ 
	switch (cur_p)
	{
		case 'b': PORTB &= (~(1 << cur_c)); break;
		case 'c': PORTC &= (~(1 << cur_c)); break;
		case 'd': PORTD &= (~(1 << cur_c)); break;
	};
}

//------------------------------------------------------------------------
void OW_DIR_OUT_B(void)
{	
	switch (cur_p)
	{
		case 'b': DDRB |= (1 << cur_c); break;
		case 'c': DDRC |= (1 << cur_c); break;
		case 'd': DDRD |= (1 << cur_c); break;
	};
}

//------------------------------------------------------------------------
void OW_DIR_IN_B(void)
{   
	switch (cur_p)
	{
		case 'b': DDRB &= mask; break;
		case 'c': DDRC &= mask; break;
		case 'd': DDRD &= mask; break;
	};
}

//------------------------------------------------------------------------
void OW_OUT_HIGH_B(void)
{
	switch (cur_p)
	{
	 	case 'b': PORTB |= (1 << cur_c);
		case 'c': PORTC |= (1 << cur_c);
		case 'd': PORTD |= (1 << cur_c);
	};
}

//------------------------------------------------------------------------
uint8_t ow_bit_io_b( uint8_t b )
{
	cli();
	
	OW_DIR_OUT_B(); // drive bus low
	
	delay_us(1); // Recovery-Time wuffwuff was 1
	if ( b ) OW_DIR_IN_B(); // if bit is 1 set bus high (by ext. pull-up)

	delay_us(15-1-OW_CONF_DELAYOFFSET);
		
	if( OW_GET_IN_B() == 0 ) b = 0;  // sample at end of read-timeslot
	
	delay_us(60-15);
	OW_DIR_IN_B();
	
	sei();
	
	return b;
}

//------------------------------------------------------------------------
uint8_t ow_byte_wr_b( uint8_t b )
{
	uint8_t i = 8, j;
	
	do {
		j = ow_bit_io_b( b & 1 );
		b >>= 1;
		if( j ) b |= 0x80;
	} while( --i );
	
	return b;
}

uint8_t ow_byte_rd_b( void )
{
  // read by sending 0xff
  return ow_byte_wr_b( 0xFF ); 
}

void ow_command_simple_b( uint8_t command )
{
	ow_reset_b();
	ow_byte_wr_b( OW_SKIP_ROM );			// to all devices
	ow_byte_wr_b( command );
}

//------------------------------------------------------------------------
uint8_t ow_reset_b(void)
{
	uint8_t err;
	uint8_t sreg;
	
	OW_OUT_LOW_B(); // disable internal pull-up (maybe on from parasite)
	OW_DIR_OUT_B(); // pull OW-Pin low for 480us
	
	delay_us(480);
	
	sreg=SREG;
	cli();
	
	// set Pin as input - wait for clients to pull low
	OW_DIR_IN_B(); // input
	delay_us(66);
	err = OW_GET_IN_B();		// no presence detect
	// nobody pulled to low, still high
	
	// after a delay the clients should release the line
	// and input-pin gets back to high due to pull-up-resistor
	delay_us(480-66);
	if( OW_GET_IN_B() == 0 )		// short circuit
		err = 1;
	SREG=sreg;
	sei();
	return err;
}

//------------------------------------------------------------------------
void ow_parasite_enable_b(void)
{
    OW_OUT_HIGH_B();
	OW_DIR_OUT_B();
}

//------------------------------------------------------------------------
void cur_set(uint8_t p, uint8_t c)
{
		cur_p = p;
		cur_c = c;

		mask = (~(1 << c ));
}

//------------------------------------------------------------------------
uint8_t DS18B20_read_meas_single(uint16_t *meas, uint8_t p, uint8_t c)
{
	uint8_t i;
	uint8_t sp[DS18X20_SP_SIZE];
	
	cur_set(p, c);	

	ow_command_simple_b(DS18X20_READ);
	for ( i=0 ; i< DS18X20_SP_SIZE; i++ ) sp[i]=ow_byte_rd_b();
	if ( crc8( &sp[0], DS18X20_SP_SIZE ) ) 
	{
		return DS18X20_ERROR_CRC;
	}
	else
	{
		*meas = sp[0] | (sp[1]<<8);
		return DS18X20_OK;
	};
}

//------------------------------------------------------------------------
uint8_t DS18B20_start_meas(uint8_t with_power_extern, uint8_t p, uint8_t c)
{
	cur_set(p, c);
	
	ow_reset_b(); //
	if(OW_GET_IN_B()) { // only send if bus is "idle" = high
		
		ow_command_simple_b( DS18X20_CONVERT_T);
		
		if (with_power_extern != DS18X20_POWER_EXTERN)
			ow_parasite_enable_b();
		return DS18X20_OK;
	} 
	else { return DS18X20_START_FAIL; };
}
