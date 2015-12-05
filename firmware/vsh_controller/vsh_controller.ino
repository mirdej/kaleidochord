#include "GnusbuinoMIDI.h"


unsigned char sensorValue[96];
unsigned char serialBuffer[7];

unsigned char sentValue[96];
unsigned char mux_ext,i,j,idx;
unsigned char mux_int;
unsigned int  temp;
unsigned char sentButtons;
unsigned char lastButtons[8];      

unsigned long lastSPI;
unsigned char serialIdx;


void setup() {
	DDRC = 0x0F;

    DDRB = (1 << 7)|(1 << 4);  //CLK/SS/ Latch
    SPCR = (1 << SPE)|(1 << MSTR)|(1 << SPR0);  //SPI
    
    	Serial.begin(57600);
}

unsigned char SPI_transmit(unsigned char cData) {
	SPDR = cData;
	while(!(SPSR & (1 << SPIF))){ ; }
	return SPDR;
}


void loop() {
	unsigned char noteOns, noteOffs;
	//----------------------------------- check SPI
	if (millis()-lastSPI > 1) {
		PORTB |= (1 << 4); // latch
		for (j = 0; j<8; j++ ) {
			temp = SPI_transmit(0);
			temp = ~temp;

			if (temp != lastButtons[j]) {
				
				noteOns = temp & ~lastButtons[j];
				noteOffs = ~temp & lastButtons[j];
				for (i = 0; i < 8; i++) {
					if (noteOns & (1 << i)) {
						MIDI.write(MIDI_NOTEON, j*8 + i, 127);
					}
				if (noteOffs & (1 << i)) {
						MIDI.write(MIDI_NOTEON, j*8 + i, 0);
					}
				}
								
			}
			lastButtons[j] = temp;
		}

		PORTB &= ~(1 << 4); // latch
			
		//----------------------------------- check Serial	

		if (Serial.available()) {
			temp = Serial.read(); 
			if (serialBuffer[serialIdx] != temp) {
				serialBuffer[serialIdx] = temp;
				MIDI.write(MIDI_CONTROLCHANGE,90+serialIdx,temp); 
			}
			serialIdx++;
		} else {
			serialIdx = 0;
		}

		lastSPI = millis();
	}

	//----------------------------------- check Analog	
	mux_ext++;
	if (mux_ext >= 16) {
		mux_ext = 0;
		mux_int++;
		if (mux_int >= 5) {
			mux_int = 0;
			delay(20);
		}
	}

	PORTC = mux_ext;
	
	idx = mux_int * 16 + mux_ext;
	temp = analogRead(mux_int) >> 2;
	temp = (sensorValue[idx] + temp ) / 2;
	
	sensorValue[idx] = temp; 
	if (sensorValue[idx] != sentValue[idx]) {
		MIDI.write(MIDI_CONTROLCHANGE,idx,sensorValue[idx]); 
		sentValue[idx] = sensorValue[idx];
	}
}



