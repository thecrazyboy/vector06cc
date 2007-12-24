#ifndef _SPECIALIO_J
#define _SPECIALIO_J

#define 	IOPORT_BASE			0xE000
#define		IOPORT_MMC_A		0x00
#define		IOPORT_SPDR			0x01
#define		IOPORT_SPSR			0x02
#define 	IOPORT_SOCKPORT		0x03			/* Socket contact port */

#define		IOPORT_SERIAL_TxD	0x04
#define		IOPORT_SERIAL_RxD	0x05
#define		IOPORT_SERIAL_CTL	0x06

#define		IOPORT_TIMER_1		0x07			/* counts down by 1 every 10ms */
#define		IOPORT_TIMER_2		0x08			/* counts down by 1 every 10ms */

#define		IOPORT_GLEDS		0x10

#define		MMC_A		(*((unsigned char *)(IOPORT_BASE+IOPORT_MMC_A)))
#define		SPDR		(*((unsigned char *)(IOPORT_BASE+IOPORT_SPDR)))
#define		SPSR        (*((unsigned char *)(IOPORT_BASE+IOPORT_SPSR)))
#define		SOCKPORT	(*((unsigned char *)(IOPORT_BASE+IOPORT_SOCKPORT)))

#define		SERIAL_TxD	(*((unsigned char *)(IOPORT_BASE+IOPORT_SERIAL_TxD)))
#define		SERIAL_RxD	(*((unsigned char *)(IOPORT_BASE+IOPORT_SERIAL_RxD)))
#define		SERIAL_CTL	(*((unsigned char *)(IOPORT_BASE+IOPORT_SERIAL_CTL)))

#define		TIMER_1		(*((unsigned char *)(IOPORT_BASE+IOPORT_TIMER_1)))
#define		TIMER_2		(*((unsigned char *)(IOPORT_BASE+IOPORT_TIMER_2)))

#define		GREEN_LEDS	(*((unsigned char *)(IOPORT_BASE+IOPORT_GLEDS)))


#define SOCKWP		0x20			/* Write protect switch (PB5) */
#define SOCKINS		0x10			/* Card detect switch (PB4) */


#define SELECT()	MMC_A &= ~1
#define DESELECT()	MMC_A |= 1

#define SPIF	0x01

#define loop_until_bit_is_set(x,b)	{for(;(x)&(b)!=0;);}

#define rcvr_spi_m(dst)	{SPDR=0xFF; loop_until_bit_is_set(SPSR,SPIF); *(dst)=SPDR;}

#define xmit_spi(dat) 	SPDR=(dat); loop_until_bit_is_set(SPSR,SPIF)

#endif