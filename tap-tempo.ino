#include <Wire.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>
#include <avr/power.h>
#include "TempoController.h"

#define IDLE_DRAW_INTERVAL 300000L

uint8_t resetPin = 3;

bool idleDisplay = true;
unsigned long lastIdlePrint = 0;

const char idleString[] = "       BEATERMATCH";
const unsigned long idleStringSize = sizeof(idleString) - 1;

TempoController *tempoControllers[2] = {};
const unsigned long tempoControllerCount = sizeof(tempoControllers) / sizeof(*tempoControllers);

void setup()
{
  pinMode(resetPin, INPUT_PULLUP);
  tempoControllers[0] = new TempoController(0x70, 5);
  tempoControllers[1] = new TempoController(0x71, 6);
}

int idleIndex = 0;
void drawIdle()
{
  for (int i = 0; i < tempoControllerCount; i++)
  {
    Adafruit_AlphaNum4 *disp = &tempoControllers[i]->disp;
    // disp.clear();
    for (int ii = 0; ii < 4; ii++)
    {
      int ind = (i * 4 + ii + idleIndex) % idleStringSize;
      disp->writeDigitAscii(ii, idleString[ind]);
    }
    disp->writeDisplay();
  }
  idleIndex = (idleIndex + 1) % idleStringSize;
}

void loop()
{
  unsigned long now = micros();

  if (!digitalRead(resetPin)) {
    for (int i = 0; i < tempoControllerCount; i++) {
      tempoControllers[i]->clearData();
    }
    idleDisplay = true;
    idleIndex = 0;
  }

  bool anyDraw = false;
  for (int i = 0; i < tempoControllerCount; i++)
  {
    tempoControllers[i]->tick();
    if (tempoControllers[i]->wantsDraw)
      anyDraw = true;
  }

  if (anyDraw)
  {
    for (int i = 0; i < tempoControllerCount; i++) {
      TempoController *tc = tempoControllers[i];
      if (idleDisplay || tc->wantsDraw) tc->draw();
    }
    idleDisplay = false;
  }
  else if (idleDisplay && now - lastIdlePrint >= IDLE_DRAW_INTERVAL)
  {
    lastIdlePrint = now;
    drawIdle();
  }
}