#!/bin/bash

nasm -f elf64 syscall_x86_64_linux.asm
gcc -Wall -g -static -nostdlib test1.c syscall_x86_64_linux.o -o test1
gcc -Wall -g -static -nostdlib test2.c syscall_x86_64_linux.o -o test2
