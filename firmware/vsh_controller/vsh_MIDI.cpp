/*
  MIDI.cpp - MIDI library for Midi-Gnusbuino
  Copyright (c) 2012 Michael Egger, me@anyma.ch

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "Arduino.h"
#include "vsh_MIDI.h"
#include "usbdrv.h"


#include <avr/wdt.h>




/*---------------------------------------------------------------------------*/
/* PUT MIDI DATA INTO SEND-QUEUE                                             */ 
/*---------------------------------------------------------------------------*/
void MIDIClass::write(unsigned char command, unsigned char pitch,unsigned char velocity){

	// see if this command is already in queue, replace value
	/*for (unsigned char i = 0; i < MIDI_MAX_BUFFER; i++) {
		if (_midiSendQueue[3*i] == command) {
			if (_midiSendQueue[3*i+1] == pitch) {
				_midiSendQueue[3*i+2] = velocity;
				return;
			}
		}
	}
	*/
	_midiSendQueue[_midiSendEnqueueIdx++] = command;
	_midiSendQueue[_midiSendEnqueueIdx++] = pitch;
	_midiSendQueue[_midiSendEnqueueIdx++] = velocity;
	
	_midiSendEnqueueIdx %= MIDI_MAX_BUFFER * 3;
}

size_t MIDIClass::print(const char * message){
	if (_sysex_len) {
		_sysex_len = _sysex_len+strlen(message);
		_sysex_buffer = (char *)realloc (_sysex_buffer,_sysex_len);
		_sysex_buffer = strcat (_sysex_buffer, message);
	} else {
		_sysex_idx = 0;
		_sysex_len = strlen(message)+1;
		_sysex_buffer = (char *)malloc (_sysex_len+1);
		strcpy (_sysex_buffer, message);
	}
	return _sysex_len;
}

