#include "keyboard.hpp"
#include "io.hpp"

namespace minios {

namespace {

const char normal_map[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b','\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n',0,'a','s',
    'd','f','g','h','j','k','l',';','\'', '`',0,'\\','z','x','c','v',
    'b','n','m',',','.','/',0,'*',0,' '
};

const char shift_map[128] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b','\t',
    'Q','W','E','R','T','Y','U','I','O','P','{','}','\n',0,'A','S',
    'D','F','G','H','J','K','L',':','"','~',0,'|','Z','X','C','V',
    'B','N','M','<','>','?',0,'*',0,' '
};

}

Keyboard* g_keyboard = nullptr;

void Keyboard::initialize() {
    head_ = 0;
    tail_ = 0;
    shift_ = false;
    g_keyboard = this;
}

bool Keyboard::available() const {
    return head_ != tail_;
}

void Keyboard::push(char c) {
    const u32 next = (head_ + 1) % BufferSize;

    if (next == tail_) {
        return;
    }

    buffer_[head_] = c;
    head_ = next;
}

char Keyboard::read_char() {
    while (!available()) {
        asm volatile("hlt");
    }

    const char c = buffer_[tail_];
    tail_ = (tail_ + 1) % BufferSize;

    return c;
}

extern "C" void keyboard_interrupt_handler() {
    if (!g_keyboard) {
        return;
    }

    const u8 scan = io::inb(0x60);

    // Left/right Shift pressed
    if (scan == 0x2A || scan == 0x36) {
        g_keyboard->shift_ = true;
        return;
    }

    // Left/right Shift released
    if (scan == 0xAA || scan == 0xB6) {
        g_keyboard->shift_ = false;
        return;
    }

    // Key release
    if (scan & 0x80U) {
        return;
    }

    if (scan >= 128) {
        return;
    }

    const char c = g_keyboard->shift_
        ? shift_map[scan]
        : normal_map[scan];

    if (c) {
        g_keyboard->push(c);
    }
}

} // namespace minios