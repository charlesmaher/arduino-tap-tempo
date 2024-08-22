#ifndef TempoController_h
#define TempoController_h

#include <Arduino.h>
#include <Adafruit_LEDBackpack.h>

class TempoController
{
private:
    uint8_t buttonPin;

public:
    uint8_t prevButton; // Value of last digitalRead()
    unsigned long lastChange = 0L;

    unsigned long prevBeat = 0L; // Time of last button tap
    uint16_t numBeats = 0; // Number of beats counted
    unsigned long sum = 0L; // Cumulative time of all beats
    bool wantsDraw = false;

    Adafruit_AlphaNum4 disp;
    TempoController(uint8_t address, uint8_t tempoPin);
    void tick();
    void draw();
    void clearData();
};

#endif