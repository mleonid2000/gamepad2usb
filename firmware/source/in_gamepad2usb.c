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
#define axis_up		((PINC & _BV(PC5)) ? 0 : 1)	//Axis_UP
#define axis_down	((PINC & _BV(PC4)) ? 0 : 1)	//Axis_Down
#define axis_left	((PINC & _BV(PC3)) ? 0 : 1)	//Axis_Left
#define axis_right	((PINC & _BV(PC2)) ? 0 : 1)	//Axis_Right

// PIN's - Button's
#define btn_0	((PINB & _BV(PB0)) ? 0 : 1)	//button_0
#define btn_1	((PINB & _BV(PB1)) ? 0 : 1)	//button_1
#define btn_2	((PINB & _BV(PB2)) ? 0 : 1)	//button_2
#define btn_3	((PINB & _BV(PB3)) ? 0 : 1)	//button_3
#define btn_4	((PINB & _BV(PB4)) ? 0 : 1)	//button_4
#define btn_5	((PINB & _BV(PB5)) ? 0 : 1)	//button_5
#define btn_6	((PINC & _BV(PC1)) ? 0 : 1)	//button_6
#define btn_7	((PINC & _BV(PC0)) ? 0 : 1)	//button_7
//-----------------------------------------------------------------------------

//=============================================================================
// Global Variables
t_gamepad2usb gamepad2usb_data[1];			//Buttons data
t_gamepad2usb gamepad2usb_data_turbo[1];	//Autofire Buttons data

static unsigned short int holdoncounter = 0;//Autofire activate button counter
static unsigned char autofire_flag = 0;		//Autofire flag
unsigned char autofire_counter = 0;	//Autofire pattern 2on:2off after USB send


//=============================================================================
//AVR Initialization
void inDecoderInit(void)
{
	PORTB	|=	0x1F;	DDRB	 =	0x00;	//All Pullups
	PORTC	|=	0x1F;	DDRC	 =	0x00;	//All Pullups
	PORTD	|=	0xFA;	DDRD	|=	0x08;	//PD0-PD2 "v-usb", other Pullups

	// 8-Bit timer TC2
	TCCR2	 =	(0 << WGM20 ) | (1 << WGM21) | ( 1 << CS22 ) | ( 1 << CS21 ) | ( 1 << CS20 ); //prescaler = 1024
	OCR2	 =	0x2D;		//Compare Register, 3.9254 us timer freq = 12 Mhz
	TCNT2	 =	0xD3;
	TIFR	|=	_BV(OCF2);	//Reset TC2 timer
}

//-----------------------------------------------------------------------------
//Getting data from PIN'S and fill report Data
void inDecoderPoll(void)
{
//Skip if less than 12.8 us
if (bit_is_set(TIFR, OCF2))
	{
	//Reset TC2 timer
	TIFR  |= _BV(OCF2);
	TCNT2  = 0xD3;

	//Default Values
	gamepad2usb_data[0].x = gamepad2usb_data[0].y = 0;
	gamepad2usb_data[0].buttons = 0;
	
	// Axes (y = 0x81, see USB Descriptors)
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
