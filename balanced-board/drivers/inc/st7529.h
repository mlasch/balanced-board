#ifndef _ST7529_H
#define _ST7529_H

	/* f3 discovery
	 * D0			PD0
	 * ...
	 * D7			PD7
	 * A0			PD8
	 * WR_RW	PD9
	 * E_RD		PD10
	 * RST		PD11
	 * XCS		PD12
	 */

#include <stm32f3xx.h>

#define PIN_A0			GPIO_PIN_8
#define PIN_WR_RW 	GPIO_PIN_9
#define PIN_E_RD		GPIO_PIN_10
#define PIN_RST			GPIO_PIN_11
#define PIN_XCS			GPIO_PIN_12

#define CHAR_TO_BSRR(X)	(uint16_t)( X & 0x000000ff) | ((uint16_t)(~X & 0x000000ff) << 16)

#define EXTIN			0x30
#define EXTOUT		0x31
#define DISON			0xaf
#define DISOFF		0xae
#define DISNOR		0xa6
#define DISINV		0xa7
#define COMSCN		0xbb
#define DISCTRL		0xca
#define SLPIN			0x95
#define SLPOUT		0x94
#define LASET			0x75
#define CASET			0x15
#define DATSDR		0xbc
#define RAMWR			0x5c
#define RAMRD			0x5d
#define PTLIN			0xa8
#define PTLOUT		0xa9
#define RMWIN			0xe0
#define RMWOUT		0xee
#define ASCSET		0xaa
#define SCSTART		0xab
#define OSCON			0xd1
#define OSCOFF		0xd2
#define PWRCTRL		0x20
#define VOLCTRL		0x81
#define VOLUP			0xd6
#define VOLDOWN		0xd7
#define EPSRRD1		0x7c
#define EPSRRD2		0x7d
#define NOP				0x25
#define EPINT			0x07

#define	GRAY1SET	0x20
#define GRAY2SET	0x21
#define ANASET		0x32
#define SWINT			0x34
#define EPCTIN		0xcd
#define EPCOUT		0xcc
#define EPMWR			0xfc
#define EPMRD			0xfd

#define LCD_XSIZE       240
#define LCD_YSIZE       128
#define LCD_FIRSTSEG0   0x5
#define LCD_FIRSTCOM0   0x10


void st7529_init(void);

#endif /* _ST7529_H */
