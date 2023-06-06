#include "config_mode_interpreter.h"

uint8_t ConfigModeInterpreter::num_clicks_;
uint32_t ConfigModeInterpreter::click_time_;
uint8_t ConfigModeInterpreter::click_counter{0};
uint32_t ConfigModeInterpreter::start_clicks_time{0};
uint32_t ConfigModeInterpreter::last_interrupt_time{0};