#include <Arduino.h>

/*
I, Mychaylo Tatarynov, student number 000800271, certify that all code submitted is my own work; 
that I have not copied it from any other source. 
I also certify that I have not allowed my work to be copied by others.

2022/10/17
*/

//Defining IO pins.
#define PIN_PIR D5
#define PIN_BUTTON D6


//The possible alarm states.
enum AlarmState {
  Idle, Countdown, Active, Disabled
};

//The current alarm state.
enum AlarmState State = Idle;

//The time in ms before the alarm activates.
const int COUNTDOWN_TIMER = 10000;
//The time between pauses when the LED is flashing.
const int FLASH_DELAY = 200;

//The motion sensor's current state (true is motion detected).
bool PIRState = false;
//The button's current state (true is pressed).
bool ButtonState = false;
//The LED's current state (true is on).
bool LEDState = false;

//When the countdown began.
int CountdownStartTime;
//When the time flashed last.
int LastFlashTime = 0;
//Whether or not the button's input has been sunk (responded to).
bool ButtonSunk = true;

//Updates the input sensor states.
void updateInputStates() {
  PIRState = digitalRead(PIN_PIR);

  bool buttonState = !digitalRead(PIN_BUTTON);
  if (ButtonState != buttonState) {
    ButtonSunk = false;
  }
  ButtonState = buttonState;
}

//Sets the LED's state (turns it on or off).
void SetLEDState(bool state) {
  if (state == LEDState) {
    return;
  }

  digitalWrite(LED_BUILTIN, !state);
  LEDState = state;
}

//Updates the alarm's state.
void updateState() {
  int currentTime = millis();

  switch (State) {
    case Idle:
      SetLEDState(false);

      //If motion is detected, starts countdown.
      if (PIRState) {
        State = Countdown;
        CountdownStartTime = currentTime;
      }
      break;

    case Countdown:
      if (currentTime - CountdownStartTime >= COUNTDOWN_TIMER) {
        State = Active;
      } else if (ButtonState && !ButtonSunk) {
        //If the button is pressed and hasn't sunk, the alarm is disabled.
        ButtonSunk = true;
        State = Disabled;
      } else if (currentTime - LastFlashTime >= FLASH_DELAY) {
        //Flashes the LED every flash interval time.
        SetLEDState(!LEDState);
        LastFlashTime = currentTime;
      }
      break;

    case Active:
      SetLEDState(true);
      break;

    case Disabled:
      SetLEDState(false);

      if (ButtonState && !ButtonSunk) {
        ButtonSunk = true;
        State = Idle;
      }
      break;
  }
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  Serial.println("\n\nName: Mychaylo Tatarynov (000800271)");
  Serial.println("ESP8266 Chip Id: " + String(ESP.getChipId()));
  Serial.println("Flash Chip Id: " + String(ESP.getFlashChipId()) + "\n");

  //PIR sensor as a digital input.
  pinMode(PIN_PIR, INPUT);
  //Button as a digital input.
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  //LED as a digital output.
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //Updates all states.
  updateInputStates();
  updateState();
}