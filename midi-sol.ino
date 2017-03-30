/*
midi-sol
*/

#include <Event.h>
#include <Timer.h>
#include <MIDI.h>

#define MIDI_CHANNEL 1 // channel to listen for messages on

#define PIN_RANGE_UPPER 13 // set a range of pins to use for notes
#define PIN_RANGE_LOWER 5

#define OCTAVE 4
#define OFFSET 24 

#define BASE_MIDI_NOTE (OCTAVE * 12 + OFFSET) // corresponds to actual note Cx 
                                              // where x is our defined octave


MIDI_CREATE_DEFAULT_INSTANCE();

Timer t;

int pulseTimes[9] = {0, 0, 0, 
                     0, 0, 0,
                     0, 0, 0};


void endPulse() {
  char i;
  char lowest = 0;
  int compare;

  // turn off whichever pin has the shortest time in the array
  for (i = 0; i < 9; i++) {
    compare = pulseTimes[i];
    
    if (pulseTimes[lowest] == 0)
      lowest = i;

    if (compare < pulseTimes[lowest] && compare != 0)
      lowest = i;
    
  }
  pulseTimes[lowest] = 0;
  digitalWrite(lowest + 5, LOW);
  
}

void beginPulse(int pin, int time) {
  pulseTimes[pin - PIN_RANGE_LOWER] = time;
  digitalWrite(pin, HIGH);

  // register the end event for 'time' ms later
  t.after(time, endPulse);
}


void handleNoteOn(byte channel, byte note, byte velocity) {
  if (channel == MIDI_CHANNEL) {
    char pin = PIN_RANGE_UPPER - (note - BASE_MIDI_NOTE);
  
    if (pulseTimes[pin - PIN_RANGE_LOWER] == 0) {
      beginPulse(pin, (velocity * 3) / 2);
    }
  }
}

void initMidi() {
  MIDI.begin(MIDI_CHANNEL);
  MIDI.setHandleNoteOn(handleNoteOn);
}

void initPins() {
  int i;

  for (i = PIN_RANGE_LOWER; i <= PIN_RANGE_UPPER; i++){
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }  
}

// the setup function runs once when you press reset or power the board
void setup() {
  initPins();
  initMidi();
}


// the loop function runs over and over again forever
void loop() {
  MIDI.read();
  t.update();
}
