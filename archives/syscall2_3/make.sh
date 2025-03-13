#!/bin/bash

nasm -f elf64 syscall_x86_64_linux.asm
gcc -Wall -static -nostdlib syscall_test.c syscall_x86_64_linux.o -o syscall_test