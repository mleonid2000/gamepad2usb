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
//	gamepad2usb (2 axes, 8 buttons), file "common.h"
//=============================================================================
//System settings
#include <avr/io.h>			//Input-Output Library
#include <avr/interrupt.h>	//Interrupt Library
#include <avr/pgmspace.h>	//Settings Library
#include <util/delay.h>		//Delay Library
#include <usbdrv/usbdrv.h>	//V-USB Library

//=============================================================================
//System Settings
#define F_CPU 12000000L

//=============================================================================
//Report Structure
typedef struct
{
	//Zero byte - ReportID
	unsigned char x;		//Byte 1, X-axe (-127.0.127)
	unsigned char y;		//Byte 2, Y-axe (-127.0.127)
	unsigned char buttons;	//Byte 3, Buttons (0-7)
} t_gamepad2usb;

//=============================================================================
//Global Variable
extern t_gamepad2usb gamepad2usb_data[1];
extern unsigned char autofire_counter;	//Autofire pattern 2on:2off after USB send

//=============================================================================
//Function prototypes
extern void inDecoderInit(void);	//AVR Initialization
extern void inDecoderPoll(void);	//Data poll
extern void outSendData(void);		//Send data over USB
