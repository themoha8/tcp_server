#!/bin/bash

gcc -Wall -g -static -nostdlib -c syscall2_2.c
gcc -Wall -g -static -nostdlib prog2_2.c syscall2_2.o -o prog2_2
