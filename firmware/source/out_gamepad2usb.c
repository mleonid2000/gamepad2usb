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
//	gamepad2usb (2 axes, 8 buttons), file "out_gamepad2usb.c"
//=============================================================================
//Header Files
#include "common.h" //Common header file

//=============================================================================
//USB Report Buffer
static unsigned char usbReport[1+sizeof(t_gamepad2usb)]; //First byte - ReportID

//=============================================================================
//USB HID Descriptor's
PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] =
{
	//2Axes, 8 Buttons
	0x05, 0x01, 		// USAGE_PAGE (Generic Desktop)
	0x09, 0x05, 		// USAGE (GamePad)
	0xa1, 0x01, 		//	COLLECTION (Application)
	0x09, 0x01, 		//		USAGE (Pointer)
	0xa1, 0x00, 		//		COLLECTION (Physical)
	0x85, 0x01, 		//			REPORT_ID (1)
	0x09, 0x30, 		//			USAGE (X)
	0x09, 0x31, 		//			USAGE (Y)
	0x15, 0x81,		//			LOGICAL_MINIMUM (-127)
	0x25, 0x7F,		//			LOGICAL_MAXIMUM (127)
	0x75, 0x08, 		//			REPORT_SIZE (8)
	0x95, 0x02, 		//			REPORT_COUNT (2)
	0x81, 0x02, 		//			INPUT (Data,Var,Abs)
	0xc0,       		//		END_COLLECTION
	0x05, 0x09, 		//			USAGE_PAGE (Button)
	0x19, 0x01, 		//   			USAGE_MINIMUM (Button 1)
	0x29, 0x08, 		//   			USAGE_MAXIMUM (Button 8)
	0x15, 0x00, 		//   			LOGICAL_MINIMUM (0)
	0x25, 0x01, 		//   			LOGICAL_MAXIMUM (1)
	0x75, 0x01, 		// 			REPORT_SIZE (1)
	0x95, 0x08, 		//			REPORT_COUNT (8)
	0x81, 0x02, 		//			INPUT (Data,Var,Abs)
	0xc0,       		// END_COLLECTION
};

//-----------------------------------------------------------------------------
//Build Report with ReportID
static unsigned char *usbBuildReport(unsigned char id)
{
	// For device with 2 or more device descriptors(multi device)
	// static unsigned char lastReport = 0; //Last ReportID

	// if (id) lastReport = id; //Last Report ReportID
	// else lastReport = (lastReport %2) + 1; //Next ReportID
	
	//Copy Report to Buffer
	unsigned char *dp = (void *) & gamepad2usb_data[0];
	unsigned char *rp = usbReport;
	unsigned char i;

	//Fill ReportID
	*rp++ = 1;
	// *rp++ = lastReport;
	// *rp++ = gamepad2usb_data[lastReport-1].report_id;

	for (i = 0; i < sizeof(t_gamepad2usb); i++) *rp++ = *dp++;

	//Return Report Pointer
	return usbReport;
}

//-----------------------------------------------------------------------------
//USBFunctionSetup
unsigned char usbFunctionSetup(unsigned char data[8])
{
	usbRequest_t *rq = (void *)data;

	if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) 
	{
        switch(rq->bRequest) 
			{
			// Return the report if host requests it without USB interrupt
			case USBRQ_HID_GET_REPORT: 
				// wValue: ReportType (highbyte), ReportID (lowbyte)
				usbMsgPtr = usbBuildReport(rq->wValue.bytes[0]);
				return sizeof(usbReport);
				
			// case USBRQ_HID_GET_IDLE: // send idle rate to PC as required by spec
				// usbMsgPtr = (int)&idleRate;
				// return 1;
				
			// case USBRQ_HID_SET_IDLE: // save idle rate as required by spec
				// idleRate = rq->wValue.bytes[1];
				// return 0;
			}
    }
    
    return 0; // by default don't return any data
}

//-----------------------------------------------------------------------------
// Check if the USB Interrupt In point buffer is empty and return
// the data buffer for the following host request via USB interrupt
void outSendData(void)
{
	if (usbInterruptIsReady())
		{
		// fill in the report buffer and return the
		// data pointer (Report ID is selected automatically)
		usbSetInterrupt(usbBuildReport(0), sizeof(usbReport));
		autofire_counter++;
		if (autofire_counter > 3) {autofire_counter = 0;};
		}
}
