#include <Arduino.h>
#include <TM1637Display.h>

#include "config_mode_interpreter.h"
#include "timer.h"
#include "web_interface.h"

// Display pins
#define CLK 7
#define DIO 6

// Input pins
#define DOOR_SENSOR 5
#define PIR 4

TM1637Display display(CLK, DIO);

Timer timer(60 * 60 * 1000, []() { return millis(); });  // 60 minutes in milliseconds

WebInterface web_interface(/* "Timer", "thegarage2311", */ timer);

ConfigModeInterpreter config_mode_interpreter(9, 7, 10000);

void setup()
{
    pinMode(DOOR_SENSOR, INPUT_PULLUP);
    pinMode(PIR, INPUT);
    Serial.begin();
    Serial.println("Timer starting");
    web_interface.Initialize();
    timer.Reset();
    display.setBrightness(0x0f);
}

void DisplayTime(uint32_t time, bool colon)
{
    display.showNumberDecEx(time / 60, colon << 6, true, 2, 0);
    display.showNumberDecEx(time % 60, colon << 6, true, 2, 2);
}

void loop()
{
    web_interface.MaintainConnection();
    web_interface.notifyClients();

    if (config_mode_interpreter.IsConfigMode())
    {
        IPAddress ip = WiFi.localIP();
        Serial.println(ip);
        display.showNumberHexEx(ip[0], 1 << 6, true, 2, 0);
        display.showNumberHexEx(ip[1], 1 << 6, true, 2, 2);
        delay(2000);
        display.showNumberHexEx(ip[2], 1 << 6, true, 2, 0);
        display.showNumberHexEx(ip[3], 1 << 6, true, 2, 2);
        delay(2000);
        return;
    }

    if (digitalRead(DOOR_SENSOR) /*  || !digitalRead(PIR) */)
    {
        timer.Reset();
        display.clear();
        delay(500);
        return;
    }

    int32_t time_remaining = timer.GetTimeRemaining() / 1000;  // in seconds
    Serial.printf(
        "Time remaining: %d, door: %d, pir: %d\n", time_remaining, digitalRead(DOOR_SENSOR), digitalRead(PIR));

    if (time_remaining <= 0)
    {
        DisplayTime(0, true);
        delay(500);
        display.clear();
        delay(500);
    }
    else
    {
        DisplayTime(time_remaining, true);
        delay(500);
        DisplayTime(time_remaining, false);
        delay(500);
    }
}
