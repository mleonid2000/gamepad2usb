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
//	gamepad2usb (2 axes, 8 buttons), file "main.c"
//=============================================================================
//Header Files
#include "common.h" //Common header file

//-----------------------------------------------------------------------------
int main(void)
{
	usbInit();				//USB-Initialization

	usbDeviceDisconnect();	//Disconnect USB
	_delay_ms(250);			//Delay before USB-Initialization
	usbDeviceConnect();		//USB Connect

	inDecoderInit(); //Initialization of decoder_buttons

	sei(); //Allow interrupt's

	while (1) // Unlimited cycle
		{
			inDecoderPoll();//Input of buttons_joypads
			outSendData();	//Output data
			usbPoll();		//Send data over USB
		}
}
