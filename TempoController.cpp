#include "TempoController.h"

TempoController::TempoController(uint8_t address, uint8_t tempoPin)
{
    // Initialize button
    buttonPin = tempoPin;
    pinMode(buttonPin, INPUT_PULLUP);

    // Initialize display
    disp = Adafruit_AlphaNum4();
    disp.begin(address);
    disp.setBrightness(4);

    // Get button state
    prevButton = digitalRead(buttonPin);
}

void TempoController::tick()
{
    unsigned long now = micros();

    if (numBeats != 0 && now - prevBeat > 5000000L) {
        numBeats = 0;
        disp.setBrightness(4);
    }

    bool state = digitalRead(buttonPin);
    if (state == prevButton) return;
    if ((now - lastChange) < 50000L) return;
    lastChange = now;
    prevButton = state;

    // now debounced (theoretically)


    if (!state) {
        wantsDraw = true;
        if (numBeats == 0) {
            clearData();
            disp.setBrightness(10);
            prevBeat = now;
            numBeats = 1;
            return;
        }
        lastChange = now;
        numBeats++;
        sum += 600000000L / (now - prevBeat);
        prevBeat = now;
    }
}

void TempoController::draw()
{
    if (numBeats > 1)
    {
        int b = (sum / (numBeats - 1)); // Average time per tap
        if (b > 9999)
            b = 9999;
        for (int x = 3; x >= 0; x--)
        {
            if (b == 0 && x < 2)
            {
                disp.writeDigitAscii(x, ' '); // Blank instead of leading zeroes
                continue;
            }
            disp.writeDigitAscii(x, '0' + (b % 10), x == 2);
            b = b / 10;
        }
    }
    else
    {
        disp.clear();
        disp.writeDigitAscii(2, '-', true);
        disp.writeDigitAscii(3, '-');
    }
    disp.writeDisplay();
    wantsDraw = false;
}

void TempoController::clearData()
{
    disp.setBrightness(4);
    sum = 0;
    numBeats = 0;
}