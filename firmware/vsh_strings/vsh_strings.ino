#include "Timer.h"
//#include "GnusbuinoMIDI.h"// you have to include the Gnusbuino MIDI library
#include <avr/eeprom.h>

#define DEBUG 0

#define STATE_NOT_CALIBRATING 0
#define STATE_SET_THRESHES 1
#define STATE_CALIBRATE_BOTTOM 2
#define STATE_CALIBRATE_TOP 3

#define STATE_FINGER 1
#define STATE_NO_FINGER 2
#define HIST_SIZE 30

Timer timer;
int ledEvent;

unsigned int rawData[7];
unsigned char sensorValue[7];
unsigned char sensorHistory[7][HIST_SIZE];
unsigned char sentValue[7];

signed int top[7];
signed int bottom[7];
signed int threshold[7];
unsigned char state[7];
unsigned char depressed;

double t;
unsigned char mux, pass, p,i;
signed int temp;

unsigned char buttons,lastbuttons;
unsigned char calibrating_state;


signed int saveThreshRead(unsigned char n) {
		threshold[n] = eeprom_read_word((uint16_t*)(n*2));
		if (abs(threshold[n]-180) > 150) 	threshold[n] = 180;
		
		threshold[n] = 180;
}
signed int saveTopRead(unsigned char n) {
		top[n]  = eeprom_read_word((uint16_t*)(28 + n*2));
		if (abs(top[n]-800) > 200) 			top[n] = 800;
		
		top[n] = 800;
}
signed int saveBottomRead(unsigned char n) {
		bottom[n] = eeprom_read_word((uint16_t*)(14 + n*2));
		if (abs(bottom[n]-250) > 200) 		bottom[n] = 250;
		
		bottom[n] = 250;
}

void stopLed(void){
  timer.stop(ledEvent);
  digitalWrite(16,LOW);
}

void sendData(void) {
	for (i = 0; i < 7; i++){
		    Serial.write(sensorValue[i]);
	}
	Serial.write(depressed);
	//digitalWrite(16,HIGH);
}


void sendDebugData(void) {
/*
	for (i = 0; i < 7;i++) {
		i = 0;
		temp = rawData[i];
		MIDI.write(MIDI_CONTROLCHANGE,50 + i, temp & 3);
		MIDI.write(MIDI_CONTROLCHANGE,60 + i,temp >> 2); 


		temp = threshold[i];
		MIDI.write(MIDI_CONTROLCHANGE,70 + i, temp & 3);
		MIDI.write(MIDI_CONTROLCHANGE,80 + i,temp >> 2); 

	
		temp = bottom[i];
		MIDI.write(MIDI_CONTROLCHANGE,90 + i, temp & 3);
		MIDI.write(MIDI_CONTROLCHANGE,100 + i,temp >> 2); 
	
		temp = top[i];
		MIDI.write(MIDI_CONTROLCHANGE,110 + i, temp & 3);
		MIDI.write(MIDI_CONTROLCHANGE,120 + i,temp >> 2); 
	}*/
}


void setup() {
	analogReference(EXTERNAL);
	pinMode(0,INPUT_PULLUP);
	pinMode(16,OUTPUT);
	
	for (char i = 0; i < 7; i++) {
		saveThreshRead(i);
		saveTopRead(i);
		saveBottomRead(i);
		
	}
	
	if (DEBUG) {timer.every(40,sendDebugData);}
	Serial.begin(57600);
	timer.every(40,sendData);
}



void loop() {
	
	mux++;

	buttons = !digitalRead(0);
	if (buttons) {
		if (!lastbuttons) {
								// click of a button
			switch (calibrating_state) {
				case STATE_NOT_CALIBRATING: {
					calibrating_state = STATE_SET_THRESHES;
					mux = 0;
					digitalWrite(16,HIGH);
					break;
				}
				
				case STATE_CALIBRATE_BOTTOM: {
					calibrating_state = STATE_CALIBRATE_TOP;
					stopLed();
					ledEvent = timer.oscillate(16, 500, HIGH);
					break;
				}

				case STATE_CALIBRATE_TOP: {
					for (i = 0; i < 7; i++) {
						eeprom_write_word((uint16_t*)(i*2),threshold[i]);
						eeprom_write_word((uint16_t*)(14 + i*2),bottom[i]);
						eeprom_write_word((uint16_t*)(28 + i*2),top[i]);	
									
					}
					stopLed();
					delay(200);
					ledEvent = timer.oscillate(16, 100, HIGH);
					timer.after(2000,stopLed);
					calibrating_state = STATE_NOT_CALIBRATING;
					break;
				}
					
			}
		}	
	} 
	lastbuttons = buttons;
	
	if (mux >= 7) {
		if (calibrating_state == STATE_SET_THRESHES) {
			calibrating_state = STATE_CALIBRATE_BOTTOM;
			ledEvent = timer.oscillate(16, 1000, HIGH);
		}
		mux = 0;
		pass++;
		pass %= HIST_SIZE;
	}
	
	temp =  1024 - analogRead(mux);
	rawData[mux] = temp;
	
	
	
	if (calibrating_state == STATE_SET_THRESHES) {
		threshold[mux] 	= temp + 5;
		bottom[mux] 	= threshold[mux] + 1;
		top[mux] 		= threshold[mux] + 100;
	}
	if (calibrating_state == STATE_CALIBRATE_TOP) {
			if ( temp > top[mux]) {
				top[mux] = temp; 
			}
	}
	if (calibrating_state == STATE_CALIBRATE_BOTTOM) {
			if ( temp > bottom[mux]) {
				bottom[mux] = temp; 
			}
	}

	


	if (temp > threshold[mux]) { 
		depressed |= (1 << mux);
		t = (double)temp;
		t = (t - bottom[mux]) / (top[mux] - bottom[mux]);
		if (t < 0) t = 0;
		if (t > 1.) t = 1.;
		t *= 255.;

		if (state[mux] == STATE_NO_FINGER) {// new fingerpress
		for (i = 0; i < 5; i++)
			sensorHistory[mux][i] = t;
		}
		sensorHistory[mux][pass] = t;
	
		p = pass + HIST_SIZE - 3;
		p %= HIST_SIZE;
		sensorValue[mux] = sensorHistory[mux][p];
		
		//sensorValue[mux] = t;
		state[mux] = STATE_FINGER;
	} else {
		depressed &= ~(1 << mux);
		if (state[mux] == STATE_FINGER) {// we lost the finger
			p = pass + 1;
			p %= HIST_SIZE;
			sensorValue[mux] = sensorHistory[mux][p];
		}
		state[mux] = STATE_NO_FINGER;
	}

	timer.update();
}



