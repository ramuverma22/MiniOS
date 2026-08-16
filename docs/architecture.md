# MiniOS architecture

## Boot path

1. GRUB loads `kernel.elf` using Multiboot v1.
2. `boot/boot.s` creates a 16 KiB kernel stack and calls `kernel_main`.
3. The C++ kernel validates the Multiboot magic value and initializes services.
4. The PIC is remapped so hardware IRQs do not overlap CPU exception vectors.
5. The PIT generates IRQ0 at 100 Hz.
6. The PS/2 keyboard generates IRQ1 events which are translated into characters and placed into a ring buffer.
7. The shell consumes characters from the buffer and executes built-in commands.

## Why interrupts are used

The first version polled the keyboard controller in a tight loop. Version 0.2 uses hardware interrupts instead: the CPU can halt while the shell waits for input, and the keyboard interrupt wakes the kernel path when a key arrives. This is a more realistic operating-system design.

## Current process model

`ProcessManager` contains a small table representing the kernel and shell. It is deliberately not called a scheduler: there are no saved CPU contexts, address spaces, or context switches yet.

## Safety boundaries

- Freestanding C++: no libc, heap, exceptions, or RTTI.
- Kernel linked at 1 MiB.
- Keyboard ring buffer is bounded and drops new input if full.
- Shell input is bounded to 63 characters plus a null terminator.
- Memory reporting only uses Multiboot memory information when its flag is present.

## Planned architecture

```text
User programs (Ring 3)
        |
   System calls
        |
Kernel (Ring 0)
  |     |      |
 VMM  Scheduler  FS
  |      |       |
Paging  Timer  Block I/O
```
