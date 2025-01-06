.section .text
.global _start

_start:
    # Write "Hello, World!\n" to stdout
    li a7, 64           # syscall number for write
    li a0, 1           # file descriptor (stdout)
    la a1, message     # buffer address
    li a2, 14          # length
    ecall

    # Exit program
    li a7, 93          # syscall number for exit
    li a0, 0           # exit code 0
    ecall

.section .data
message:
    .string "Hello, World!\n"
