/* 
 * Copyright (C) 2016. Leonid Miroshnichenko
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The author may be contacted at vk.com/gamepad2usb
 */

//=============================================================================
//	gamepad2usb (2 axes, 8 buttons), file "in_gamepad2usb.h"
//=============================================================================
//Header Files
#include "common.h" //Common header file

//=============================================================================
//ATmega8a-au (TQFP32) Port configuration
//You can use any port. PortB, PortC or PortD(except PD0, PD1 and PD2)
//Change PIN's - axis or buttons. It is simple!!!
//-----------------------------------------------------------------------------
// PIN's - Axe's
#define axis_up		(bit_is_clear(PINC, PC5) ? 1 : 0)	//Axis_UP
#define axis_down	(bit_is_clear(PINC, PC4) ? 1 : 0)	//Axis_Down
#define axis_left	(bit_is_clear(PINC, PC3) ? 1 : 0)	//Axis_Left
#define axis_right	(bit_is_clear(PINC, PC2) ? 1 : 0)	//Axis_Right

// PIN's - Button's
#define btn_0		(bit_is_clear(PINB, PB0) ? 1 : 0)	//button_0
#define btn_1		(bit_is_clear(PINB, PB1) ? 1 : 0)	//button_1
#define btn_2		(bit_is_clear(PINB, PB2) ? 1 : 0)	//button_2
#define btn_3		(bit_is_clear(PINB, PB3) ? 1 : 0)	//button_3
#define btn_4		(bit_is_clear(PINB, PB4) ? 1 : 0)	//button_4
#define btn_5		(bit_is_clear(PINB, PB5) ? 1 : 0)	//button_5
#define btn_6		(bit_is_clear(PINC, PC1) ? 1 : 0)	//button_6
#define btn_7		(bit_is_clear(PINC, PC0) ? 1 : 0)	//button_7
//-----------------------------------------------------------------------------

//=============================================================================
// Global Variables
t_gamepad2usb gamepad2usb_data[1];		//Buttons data
t_gamepad2usb gamepad2usb_data_turbo[1];	//Autofire Buttons data

static unsigned short int holdoncounter = 0;//Autofire activate button counter
static unsigned char autofire_flag = 0;	//Autofire flag
unsigned char autofire_counter = 0;	//Autofire pattern 2on:2off after USB send

//=============================================================================
//AVR Initialization
void inDecoderInit(void)
{
	//All pullups
	PORTB	 =	0x1F;
	DDRB	 =	( 0 << PB0 ) | ( 0 << PB1 ) | ( 0 << PB2 ) |
			( 0 << PB3 ) | ( 0 << PB4 ) | ( 0 << PB5 ) ;
	
	//All pullups
	PORTC	 =	0x1F;
	DDRC	 =	( 0 << PC0 ) | ( 0 << PC1 ) | ( 0 << PC2 ) |
			( 0 << PC3 ) | ( 0 << PC4 ) | ( 0 << PC5 ) ;

	//All pullups
	PORTD	|=	0xFA;
	DDRD	|=	( 0 << PD0 ) | ( 0 << PD1 ) | ( 0 << PD2 ) |
			( 0 << PD3 ) | ( 0 << PD4 ) | ( 0 << PD5 ) |
			( 0 << PD6 ) | ( 0 << PD7 );

	// 8-Bit таймер TC2
	//Предделитель = 1024
	TCCR2	 =	( 0 << WGM20 ) | ( 1 << WGM21) | ( 1 << CS22  ) |
			( 1 << CS21  ) | ( 1 << CS20 );

	//Compare Register, 3.9254 us timer freq = 12 Mhz
	OCR2  = 0x2D;
	TCNT2 = 0xD3;

	//Compare Register, 7.936 us timer freq = 12 Mhz
	// OCR2  = 0x5C;
	// TCNT2 = 0xA3;

	//Compare Register, 10.24 us timer freq = 12 Mhz
	// OCR2  = 0x78;
	// TCNT2 = 0x88;

	//Compare Register, 12.8 us timer freq = 12 Mhz
	// OCR2  = 0x96;
	// TCNT2 = 0x6A;

	//Reset TC2 timer
	TIFR	|=	_BV(OCF2);
}

//-----------------------------------------------------------------------------
//Getting data from PIN'S and fill report Data
void inDecoderPoll(void)
{
//Action if timeout
if (bit_is_set(TIFR, OCF2))
	{
	//Reset TC2 timer
	TIFR  |= _BV(OCF2);
	TCNT2  = 0xD3;

	//Default Values
	gamepad2usb_data[0].x = gamepad2usb_data[0].y = 0;
	gamepad2usb_data[0].buttons = 0;
	
	// Axes
	if	(axis_up)	{ gamepad2usb_data[0].y = 0x81;}	//axis_up
	if	(axis_down)	{ gamepad2usb_data[0].y = 0x7F;}	//axis_down
	if	(axis_left)	{ gamepad2usb_data[0].x = 0x81;}	//axis_left
	if	(axis_right)	{ gamepad2usb_data[0].x = 0x7F;}	//axis_right

	// Buttons
	gamepad2usb_data[0].buttons =	(btn_0	<< 0) | // button 0
					(btn_1	<< 1) | // button 1
					(btn_2	<< 2) | // button 2
					(btn_3	<< 3) | // button 3
					(btn_4	<< 4) | // button 4
					(btn_5	<< 5) | // button 5
					(btn_6	<< 6) | // button 6
					(btn_7	<< 7) ; // button 7

	//Reset autofire mode if no buttons pressed
	if ((gamepad2usb_data[0].buttons == 0)) { holdoncounter = 0; autofire_flag = 0; }
	
	//Autofire activate button hold on counter
	if ((btn_0) && (autofire_flag == 0))	holdoncounter++;

	/*Activate Autofire mode
		Autofire mode activate only for pressed buttons
		at the moment of activation autofire.
	*/

	if ((holdoncounter >= 0x0040) && (autofire_flag == 0)) //hold on time around 1-2 sec
		{
			autofire_flag = 1; // Activate Autofire Mode
			holdoncounter = 0; // Reset Autofire holdon counter
			gamepad2usb_data_turbo[0].buttons = gamepad2usb_data[0].buttons; //Autofire Buttons data
		}

	if ((autofire_flag == 1) && (autofire_counter >= 2))
		{
			// Autofire all buttons except btn_0
			gamepad2usb_data[0].buttons ^= 	(gamepad2usb_data_turbo[0].buttons & (gamepad2usb_data[0].buttons & ~1));
		}
	}
}
