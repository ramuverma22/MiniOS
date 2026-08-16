#pragma once

#include "types.hpp"
#include "terminal.hpp"
#include "memory.hpp"
#include "process.hpp"
#include "keyboard.hpp"
#include "interrupts.hpp"

namespace minios {

[[noreturn]] void halt();

}

extern "C" void kernel_main(
    unsigned int magic,
    unsigned int multiboot_info
);
