#include <Keyboard.h>

/* CONFIG */
const int mode = 1;             // 1: straight key (or software oscillator for iambic paddle) 2: iambic paddle oscillator
const bool speaker = false;      // speaker on or off
const bool kbEmulator = true;  // keyboard emulator on or off
const int key1 = 91;            // ascii character for dit (or straight key)
const int key2 = 93;            // ascii character for dah
const unsigned long unit = 5000;  // number of cycles per unit
const int note = 800;           // music note/pitch

const int button1Pin = 3;       // dit (or straight key)
const int button2Pin = 5;       // dah
const int audioPin = 9;         // output audio on pin 9

unsigned long timer1 = 0;
unsigned long timer2 = 0;
unsigned long pauseTimer = 0;
int queue = 0;
int button1State = 0;
int button2State = 0;

void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  Keyboard.begin();
}

void loop() {
  //   read the state of the pushbutton value:
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (button1State == HIGH) {
    if (mode == 1) {
      if (speaker) {
        tone(audioPin, note);
      }
      if (kbEmulator) {
        Keyboard.press(key1);
      }
    } else if (mode == 2){
      registerKeyPress(1);
    }
  } else {
    if (mode == 1) {
      Keyboard.release(key1);
    }
  }
  if (button2State == HIGH) {
    if (mode == 1) {
      if (speaker) {
        tone(audioPin, note);
      }
      if (kbEmulator) {
        Keyboard.press(key2);
      }
    } else if (mode == 2) {
      registerKeyPress(2);
    }
  } else {
    if (mode == 1) {
      Keyboard.release(key2);
    }
  } 
  if (button1State == LOW && button2State == LOW) {
    if (mode == 1) {
      noTone(audioPin);
      Keyboard.releaseAll();
    }
  }
  if (mode == 2) {
    oscillatePaddle();
  }
}

void registerKeyPress(int key){
  if (queue > 0) return;
  if (key == 1 && timer1) {
    return;
  } else if (key == 2 && timer2) {
    return;
  }
  queue = key;
}

void oscillatePaddle() {
  if (pauseTimer) {
    noTone(audioPin);
    pauseTimer--;
    return;
  }
  if (timer1) {
    timer1--;
    if (timer1 == 0) {
      pauseTimer = unit;
    }
    return;
  } else if (timer2) {
    timer2--;
    if (timer2 == 0) {
      pauseTimer = unit;
    }
    return;
  }

  if (timer1 == 0 && timer2 == 0 && pauseTimer == 0){
    if (queue == 1) {
      queue = 0;
      timer1 = unit;
      if (speaker) {
        tone(audioPin, note);
      }
      if (kbEmulator) {
        Keyboard.press(key1);
      }
    } else if (queue == 2) {
      queue = 0;
      timer2 = unit * 3;
      if (speaker) {
        tone(audioPin, note);
      }
      if (kbEmulator) {
        Keyboard.press(key2);
      }
    } else {
      noTone(audioPin);
      Keyboard.releaseAll();
    }
  } 
}