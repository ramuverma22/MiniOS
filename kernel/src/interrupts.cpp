#include "interrupts.hpp"
#include "io.hpp"
#include "keyboard.hpp"

namespace minios {

namespace {

struct [[gnu::packed]] IdtEntry {
    u16 offset_low;
    u16 selector;
    u8 zero;
    u8 type_attr;
    u16 offset_high;
};

struct [[gnu::packed]] IdtPointer {
    u16 limit;
    u32 base;
};

IdtEntry idt[256]{};

volatile u32 ticks = 0;

void set_gate(u8 vector, u32 handler, u8 flags = 0x8E) {
    idt[vector].offset_low =
        static_cast<u16>(handler & 0xFFFF);

    // Kernel code segment used by our GDT.
    idt[vector].selector = 0x10;

    idt[vector].zero = 0;

    idt[vector].type_attr = flags;

    idt[vector].offset_high =
        static_cast<u16>((handler >> 16) & 0xFFFF);
}

void pic_remap() {
    using io::inb;
    using io::outb;

    const u8 master_mask = inb(0x21);
    const u8 slave_mask = inb(0xA1);

    // Start PIC initialization sequence.
    outb(0x20, 0x11);
    io::io_wait();

    outb(0xA0, 0x11);
    io::io_wait();

    // Master PIC -> interrupts 0x20-0x27.
    outb(0x21, 0x20);
    io::io_wait();

    // Slave PIC -> interrupts 0x28-0x2F.
    outb(0xA1, 0x28);
    io::io_wait();

    // Tell master that slave is connected to IRQ2.
    outb(0x21, 0x04);
    io::io_wait();

    // Tell slave its cascade identity.
    outb(0xA1, 0x02);
    io::io_wait();

    // Use 8086 mode.
    outb(0x21, 0x01);
    io::io_wait();

    outb(0xA1, 0x01);
    io::io_wait();

    /*
     * Enable:
     * IRQ0 = timer
     * IRQ1 = keyboard
     *
     * Keep the remaining interrupts masked.
     */
    outb(0x21, static_cast<u8>(master_mask & 0xFC));

    // Keep all slave IRQs masked.
    outb(0xA1, slave_mask);
}

void pit_initialize() {
    constexpr u32 input_hz = 1193182;
    constexpr u32 frequency = 100;

    const u16 divisor =
        static_cast<u16>(input_hz / frequency);

    // Channel 0, low byte/high byte, square wave mode.
    io::outb(0x43, 0x36);

    // Send divisor.
    io::outb(
        0x40,
        static_cast<u8>(divisor & 0xFF)
    );

    io::outb(
        0x40,
        static_cast<u8>(divisor >> 8)
    );
}

} // anonymous namespace

void interrupts_initialize() {
    // Clear IDT.
    for (u32 i = 0; i < 256; ++i) {
        idt[i] = {};
    }

    /*
     * IMPORTANT:
     *
     * Our GDT uses 0x10 as the kernel code segment.
     */
    set_gate(
        32,
        reinterpret_cast<u32>(&timer_irq_entry)
    );

    set_gate(
        33,
        reinterpret_cast<u32>(&keyboard_irq_entry)
    );

    // Remap PIC before enabling interrupts.
    pic_remap();

    // Configure timer.
    pit_initialize();

    // Load IDT.
    IdtPointer pointer{
        static_cast<u16>(sizeof(idt) - 1),
        reinterpret_cast<u32>(&idt)
    };

    asm volatile(
        "lidt %0"
        :
        : "m"(pointer)
    );
}

void interrupts_enable() {
    asm volatile("sti");
}

void interrupts_disable() {
    asm volatile("cli");
}

u32 timer_ticks() {
    return ticks;
}

extern "C" void timer_interrupt_handler() {
    ++ticks;

    // Send End Of Interrupt to master PIC.
    io::outb(0x20, 0x20);
}

} // namespace minios
