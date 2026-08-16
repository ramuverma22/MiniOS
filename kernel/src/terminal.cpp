#include "terminal.hpp"

namespace minios {
namespace {
constexpr u16 vga_entry(char c, u8 color) {
    return static_cast<u16>(static_cast<u8>(c)) | (static_cast<u16>(color) << 8);
}
}

void Terminal::initialize() {
    color_ = 0x0F;
    clear();
}

void Terminal::set_color(u8 foreground, u8 background) {
    color_ = static_cast<u8>((foreground & 0x0F) | ((background & 0x0F) << 4));
}

void Terminal::clear() {
    row_ = 0;
    column_ = 0;
    for (u16 y = 0; y < Height; ++y) {
        for (u16 x = 0; x < Width; ++x) {
            buffer_[y * Width + x] = vga_entry(' ', color_);
        }
    }
}

void Terminal::scroll() {
    for (u16 y = 1; y < Height; ++y) {
        for (u16 x = 0; x < Width; ++x) {
            buffer_[(y - 1) * Width + x] = buffer_[y * Width + x];
        }
    }
    for (u16 x = 0; x < Width; ++x) {
        buffer_[(Height - 1) * Width + x] = vga_entry(' ', color_);
    }
    row_ = Height - 1;
}

void Terminal::newline() {
    column_ = 0;
    if (++row_ >= Height) scroll();
}

void Terminal::put(char c) {
    if (c == '\n') {
        newline();
        return;
    }
    if (c == '\b') {
        if (column_ > 0) {
            --column_;
            buffer_[row_ * Width + column_] = vga_entry(' ', color_);
        }
        return;
    }
    buffer_[row_ * Width + column_] = vga_entry(c, color_);
    if (++column_ >= Width) newline();
}

void Terminal::write(const char* text) {
    while (*text) put(*text++);
}

void print_number(Terminal& terminal, u32 value) {
    char digits[10];
    u32 count = 0;
    if (value == 0) { terminal.put('0'); return; }
    while (value && count < sizeof(digits)) {
        digits[count++] = static_cast<char>('0' + value % 10);
        value /= 10;
    }
    while (count) terminal.put(digits[--count]);
}

void print_hex(Terminal& terminal, u32 value) {
    static constexpr char hex[] = "0123456789ABCDEF";
    terminal.write("0x");
    for (int shift = 28; shift >= 0; shift -= 4) {
        terminal.put(hex[(value >> shift) & 0xF]);
    }
}
}