const char *byte_to_binary(int x){
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

size_t MIDIClass::print(unsigned long d, int base){
	char temp[36];
	switch (base) {
		case DEC: sprintf(temp,"%u",d); break;
		case BIN: sprintf(temp,"0b%s",byte_to_binary(d)); break;
		case HEX: sprintf(temp,"0x%X",d); break;
	}
	return print(temp);
}

size_t MIDIClass::print(long d, int base){
	char temp[36];
	switch (base) {
		case DEC: sprintf(temp,"%d",d); break;
		case BIN: sprintf(temp,"0b%s",byte_to_binary(d)); break;
		case HEX: sprintf(temp,"0x%X",d); break;
	}
	return print(temp);
}

size_t MIDIClass::print(unsigned int d, int base){
	return print((unsigned long)d,base);
}

size_t MIDIClass::print(unsigned char d, int base){
	return print((unsigned long)d,base);
}

size_t MIDIClass::print(char d, int base){
	return print((long)d,base);
}

size_t MIDIClass::print(int d, int base){
	return print((unsigned long)d,base);
}

size_t MIDIClass::print(double d, int comma) {
	char temp[36];
	sprintf(temp,"%F",2.678);
	return print(temp);
}

size_t MIDIClass::println(void){
  size_t n = print("\r\n");
  return n;
}

size_t MIDIClass::println(const char * c) {
	print(c);
	return println();
}

size_t MIDIClass::println(char c, int base) {
	print(c,base);
	return println();
}

size_t MIDIClass::println(int c, int base) {
	print(c,base);
	return println();
}

size_t MIDIClass::println(long c, int base) {
	print(c,base);
	return println();
}

size_t MIDIClass::println(unsigned char c, int base) {
	print(c,base);
	return println();
}
size_t MIDIClass::println(unsigned int c, int base) {
	print(c,base);
	return println();
}
size_t MIDIClass::println(unsigned long c, int base) {
	print(c,base);
	return println();
}



void MIDIClass::receiveMIDI(unsigned char command, unsigned char pitch,unsigned char velocity){
	// see if this command is already in queue, replace value
	for (unsigned char i = 0; i < MIDI_MAX_BUFFER; i++) {
		if (_midiRecvQueue[3*i] == command) {
			if (_midiRecvQueue[3*i+1] == pitch) {
				_midiRecvQueue[3*i+2] = velocity;
				return;
			}
		}
	}
	_midiRecvQueue[_midiRecvEnqueueIdx++] = command;
	_midiRecvQueue[_midiRecvEnqueueIdx++] = pitch;
	_midiRecvQueue[_midiRecvEnqueueIdx++] = velocity;
	
	_midiRecvEnqueueIdx %= MIDI_MAX_BUFFER * 3;

}

unsigned char MIDIClass::read(MIDIMessage* msg) {
	if (_midiRecvEnqueueIdx != _midiRecvDequeueIdx) {
			msg->command 	= _midiRecvQueue[_midiRecvDequeueIdx];
			msg->key 		= _midiRecvQueue[_midiRecvDequeueIdx+1];
			msg->value 		= _midiRecvQueue[_midiRecvDequeueIdx+2];

			_midiRecvQueue[_midiRecvDequeueIdx++] = 0;
			_midiRecvQueue[_midiRecvDequeueIdx++] = 0;
			_midiRecvQueue[_midiRecvDequeueIdx++] = 0;

			_midiRecvDequeueIdx %= MIDI_MAX_BUFFER * 3;
			return 1;			
	} else {
		return 0;
	}
}

void MIDIClass::sendMIDI(void) {

	if (usbInterruptIsReady()) {	// ready to send some MIDI ?
	
	// Sysex handling:
			// Sysex have to be split up into multiple packets of 32 bits ( 1 byte status, 3 bytes payload )
			// examples: 
			// message with 4 bytes		00 01 02 03 		means sysex message		F0 00 01 02 03 F7 			becomes 	04 F0 00 01 - 07 02 03 F7
			// message with 5 bytes		00 01 02 03 04								F0 00 01 02 03 04 F7 					04 F0 00 01 - 04 02 03 04 - 05 F7 00 00
			// message with 6 bytes		00 01 02 03 04 05							F0 00 01 02 03 04 05 F7 				04 F0 00 01 - 04 02 03 04 - 06 05 F7 00
			

			
	// Sysex handling:
			// Sysex have to be split up into multiple packets of 32 bits ( 1 byte status, 3 bytes payload )
			// examples: 
			// message with 4 bytes		00 01 02 03 		means sysex message		F0 00 01 02 03 F7 			becomes 	04 F0 00 01 - 07 02 03 F7
			// message with 5 bytes		00 01 02 03 04								F0 00 01 02 03 04 F7 					04 F0 00 01 - 04 02 03 04 - 05 F7 00 00
			// message with 6 bytes		00 01 02 03 04 05							F0 00 01 02 03 04 05 F7 				04 F0 00 01 - 04 02 03 04 - 06 05 F7 00
			

			
			if (_sysex_len) {
				
				if (_sysex_len > 3) {
					this->_midiOutData[0] = 0x04;							//	USB-MIDI: Sysex on cable 0
					
					if (_sysex_idx == 0) {
						this->_midiOutData[1] = 0xF0;						// 	MIDI Sysex message
					} else {
						this->_midiOutData[1] = _sysex_buffer[_sysex_idx++];
						_sysex_len -= 1;
					}
					this->_midiOutData[2] = _sysex_buffer[_sysex_idx++];
					this->_midiOutData[3] = _sysex_buffer[_sysex_idx++];
					_sysex_len -= 2;
					
				} else {
				
					if (_sysex_len == 3) {
						this->_midiOutData[0] = 0x07;										//	USB-MIDI: SysEx ends with following three bytes.
						this->_midiOutData[1] = _sysex_buffer[_sysex_idx++];
						this->_midiOutData[2] = _sysex_buffer[_sysex_idx++];
						this->_midiOutData[3] = 0xF7;
						
					} else if (_sysex_len == 2) {
						this->_midiOutData[0] = 0x06;									//	USB-MIDI: SysEx ends with following two bytes.
						this->_midiOutData[1] = _sysex_buffer[_sysex_idx++];
						this->_midiOutData[2] = 0xF7;
						this->_midiOutData[3] = 0x00;
						
					} else {
						this->_midiOutData[0] = 0x05;															//	USB-MIDI: SysEx ends with following one byte.
						this->_midiOutData[1] = 0xF7;
						this->_midiOutData[2] = 0x00;
						this->_midiOutData[3] = 0x00;
					}
					_sysex_len = 0;
					_sysex_idx = 0;
					free(_sysex_buffer);
				}
				
				usbSetInterrupt(this->_midiOutData, 4);

				return;
		}
			
			
		if (_midiSendEnqueueIdx != _midiSendDequeueIdx) {

			unsigned char cmd;
			cmd = _midiSendQueue[_midiSendDequeueIdx];

			this->_midiOutData[0] = ((cmd>>4) & 0x0F) | ((cmd<<4) & 0xF0); //swap high/low nibble
			this->_midiOutData[1] = cmd;
			this->_midiOutData[2] = _midiSendQueue[_midiSendDequeueIdx+1];
			this->_midiOutData[3] = _midiSendQueue[_midiSendDequeueIdx+2];


			_midiSendQueue[_midiSendDequeueIdx++] = 0;
			_midiSendQueue[_midiSendDequeueIdx++] = 0;
			_midiSendQueue[_midiSendDequeueIdx++] = 0;

			_midiSendDequeueIdx %= MIDI_MAX_BUFFER * 3;

			usbSetInterrupt(this->_midiOutData, 4);
			
		}
	}
}


void MIDIClass::flush(void) {

    while( _sysex_len | _midiSendEnqueueIdx != _midiSendDequeueIdx) {
     	//doPeriodical();
     			usbPoll();
		MIDI.sendMIDI();        
		wdt_reset();

    }
}







