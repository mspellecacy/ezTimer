//import some stuff
#include "pitches.h"
#include "intervals.h"
#include <TimerObject.h>

// Constants won't change.
// They're used here to set pin numbers:
const int primaryButtonPin = 2;    // Starts/Stops primaryTimer
const int secondaryButtonPin = 4;  // Adjusts M
const int ledPin =  13;      // the number of the LED pin
const int speakerPin = 8;

// variables will change:
int primaryButtonState = 0;         // variable for reading the pushbutton status
int secondaryButtonState = 0;
int lastPrimaryButtonState = 0;
int lastSecondaryButtonState = 0;
int timeRemaining = 0;
int lastTimeRemaining = 0;
TimerObject *primaryTimer = new TimerObject(1000);
TimerObject *secondaryTimer = new TimerObject(1000);


// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

long intervals[] = {
  180000, // 3 minutes
  150000, // 2.5 minutes
  120000, // 2 minutes
  90000, // 1.5 minutes
  60000, // 1 minutes
  30000, // .5 minutes
  15000 // .25 minutes
};
int currentInterval = 6;
int intervalSize = sizeof(intervals)/sizeof(long) - 1;


void playSong() {
  for (int thisNote = 0; thisNote < 8; thisNote++) {
    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(speakerPin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(speakerPin);
  }  
}

void updateCountdown(){
  //Serial.println("Updating countdown notification...");
  if(primaryTimer->isEnabled()) {
    long timeRemaining = intervals[currentInterval] - primaryTimer->getCurrentTime();
    Serial.print("Time Remaining: ");
    Serial.print(timeRemaining/1000, DEC);
    Serial.print(" seconds");
    Serial.println();

  }
}

void setup() {
  //Setup someting to log to.
  Serial.begin(9600);

  //Primary timer holds the actual countdown timer.
  //Set timer to singleShot
  primaryTimer->setInterval(intervals[6]);
  primaryTimer->setSingleShot(true);
  primaryTimer->setEnabled(false);

  //Secondard timer polls the primaryTimer for updates while the primaryTimer is running.
  secondaryTimer->setSingleShot(false);
  secondaryTimer->setOnTimer(&updateCountdown);
  secondaryTimer->setEnabled(true);
  secondaryTimer->Start();

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);

  // initialize the pushbuttons pin as an input:
  pinMode(primaryButtonPin, INPUT);
  pinMode(secondaryButtonPin, INPUT);  
}

void loop(){
  // read the state of the pushbutton value:
  primaryButtonState = digitalRead(primaryButtonPin);
  secondaryButtonState = digitalRead(secondaryButtonPin);
  //Update Timers...
  primaryTimer->Update();
  secondaryTimer->Update();

  //Do some logic...
  if(! primaryButtonState == lastPrimaryButtonState){
    //Is the primary button being pressed?
    if (primaryButtonState == HIGH) {     
      if(primaryTimer->isEnabled()){
        primaryTimer->Stop();
        primaryTimer->setEnabled(false);
        Serial.println("Timer reset.");
      } 
      else {

        primaryTimer->setEnabled(true);
        primaryTimer->setInterval(intervals[currentInterval]); 
        primaryTimer->setOnTimer(&playSong);
        primaryTimer->Start();
        Serial.println("Timer started.");
        Serial.print("Timer Interval: ");
        Serial.print(intervals[currentInterval], DEC);
        Serial.println();
      }
    }
  }
  if(! secondaryButtonState == lastSecondaryButtonState){
    //If the primaryTimer is running then we ignore the secondary button...
    if(primaryTimer->isEnabled() == false) {
      if (secondaryButtonState == HIGH) {
        //Index out of bounds check...
        if((currentInterval+1) > intervalSize) {
           currentInterval = 0;
        } else {
           currentInterval = currentInterval + 1;
          
        }     
        Serial.print("New Timer Interval: ");
        Serial.print(intervals[currentInterval], DEC);
        Serial.println();
      }
    }
  }



  lastPrimaryButtonState = primaryButtonState;
  lastSecondaryButtonState = secondaryButtonState;
}
















