#!/bin/bash

gcc -Wall -g -static -nostdlib -c syscall1.c
gcc -Wall -g -static -nostdlib prog1.c syscall1.o -o prog1
