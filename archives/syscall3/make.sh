#!/bin/bash

gcc -Wall -O2 -Wextra -Wpedantic -Werror -ansi -std=c89 -static -nostdlib syscall_test.c syscall.c -o syscall_test