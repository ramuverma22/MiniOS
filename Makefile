CXX = i686-linux-gnu-g++
AS = i686-linux-gnu-as
LD = i686-linux-gnu-ld

CXXFLAGS = -std=gnu++17 \
           -ffreestanding \
           -fno-exceptions \
           -fno-rtti \
           -fno-stack-protector \
           -fno-pie \
           -fno-pic \
           -m32 \
           -march=i686 \
           -Wall \
           -Wextra \
           -Werror \
           -O2 \
           -Ikernel/include

ASFLAGS = --32

LDFLAGS = -m elf_i386 \
          -T kernel/linker.ld \
          -z max-page-size=0x1000

BUILD = build

KERNEL = $(BUILD)/kernel.elf
ISO = $(BUILD)/minios.iso

OBJECTS = \
	$(BUILD)/boot.o \
	$(BUILD)/interrupts_asm.o \
	$(BUILD)/kernel.o \
	$(BUILD)/terminal.o \
	$(BUILD)/memory.o \
	$(BUILD)/process.o \
	$(BUILD)/keyboard.o \
	$(BUILD)/interrupts.o

.PHONY: all clean run

all: $(ISO)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: boot/boot.s | $(BUILD)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD)/interrupts_asm.o: kernel/arch/interrupts.s | $(BUILD)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD)/kernel.o: kernel/src/kernel.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/terminal.o: kernel/src/terminal.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/memory.o: kernel/src/memory.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/process.o: kernel/src/process.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/keyboard.o: kernel/src/keyboard.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/interrupts.o: kernel/src/interrupts.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(KERNEL): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@

$(ISO): $(KERNEL) grub.cfg
	rm -rf $(BUILD)/isodir
	mkdir -p $(BUILD)/isodir/boot/grub
	cp $(KERNEL) $(BUILD)/isodir/boot/kernel.elf
	cp grub.cfg $(BUILD)/isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(BUILD)/isodir

clean:
	rm -rf $(BUILD)

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)
