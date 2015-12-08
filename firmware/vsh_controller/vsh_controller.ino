#include "GnusbuinoMIDI.h"
#include "Timer.h"

unsigned char sensorValue[96];
unsigned char serialBuffer[8];

unsigned char sentValue[96];
unsigned char mux_ext,i,j,idx;
unsigned char mux_int;
unsigned int  temp;
unsigned char sentButtons;
unsigned char lastButtons[8];      

unsigned char serialIdx;
Timer t;


void setup() {
	DDRC = 0x0F;

    DDRB = (1 << 7)|(1 << 4);  //CLK/SS/ Latch
    SPCR = (1 << SPE)|(1 << MSTR)|(1 << SPR0);  //SPI
    
    t.every(20,checkMIDI);
    t.every(10,checkSPI);
    t.every(1,checkSerial);
    
    
    Serial.begin(57600);
    
}

void forceSend (void) {
	for (i = 0; i < 96; i++) {	
			MIDI.write(MIDI_CONTROLCHANGE,i,sensorValue[i]); 
			checkSerial();
			delay(2);
			checkSerial();
			delay(2);
	}
}

unsigned char SPI_transmit(unsigned char cData) {
	SPDR = cData;
	while(!(SPSR & (1 << SPIF))){ ; }
	return SPDR;
}

	//----------------------------------- check SPI
void checkSPI(void) {
	unsigned char noteOns, noteOffs,noteIdx;

	PORTB |= (1 << 4); // latch
	for (j = 0; j<8; j++ ) {
		temp = SPI_transmit(0);
		temp = ~temp;

		if (temp != lastButtons[j]) {
			
			noteOns = temp & ~lastButtons[j];
			noteOffs = ~temp & lastButtons[j];
			for (i = 0; i < 8; i++) {
				
				noteIdx = j*8 + i;
				if (noteOns & (1 << i)) {
					MIDI.write(MIDI_NOTEON, noteIdx, 127);
					
					if (noteIdx == 63) forceSend();
					
				}
				
				
			if (noteOffs & (1 << i)) {
					MIDI.write(MIDI_NOTEON, noteIdx, 0);
				}
			}
							
		}
		lastButtons[j] = temp;
	}
	PORTB &= ~(1 << 4); // latch
}


//----------------------------------- check Serial	
void checkSerial(void) {
	unsigned char noteOns, noteOffs,noteIdx;

	if (Serial.available()) {
		if (serialIdx >= 8) return;
		
		temp = Serial.read(); 
		
		if (serialBuffer[serialIdx] != temp) {
			if (serialIdx < 7) {
				MIDI.write(MIDI_CONTROLCHANGE,90+serialIdx,temp); 
			} else {
				noteOns = temp & ~serialBuffer[serialIdx];
				noteOffs = ~temp & serialBuffer[serialIdx];
				for (noteIdx = 0; noteIdx < 8; noteIdx++) {
					if (noteOns & (1 << noteIdx)) {
						MIDI.write(MIDI_NOTEON, noteIdx+90, 127);
					}
					if (noteOffs & (1 << noteIdx)) {
						MIDI.write(MIDI_NOTEON, noteIdx+90, 0);
					}
				}
			}
			serialBuffer[serialIdx] = temp;

		}
		serialIdx++;
	} else {
		serialIdx = 0;
	}
}


void checkMIDI(void) {

	for (i = 0; i < 96; i++) {	
		if (sensorValue[i] != sentValue[i]) {
			MIDI.write(MIDI_CONTROLCHANGE,i,sensorValue[i]); 
			sentValue[i] = sensorValue[i];
		}
	}
}


void loop() {
	t.update();
	
	//----------------------------------- check Analog	
	mux_ext++;
	if (mux_ext >= 16) {
		mux_ext = 0;
		mux_int++;
		if (mux_int >= 5) {
			mux_int = 0;
			return;
		}
	}

	PORTC = mux_ext;
	
	idx = mux_int * 16 + mux_ext;
	temp = analogRead(mux_int) >> 2;
	temp = (sensorValue[idx] + temp ) / 2;
	
	sensorValue[idx] = temp; 
}



