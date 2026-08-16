.section .text

.global keyboard_irq_entry
.type keyboard_irq_entry, @function
.extern keyboard_interrupt_handler

keyboard_irq_entry:
    pusha
    call keyboard_interrupt_handler

    # Send End Of Interrupt to the master PIC.
    mov $0x20, %al
    mov $0x20, %dx
    out %al, %dx

    popa
    iret


.global timer_irq_entry
.type timer_irq_entry, @function
.extern timer_interrupt_handler

timer_irq_entry:
    pusha
    call timer_interrupt_handler
    popa
    iret