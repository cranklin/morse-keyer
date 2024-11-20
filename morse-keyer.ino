/*
 * Copyright (C) 2024 Edward Kim (KE6EEK cranklin.com @crankerson)
 *
 * This file is part of KE6EEK Morse Keyer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#include <Keyboard.h>

/* CONFIG */
int mode = 1;                   // 1: straight key (or software keyer) 2: iambic A 3: iambic B
const bool speaker = true;      // speaker on or off (keep it on with jumper on/off)
const bool kbEmulator = true;  // keyboard emulator on or off
const bool keyer = true;        // analog keyer to radio output
const char key1 = KEY_LEFT_CTRL;            // ascii character for dit (or straight key)
const char key2 = KEY_RIGHT_CTRL;            // ascii character for dah
const int note = 500;           // music note/pitch
const int debounceLimit = 10;
int wpm = 23;
int unit = 60;

const int button1Pin = 3;       // dit (or straight key)
const int button2Pin = 5;       // dah
const int audioPin = 9;         // output audio on pin 9
const int keyerPin = 10;        // output to radio on pin 10
const int jumperPin1 = 18;      // used to set mode
const int jumperPin2 = 19;      // used to set mode
const int wpmPin = 20;          // used to read potentiometer value for wpm

unsigned long ditStart = 0;
unsigned long ditStop = 0;
bool ditState = false;
unsigned long dahStart = 0;
unsigned long dahStop = 0;
bool dahState = false;
unsigned long signalStartTime = 0;
unsigned long signalStopTime = 0;
bool sending = false;
int queue = 0;
int lastKey = 0;
int button1State = 0;
int button2State = 0;
String keyString = "";    // 1 = dit, 2 = dah, 3 = short pause, 0 = long pause
long wpmPinValue = 0;

void setup() {
  // initialize the pushbutton pin as an input:
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  pinMode(keyerPin, OUTPUT);
  pinMode(jumperPin1, INPUT_PULLUP);
  pinMode(jumperPin2, INPUT_PULLUP);
  Keyboard.begin();
}

