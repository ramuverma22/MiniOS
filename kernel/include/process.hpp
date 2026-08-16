#pragma once

#include "types.hpp"

namespace minios {

class Terminal;

struct Process {
    u32 pid;
    const char* name;
    bool active;
};

class ProcessManager {
public:
    void initialize();

    void list(Terminal& terminal) const;

    bool kill(u32 pid);

    u32 count() const {
        return count_;
    }

private:
    static constexpr u32 MaxProcesses = 8;

    Process processes_[MaxProcesses]{};

    u32 count_ = 0;
};

} // namespace minios
