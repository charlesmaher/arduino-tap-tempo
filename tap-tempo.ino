/*******************************************************************
  Tap tempo sketch for Arduino Uno.  Tap a button in sync with
  a beat, LED display shows beats per minute.  Stop tapping for
  4 second to reset counter.

  Required libraries include Adafruit_LEDBackpack and Adafruit_GFX.

  Pushbutton between Pin #5 and GND.
 *******************************************************************/

#include <Wire.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>
#include <avr/power.h>

Adafruit_AlphaNum4 disp = Adafruit_AlphaNum4();

unsigned long
  prevBeat = 0L, // Time of last button tap
  sum      = 0L; // Cumulative time of all beats
uint16_t
  nBeats   = 0;  // Number of beats counted
uint8_t
  prevButton;    // Value of last digitalRead()
uint8_t
  buttonPin = 5,
  resetPin = 3;

bool idleDisplay = true;

void setup() {
  disp.begin(0x70);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(resetPin, INPUT_PULLUP);
  prevButton = digitalRead(buttonPin);
}

static unsigned long debounce() { // Waits for change in button state
  uint8_t       b;
  unsigned long start, last, lastDraw;
  long          d, drawInterval = 300000L;

  start = micros();
  lastDraw = start - drawInterval;
  int drawIndex = 0;
  char text[] = "   TAP the BEAT  ";
  int textLength = sizeof(text) - 1;

  for(;;) {
    last = micros();
    while((b = digitalRead(buttonPin)) != prevButton) { // Button changed?
      if((micros() - last) > 25000L) {          // Sustained > 25 mS?
        prevButton = b;                         // Save new state
        return last;                            // Return time of change
      }
    } // Else button unchanged...do other things...

    bool resetPressed = !digitalRead(resetPin);

    d = (last - start) - 1500000L; // Function start time minus 1.5 sec
    if(d > 0 || resetPressed) {    // > 1.5 sec with no button change?
      nBeats = 0;                  // Reset counters
      prevBeat = sum = 0L;
    }

    if (resetPressed) {
      idleDisplay = true;
    } 

    // If no prior tap has been registered, program is waiting
    // for initial tap.  Show instructions on display.
    if(!prevBeat && idleDisplay && last - lastDraw > drawInterval) {
      for (int x = 0; x < 4; x++) {
        disp.writeDigitAscii(x, text[(drawIndex + x) % textLength]);
      }
      disp.writeDisplay();
      lastDraw = last;
      drawIndex = (drawIndex + 1) % textLength;
    }
  }
}

void loop() {
  unsigned long t;
  uint16_t      b;

  t = debounce(); // Wait for new button state
  idleDisplay = false;

  if(prevButton == HIGH) {             // Low-to-high (button tap)?
    if(prevBeat) {                     // Button tapped before?
      nBeats++;
      sum += 600000000L / (t - prevBeat); // BPM * 10
      b    = (sum / nBeats);              // Average time per tap
      if(b > 9999) b = 9999;
      for (int x = 3; x >= 0; x--) {
        if (b == 0 && x < 2) {
          disp.writeDigitAscii(x, ' '); // Blank instead of leading zeroes
          continue;
        }
        disp.writeDigitAscii(x, '0' + (b % 10), x == 2);
        b = b / 10;
      }
    } else {                               // First tap
      disp.clear();                        // Clear display, but...
      disp.writeDigitAscii(2, '-', true);  // a dot shows it's on
      disp.writeDigitAscii(3, '-');
    }
    disp.writeDisplay();
    prevBeat = t;                      // Record time of last tap
  }
}