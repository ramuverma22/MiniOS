#pragma once
#include "types.hpp"

namespace minios {
class Terminal {
public:
    void initialize();
    void write(const char* text);
    void put(char character);
    void newline();
    void set_color(u8 foreground, u8 background = 0);
    void clear();

private:
    static constexpr u16 Width = 80;
    static constexpr u16 Height = 25;
    u16 row_ = 0;
    u16 column_ = 0;
    u8 color_ = 0x0F;
    volatile u16* const buffer_ = reinterpret_cast<volatile u16*>(0xB8000);

    void scroll();
};

void print_number(Terminal& terminal, u32 value);
void print_hex(Terminal& terminal, u32 value);
}
