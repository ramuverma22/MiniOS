#include "terminal.hpp"
#include "process.hpp"
#include "keyboard.hpp"
#include "interrupts.hpp"
#include "memory.hpp"
#include <cstdint>

namespace minios {

namespace {

/*
 * Check whether text starts with prefix.
 */
bool starts_with(const char* text, const char* prefix) {

    while (*prefix) {

        if (*text != *prefix) {
            return false;
        }

        ++text;
        ++prefix;
    }

    return true;
}


/*
 * Compare two strings.
 */
bool string_equals(const char* a, const char* b) {

    while (*a && *b) {

        if (*a != *b) {
            return false;
        }

        ++a;
        ++b;
    }

    return *a == *b;
}


/*
 * Print shell prompt.
 */
void shell_print_prompt(Terminal& terminal) {

    terminal.write("minios> ");
}


/*
 * Convert decimal text to number.
 */
u32 parse_decimal(const char* text) {

    u32 value = 0;

    while (*text) {

        if (*text < '0' || *text > '9') {
            break;
        }

        value =
            value * 10 +
            static_cast<u32>(*text - '0');

        ++text;
    }

    return value;
}


/*
 * Convert hexadecimal text to address.
 */
uintptr_t parse_hex(const char* text) {

    uintptr_t value = 0;

    while (*text == ' ') {
        ++text;
    }

    /*
     * Skip 0x / 0X.
     */
    if (text[0] == '0' &&
        (text[1] == 'x' || text[1] == 'X')) {

        text += 2;
    }

    while (*text) {

        char c = *text;
        u32 digit = 0;

        if (c >= '0' && c <= '9') {

            digit =
                static_cast<u32>(c - '0');

        } else if (c >= 'a' && c <= 'f') {

            digit =
                static_cast<u32>(c - 'a' + 10);

        } else if (c >= 'A' && c <= 'F') {

            digit =
                static_cast<u32>(c - 'A' + 10);

        } else {

            break;
        }

        value =
            (value << 4) |
            static_cast<uintptr_t>(digit);

        ++text;
    }

    return value;
}


/*
 * HELP command.
 */
void shell_help(Terminal& terminal) {

    terminal.newline();

    terminal.write("Available commands:");
    terminal.newline();

    terminal.write(
        "  help     - Show available commands"
    );
    terminal.newline();

    terminal.write(
        "  clear    - Clear the screen"
    );
    terminal.newline();

    terminal.write(
        "  info     - Show MiniOS information"
    );
    terminal.newline();

    terminal.write(
        "  uptime   - Show system uptime"
    );
    terminal.newline();

    terminal.write(
        "  echo     - Print text"
    );
    terminal.newline();

    terminal.write(
        "  memory   - Show memory information"
    );
    terminal.newline();

    terminal.write(
        "  heap     - Show heap information"
    );
    terminal.newline();

    terminal.write(
        "  alloc    - Allocate heap memory"
    );
    terminal.newline();

    terminal.write(
        "  free     - Free allocated heap memory"
    );
    terminal.newline();

    terminal.write(
        "  ps       - Show running processes"
    );
    terminal.newline();

    terminal.write(
        "  kill     - Terminate a process"
    );
    terminal.newline();
}


/*
 * INFO command.
 */
void shell_info(Terminal& terminal) {

    terminal.newline();

    terminal.write("MiniOS Information");
    terminal.newline();

    terminal.write("------------------");
    terminal.newline();

    terminal.write("Architecture: i386");
    terminal.newline();

    terminal.write("Kernel: MiniOS");
    terminal.newline();

    terminal.write("Language: C++17");
    terminal.newline();

    terminal.write("Bootloader: GRUB");
    terminal.newline();

    terminal.write("Status: RUNNING");
    terminal.newline();
}


/*
 * UPTIME command.
 */
void shell_uptime(Terminal& terminal) {

    terminal.newline();

    terminal.write("Timer ticks: ");

    print_number(
        terminal,
        timer_ticks()
    );

    terminal.newline();
}


/*
 * MEMORY command.
 */
void shell_memory(
    Terminal& terminal,
    MemoryManager& memory
) {

    terminal.newline();

    terminal.write("Memory Information");
    terminal.newline();

    terminal.write("------------------");
    terminal.newline();

    if (!memory.information_available()) {

        terminal.write(
            "Memory information unavailable."
        );

        terminal.newline();

        return;
    }

    terminal.write("Total memory: ");

    print_number(
        terminal,
        memory.total_memory_kib()
    );

    terminal.write(" KiB");
    terminal.newline();

    terminal.write("Total memory: ");

    print_number(
        terminal,
        memory.total_memory_kib() / 1024
    );

    terminal.write(" MiB");
    terminal.newline();
}


/*
 * HEAP command.
 */
void shell_heap(
    Terminal& terminal,
    MemoryManager& memory
) {

    terminal.newline();

    terminal.write("Heap Information");
    terminal.newline();

    terminal.write("----------------");
    terminal.newline();

    terminal.write("Heap size: ");

    print_number(
        terminal,
        memory.heap_size()
    );

    terminal.write(" bytes");
    terminal.newline();

    terminal.write("Heap used: ");

    print_number(
        terminal,
        memory.heap_used()
    );

    terminal.write(" bytes");
    terminal.newline();

    terminal.write("Heap free: ");

    print_number(
        terminal,
        memory.heap_free()
    );

    terminal.write(" bytes");
    terminal.newline();
}


/*
 * ALLOC command.
 *
 * Usage:
 * alloc 100
 */
void shell_alloc(
    Terminal& terminal,
    MemoryManager& memory,
    const char* command
) {

    const char* text = command + 5;

    while (*text == ' ') {
        ++text;
    }

    terminal.newline();

    if (*text == '\0') {

        terminal.write(
            "Usage: alloc <bytes>"
        );

        terminal.newline();

        return;
    }

    u32 size =
        parse_decimal(text);

    if (size == 0) {

        terminal.write(
            "Invalid allocation size."
        );

        terminal.newline();

        return;
    }

    void* pointer =
        memory.allocate(size);

    if (pointer == nullptr) {

        terminal.write(
            "Allocation failed."
        );

        terminal.newline();

        return;
    }

    terminal.write("Allocated ");

    print_number(
        terminal,
        size
    );

    terminal.write(" bytes.");
    terminal.newline();

    terminal.write("Address: ");

    print_hex(
        terminal,
        static_cast<u32>(
            reinterpret_cast<uintptr_t>(
                pointer
            )
        )
    );

    terminal.newline();
}


/*
 * FREE command.
 *
 * Usage:
 * free 0x0010700C
 */
void shell_free(
    Terminal& terminal,
    MemoryManager& memory,
    const char* command
) {

    const char* text = command + 4;

    while (*text == ' ') {
        ++text;
    }

    terminal.newline();

    if (*text == '\0') {

        terminal.write(
            "Usage: free <address>"
        );

        terminal.newline();

        return;
    }

    uintptr_t address =
        parse_hex(text);

    if (address == 0) {

        terminal.write(
            "Invalid address."
        );

        terminal.newline();

        return;
    }

    memory.deallocate(
        reinterpret_cast<void*>(address)
    );

    terminal.write(
        "Memory freed."
    );

    terminal.newline();
}


/*
 * PS command.
 */
void shell_ps(
    Terminal& terminal,
    ProcessManager& processes
) {

    processes.list(terminal);
}


/*
 * KILL command.
 *
 * Usage:
 * kill 3
 */
void shell_kill(
    Terminal& terminal,
    ProcessManager& processes,
    const char* command
) {

    /*
     * Skip "kill".
     */
    const char* text = command + 4;

    while (*text == ' ') {
        ++text;
    }

    terminal.newline();

    if (*text == '\0') {

        terminal.write(
            "Usage: kill <PID>"
        );

        terminal.newline();

        return;
    }

    u32 pid =
        parse_decimal(text);

    if (pid == 0) {

        terminal.write(
            "Invalid PID."
        );

        terminal.newline();

        return;
    }

    if (processes.kill(pid)) {

        terminal.write(
            "Process terminated."
        );

    } else {

        terminal.write(
            "Unable to terminate process."
        );
    }

    terminal.newline();
}

} // anonymous namespace


/*
 * Kernel entry point.
 */
extern "C"
void kernel_main(
    unsigned int magic,
    unsigned int multiboot_info
) {

    /*
     * Avoid unused parameter warning.
     */
    (void)magic;


    /*
     * Terminal.
     */
    Terminal terminal;

    terminal.initialize();

    terminal.set_color(15, 0);


    /*
     * MiniOS title.
     */
    terminal.write(
        "======================================"
    );

    terminal.newline();

    terminal.write(
        "              MiniOS Kernel"
    );

    terminal.newline();

    terminal.write(
        "======================================"
    );

    terminal.newline();

    terminal.newline();


    /*
     * Kernel startup.
     */
    terminal.write(
        "Kernel booted successfully!"
    );

    terminal.newline();

    terminal.write(
        "Terminal initialized successfully."
    );

    terminal.newline();


    /*
     * Keyboard.
     */
    Keyboard keyboard;

    keyboard.initialize();

    terminal.write(
        "Keyboard initialized successfully."
    );

    terminal.newline();


    /*
     * Interrupts.
     */
    interrupts_initialize();

    terminal.write(
        "Interrupts initialized successfully."
    );

    terminal.newline();

    interrupts_enable();


    /*
     * Memory manager.
     */
    MemoryManager memory;

    memory.initialize(
        static_cast<u32>(
            multiboot_info
        )
    );


    /*
     * Process manager.
     */
    ProcessManager processes;

    processes.initialize();


    /*
     * Welcome message.
     */
    terminal.newline();

    terminal.write(
        "Welcome to MiniOS!"
    );

    terminal.newline();

    terminal.newline();

    terminal.write(
        "System status: RUNNING"
    );

    terminal.newline();

    terminal.newline();


    /*
     * Command buffer.
     */
    char command[128];

    u32 length = 0;

    command[0] = '\0';


    /*
     * First prompt.
     */
    shell_print_prompt(terminal);


    /*
     * Main shell loop.
     */
    while (true) {

        /*
         * Wait for keyboard input.
         */
        if (!keyboard.available()) {

            asm volatile("hlt");

            continue;
        }


        /*
         * Read character.
         */
        char c =
            keyboard.read_char();


        /*
         * ENTER.
         */
        if (c == '\n') {

            command[length] = '\0';


            /*
             * HELP
             */
            if (string_equals(
                    command,
                    "help")) {

                shell_help(terminal);
            }


            /*
             * CLEAR
             */
            else if (string_equals(
                         command,
                         "clear")) {

                terminal.clear();
            }


            /*
             * INFO
             */
            else if (string_equals(
                         command,
                         "info")) {

                shell_info(terminal);
            }


            /*
             * UPTIME
             */
            else if (string_equals(
                         command,
                         "uptime")) {

                shell_uptime(terminal);
            }


            /*
             * MEMORY
             */
            else if (string_equals(
                         command,
                         "memory")) {

                shell_memory(
                    terminal,
                    memory
                );
            }


            /*
             * HEAP
             */
            else if (string_equals(
                         command,
                         "heap")) {

                shell_heap(
                    terminal,
                    memory
                );
            }


            /*
             * PS
             */
            else if (string_equals(
                         command,
                         "ps")) {

                shell_ps(
                    terminal,
                    processes
                );
            }


            /*
             * ECHO
             */
            else if (starts_with(
                         command,
                         "echo ")) {

                terminal.newline();

                terminal.write(
                    command + 5
                );

                terminal.newline();
            }


            /*
             * ALLOC
             */
            else if (starts_with(
                         command,
                         "alloc ")) {

                shell_alloc(
                    terminal,
                    memory,
                    command
                );
            }


            /*
             * FREE
             */
            else if (starts_with(
                         command,
                         "free ")) {

                shell_free(
                    terminal,
                    memory,
                    command
                );
            }


            /*
             * KILL
             */
            else if (starts_with(
                         command,
                         "kill ")) {

                shell_kill(
                    terminal,
                    processes,
                    command
                );
            }


            /*
             * Empty command.
             */
            else if (length == 0) {

                /*
                 * Nothing to do.
                 */
            }


            /*
             * Unknown command.
             */
            else {

                terminal.newline();

                terminal.write(
                    "Unknown command: "
                );

                terminal.write(command);

                terminal.newline();

                terminal.write(
                    "Type 'help' for available commands."
                );

                terminal.newline();
            }


            /*
             * Reset command buffer.
             */
            length = 0;

            command[0] = '\0';


            /*
             * Show prompt.
             */
            shell_print_prompt(terminal);

            continue;
        }


        /*
         * BACKSPACE.
         */
        if (c == '\b') {

            if (length > 0) {

                --length;

                terminal.put('\b');
            }

            continue;
        }


        /*
         * Normal character.
         */
        if (length < sizeof(command) - 1) {

            command[length++] = c;

            terminal.put(c);
        }
    }
}

} // namespace minios
