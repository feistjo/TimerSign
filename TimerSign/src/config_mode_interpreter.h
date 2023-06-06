#include <Arduino.h>

class ConfigModeInterpreter
{
public:
    ConfigModeInterpreter(uint8_t pin_num, uint8_t num_clicks, uint32_t click_time)
    {
        num_clicks_ = num_clicks;
        click_time_ = click_time;
        pinMode(pin_num, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(pin_num), onInterrupt, FALLING);
    }

    bool IsConfigMode()
    {
        if (click_counter == num_clicks_ && millis() - start_clicks_time > click_time_)
        {
            return true;
        }
        return false;
    }

private:
    static uint8_t num_clicks_;
    static uint32_t click_time_;
    static uint8_t click_counter;
    static uint32_t start_clicks_time;
    static uint32_t last_interrupt_time;
    static void onInterrupt()
    {
        if (millis() - last_interrupt_time < 200)
        {
            return;
        }
        last_interrupt_time = millis();
        if (millis() - start_clicks_time > click_time_)
        {
            click_counter = 0;
            start_clicks_time = millis();
        }
        click_counter++;
        Serial.println(click_counter);
    }
};