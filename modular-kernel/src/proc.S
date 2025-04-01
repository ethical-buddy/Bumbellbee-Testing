.global switch_context
.type switch_context, @function
switch_context:
    pushal          # Save general registers
    movl 4(%esp), %eax  # Load old context pointer
    movl %esp, (%eax)   # Save old ESP

    movl 8(%esp), %eax  # Load new context pointer
    movl (%eax), %esp   # Restore new ESP
    popal           # Restore registers
    ret             # Return to new process