void loop() {
  //   read the state of the pushbutton value:
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);
  wpmPinValue = analogRead(wpmPin);
  wpm = (wpmPinValue * 45 / 1023) + 5;  // y = mx + b; wpm = 45/1023 pv + 5; for a range of 5-50 wpm;
  unit = 60000 / (wpm * 50);
  if (digitalRead(jumperPin1) == LOW) {  // Pins 1 and 2 are shorted.  Iambic A mode
    mode = 2;
  } else if (digitalRead(jumperPin2) == LOW) { // Pins 2 and 3 are shorted.  Iambic B mode
    mode = 3;
  } else { // No pins are shorted.  Straight Key mode
    mode = 1;
  }

  if (mode == 1) { // straight key mode
    if (button1State == HIGH) {
      startSignal(key1);
    } else {
      Keyboard.release(key1);
    }

    if (button2State == HIGH) {
      startSignal(key2);
    } else {
      Keyboard.release(key2);
    }
    if (button1State == LOW && button2State == LOW) {
      stopSignal();
    }
  } else if (mode > 1) { // iambic modes
    if (button1State == HIGH) {
      if (!ditState && millis() - ditStop > debounceLimit) {
        ditStart = millis();
        ditState = true;
      }
    } else {
      if (ditState) {
        ditStop = millis();
        ditState = false;
      }
    }
    if (button2State == HIGH) {
      if (!dahState && millis() - dahStop > debounceLimit) {
        dahStart = millis();
        dahState = true;
      }
    } else {
      if (dahState) {
        dahStop = millis();
        dahState = false;
      }
    }
    oscillatePaddle();
  }

  if (Serial.available()) {
    String strInput = Serial.readString();
    int len = strInput.length();
    strInput.toLowerCase();
    Serial.println(strInput);
    for (int i=0;i<len;i++) {
      Serial.print(strInput[i]);
      switch(strInput[i]) {
        case 'a':
          keyString += "123";
          break;
        case 'b':
          keyString += "21113";
          break;
        case 'c':
          keyString += "21213";
          break;
        case 'd':
          keyString += "2113";
          break;
        case 'e':
          keyString += "13";
          break;
        case 'f':
          keyString += "11213";
          break;
        case 'g':
          keyString += "2213";
          break;
        case 'h':
          keyString += "11113";
          break;
        case 'i':
          keyString += "113";
          break;
        case 'j':
          keyString += "12223";
          break;
        case 'k':
          keyString += "2123";
          break;
        case 'l':
          keyString += "12113";
          break;
        case 'm':
          keyString += "223";
          break;
        case 'n':
          keyString += "213";
          break;
        case 'o':
          keyString += "2223";
          break;
        case 'p':
          keyString += "12213";
          break;
        case 'q':
          keyString += "22123";
          break;
        case 'r':
          keyString += "1213";
          break;
        case 's':
          keyString += "1113";
          break;
        case 't':
          keyString += "23";
          break;
        case 'u':
          keyString += "1123";
          break;
        case 'v':
          keyString += "11123";
          break;
        case 'w':
          keyString += "1223";
          break;
        case 'x':
          keyString += "21123";
          break;
        case 'y':
          keyString += "21223";
          break;
        case 'z':
          keyString += "22113";
          break;
        case '1':
          keyString += "122223";
          break;
        case '2':
          keyString += "112223";
          break;
        case '3':
          keyString += "111223";
          break;
        case '4':
          keyString += "111123";
          break;
        case '5':
          keyString += "111113";
          break;
        case '6':
          keyString += "211113";
          break;
        case '7':
          keyString += "221113";
          break;
        case '8':
          keyString += "222113";
          break;
        case '9':
          keyString += "222213";
          break;
        case '0':
          keyString += "222223";
          break;
        case '.':
          keyString += "1212123";
          break;
        case ',':
          keyString += "2211223";
          break;
        case '?':
          keyString += "1122113";
          break;
        case '!':
          keyString += "2121223";
          break;
        case ';':
          keyString += "2121213";
          break;
        case ':':
          keyString += "2221113";
          break;
        case '/':
          keyString += "211213";
          break;
        case '-':
          keyString += "2111123";
          break;
        case '=':
          keyString += "211123";
          break;
        case '\'':
          keyString += "1222213";
          break;
        case '_':
          keyString += "1122123";
          break;
        case '&':
          keyString += "121113";
          break;
        case '"':
          keyString += "1211213";
          break;
        case '$':
          keyString += "11121123";
          break;
        case '+':
          keyString += "121213";
          break;
        case '@':
          keyString += "1221213";
          break;
        case '(':
          keyString += "212213";
          break;
        case ')':
          keyString += "2122123";
          break;
        case ' ':
          keyString += "0";
          break;
        default:
          break;
      }
    }
    Serial.println(keyString);
  }

  // Serial inputed keys
  if (keyString.length()) {
    if (keyString[0] == '3') {
      delay(unit * 3);
    } else if (keyString[0] == '0') {
      delay(unit * 7);
    } else if (keyString[0] == '1') {
      startSignal(key1);
      delay(unit);
      stopSignal();
    } else if (keyString[0] == '2') {
      startSignal(key2);
      delay(unit * 3);
      stopSignal();
    }
    delay(unit);
    keyString.remove(0,1);
  }
}

void oscillatePaddle() {
  if (mode == 2 && !ditState && !dahState && queue) {
    if (queue == 1) { // dit is in the queue
      if (ditStart < dahStart || ditStop - ditStart > unit * 4) {
        queue = 0;
      }
    } else { // dah is in the queue
      if (dahStart < ditStart || dahStop - dahStart > unit * 2) {
        queue = 0;
      }
    }
  }
  if (ditState) {
    if (!queue) {
      if ((!dahState && !sending && millis() - signalStopTime >= unit) || lastKey == 2) {
        queue = 1;
      }
    } else {
      if (mode == 2 && !dahState && dahStart < ditStart && queue == 2) {
        queue = 0;
      }
    }
  }
  if (dahState) {
    if (!queue) {
      if ((!ditState && !sending && millis() - signalStopTime >= unit) || lastKey == 1) {
        queue = 2;
      }
    } else {
      if (mode == 2 && !ditState && ditStart < dahStart && queue == 1) {
        queue = 0;
      }
    }
  }
  if (!sending && millis() - signalStopTime >= unit) {
    processQueue();
  } else if (sending) { // middle of sending
    if ((lastKey == 1 && millis() - signalStartTime >= unit) || (lastKey == 2 && millis() - signalStartTime >= unit * 3)) { // send length achieved
      stopSignal();
    }
  }
}

void processQueue() {
  if (!sending && queue) { // not sending, something in queue, not in pause state
    lastKey = queue;
    queue = 0;
    startSignal(key1);
  }
}

void startSignal(char key) {
  sending = true;
  signalStartTime = millis();
  if (speaker) {
    tone(audioPin, note);
  }
  if (kbEmulator) {
    Keyboard.press(key);
  }
  if (keyer) {
    digitalWrite(keyerPin, HIGH);
  }
}

void stopSignal() {
  sending = false;
  signalStopTime = millis();
  noTone(audioPin);
  Keyboard.releaseAll();
  digitalWrite(keyerPin, LOW);
}
