# MiniOS

A small 32-bit operating system built from scratch in C++.

This project was created to understand how basic operating system components work at a low level. The kernel boots using GRUB and provides a simple command-line shell for interacting with the system.

## Features

- 32-bit x86 kernel
- GRUB bootloader
- VGA text terminal
- Keyboard input
- Interrupt handling
- Command-line shell
- Process manager
- Process listing with `ps`
- Process termination with `kill`
- Heap memory management
- Dynamic memory allocation and deallocation
- Memory information
- System uptime
- Basic system information
- `echo` command
- `clear` command

## Commands

```text
help
clear
info
uptime
echo
memory
heap
alloc
free
ps
kill
```

Examples:

```text
minios> info
minios> memory
minios> heap
minios> alloc 100
minios> ps
minios> kill 3
minios> uptime
minios> echo Hello MiniOS
```

## Project Structure

```text
MiniOS/
├── boot/
│   └── boot.s
├── kernel/
│   ├── arch/
│   │   └── interrupts.s
│   ├── include/
│   │   ├── types.hpp
│   │   ├── terminal.hpp
│   │   ├── memory.hpp
│   │   ├── process.hpp
│   │   ├── keyboard.hpp
│   │   └── interrupts.hpp
│   ├── src/
│   │   ├── kernel.cpp
│   │   ├── terminal.cpp
│   │   ├── memory.cpp
│   │   ├── process.cpp
│   │   ├── keyboard.cpp
│   │   └── interrupts.cpp
│   └── linker.ld
├── Makefile
├── grub.cfg
└── README.md
```

## How It Works

The system starts with the boot code and loads the kernel through GRUB.

After the kernel starts, it initializes:

1. Terminal
2. Keyboard
3. Interrupts
4. Memory manager
5. Process manager
6. Shell

After initialization, MiniOS displays:

```text
minios>
```

Commands entered by the user are read from the keyboard and processed by the kernel shell.

## Memory Management

MiniOS contains a simple heap allocator.

Memory can be allocated using:

```text
alloc 100
```

The shell returns the allocated memory address.

Heap usage can be checked using:

```text
heap
```

Allocated memory can be released using:

```text
free 0x0010700C
```

The heap keeps track of used and available memory.

## Process Management

MiniOS includes a simple process manager with a small process table.

The process list can be viewed using:

```text
ps
```

Example:

```text
Process List
--------------------------------
PID   NAME     STATUS
--------------------------------
1     kernel   RUNNING
2     idle     READY
3     shell    READY
```

A process can be terminated using its PID:

```text
kill 3
```

The kernel process cannot be terminated.

## Building

The project can be built in a Linux or WSL environment with the required tools installed.

```bash
make
```

This creates:

```text
build/minios.iso
```

## Running

Run MiniOS using QEMU:

```bash
make run
```

QEMU will start the kernel and display the MiniOS shell.

## Technologies

- C++
- x86 Assembly
- GRUB
- QEMU
- Make
- Linux / WSL

## What I Learned

This project helped me understand:

- Kernel booting
- x86 architecture
- Interrupt handling
- Keyboard input
- VGA text output
- Memory management
- Heap allocation
- Process management
- Shell implementation

## Future Improvements

- Process scheduling
- Paging and virtual memory
- File system support
- System calls
- User-mode programs
- Additional device drivers
- Improved shell

## Screenshots

### MiniOS Boot

![MiniOS Boot](https://github.com/ramuverma22/MiniOS/raw/refs/heads/main/docs/MiniOS_Screenshots/boot.png)

### Shell Commands

![Shell Commands](https://github.com/ramuverma22/MiniOS/raw/refs/heads/main/docs/MiniOS_Screenshots/shell-commands.png)

### System Information

![System Information](https://github.com/ramuverma22/MiniOS/raw/refs/heads/main/docs/MiniOS_Screenshots/shell-info.png)

## Author

Ramu Verma

IIT Kanpur
