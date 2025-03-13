#!/bin/bash

gcc -Wall -g -static -nostdlib -c syscall2.c
gcc -Wall -g -static -nostdlib prog2.c syscall2.o -o prog2
