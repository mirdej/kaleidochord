/*
  GnusbuinoMIDI.h - MIDI over USB library for the Gnusbuino
  http://gnusb.sourceforge.net
  
  Copyright (c) 2012 Michael Egger.  All right reserved.

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

#ifndef MIDICLASS_h
#define MIDICLASS_h

#include <inttypes.h>
#include <stdio.h> // for size_t


/******************************************************************************
 * Definitions
 ******************************************************************************/
// MIDI Status Bytes

#define MIDI_NOTEOFF			0x80
#define MIDI_NOTEON				0x90
#define MIDI_POLYAFTERTOUCH		0xA0
#define MIDI_CONTROLCHANGE		0xB0
#define MIDI_PROGRAMCHANGE		0xC0
#define MIDI_CHANNELAFTERTOUCH	0xD0
#define MIDI_PITCHBEND			0xE0

//Realtime Messages
#define MIDI_QUARTERFRAME		0xF1
#define MIDI_SONGPOS			0xF2
#define MIDI_SONGSELECT			0xF3
#define MIDI_TIMINGCLOCK		0xF8
#define MIDI_START				0xFA
#define MIDI_CONTINUE			0xFB
#define MIDI_STOP				0xFC

#if defined(__AVR_ATtiny85__)
	#define MIDI_MAX_BUFFER		10
#else
	#define MIDI_MAX_BUFFER		127
#endif

typedef struct {
	unsigned char command;
	unsigned char key;
	unsigned char value;
	unsigned char channel;
} MIDIMessage;

#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2


/******************************************************************************
 * MIDI Class
 ******************************************************************************/

class MIDIClass {

public:
	//void write(uint8_t,uint8_t,uint8_t);
	void write(uint8_t,uint8_t,uint8_t,uint8_t = 0);
	uint8_t read(MIDIMessage*);
	size_t print(const char *);
    size_t print(char, int = DEC);
    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(double, int = 2);
	
	size_t println(void);
	size_t println(char, int = DEC);
	size_t println(const char *);
    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(double, int = 2);
	
	
	void sendMIDI(void);
	void flush(void);
	
	void receiveMIDI(uint8_t,uint8_t,uint8_t);
	
	private:
		unsigned char _midiOutData[4];

		unsigned char _midiSendEnqueueIdx;
		unsigned char _midiSendDequeueIdx;
		unsigned char _midiSendQueue [MIDI_MAX_BUFFER * 3];
	
		unsigned char _midiRecvEnqueueIdx;
		unsigned char _midiRecvDequeueIdx;
		unsigned char _midiRecvQueue [MIDI_MAX_BUFFER * 3];

		char * _sysex_buffer;
		unsigned char _sysex_idx;
		unsigned char _sysex_len;

};

	extern MIDIClass MIDI;

#endif

