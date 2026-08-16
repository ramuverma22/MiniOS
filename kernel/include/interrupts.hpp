#pragma once
#include "types.hpp"

namespace minios {
void interrupts_initialize();
void interrupts_enable();
void interrupts_disable();
u32 timer_ticks();
}

extern "C" void keyboard_irq_entry();
extern "C" void timer_irq_entry();
