#pragma once

#include "types.hpp"

namespace minios {

// Declare the interrupt handler with C linkage BEFORE the class.
// This makes the friend declaration below refer to the same function.
extern "C" void keyboard_interrupt_handler();

class Keyboard {
public:
    void initialize();
    char read_char();
    bool available() const;

private:
    static constexpr u32 BufferSize = 128;

    volatile char buffer_[BufferSize]{};
    volatile u32 head_ = 0;
    volatile u32 tail_ = 0;
    bool shift_ = false;

    void push(char c);

    friend void keyboard_interrupt_handler();
};

extern Keyboard* g_keyboard;

} // namespace minios