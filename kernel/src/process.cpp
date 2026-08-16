#include "process.hpp"
#include "terminal.hpp"

namespace minios {

/*
 * Initialize the process table.
 */
void ProcessManager::initialize() {

    count_ = 3;

    /*
     * Process 1 - kernel
     */
    processes_[0].pid = 1;
    processes_[0].name = "kernel";
    processes_[0].active = true;

    /*
     * Process 2 - idle
     */
    processes_[1].pid = 2;
    processes_[1].name = "idle";
    processes_[1].active = true;

    /*
     * Process 3 - shell
     */
    processes_[2].pid = 3;
    processes_[2].name = "shell";
    processes_[2].active = true;

    /*
     * Clear remaining process slots.
     */
    for (u32 i = 3; i < MaxProcesses; ++i) {

        processes_[i].pid = 0;
        processes_[i].name = nullptr;
        processes_[i].active = false;
    }
}


/*
 * Kill a process using its PID.
 */
bool ProcessManager::kill(u32 pid) {

    /*
     * Never allow the kernel to be killed.
     */
    if (pid == 1) {
        return false;
    }

    for (u32 i = 0; i < MaxProcesses; ++i) {

        if (processes_[i].active &&
            processes_[i].pid == pid) {

            processes_[i].active = false;

            if (count_ > 0) {
                --count_;
            }

            return true;
        }
    }

    return false;
}


/*
 * Display process list.
 */
void ProcessManager::list(Terminal& terminal) const {

    terminal.newline();

    terminal.write("Process List");
    terminal.newline();

    terminal.write("--------------------------------");
    terminal.newline();

    terminal.write("PID   NAME     STATUS");
    terminal.newline();

    terminal.write("--------------------------------");
    terminal.newline();

    for (u32 i = 0; i < MaxProcesses; ++i) {

        if (!processes_[i].active) {
            continue;
        }

        /*
         * Print PID using the existing
         * MiniOS print_number() function.
         */
        minios::print_number(
            terminal,
            processes_[i].pid
        );

        terminal.write("     ");

        /*
         * Print process name.
         */
        terminal.write(
            processes_[i].name
        );

        terminal.write("     ");

        /*
         * Print status.
         */
        if (processes_[i].pid == 1) {

            terminal.write("RUNNING");

        } else {

            terminal.write("READY");
        }

        terminal.newline();
    }
}

} // namespace minios
