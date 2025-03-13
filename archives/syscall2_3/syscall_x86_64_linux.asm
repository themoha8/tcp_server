global errno
global sys_write
global sys_exit
global sys_read
global sys_mmap
global sys_munmap
global _start

extern main

;syscall6:
;		mov		rax, rdi	; trap
;		mov		rdi, rsi	; arg1
;		mov		rsi, rdx	; arg2
;		mov		rdx, rcx	; arg3
;		mov		rcx, r8		; arg4
;		mov		r8, r9		; arg5
;		mov		r9, [rsp+8] ; arg6
;		syscall
;		cmp		rax, -4095
;		jbe		.ok
;		mov		[errno], rax
;		mov		rax, -1
;		jmp		.quit
;.ok:	mov		qword [errno], 0
;.quit:	ret

syscall6:
		syscall
		cmp		rax, -4095
		jbe		.ok
		mov		[errno], rax
		mov		rax, -1
		jmp		.quit
.ok:	mov		qword [errno], 0
.quit:	ret

sys_write:
		mov 	rax, s_write
		jmp 	syscall6

sys_exit:
		mov 	rax, s_exit
		jmp 	syscall6

sys_read:
		mov		rax, s_read
		jmp		syscall6

sys_mmap:
		mov		rax, s_mmap
		jmp		syscall6

sys_munmap:
		mov		rax, s_munmap
		jmp		syscall6
	
section .bss
errno	resq	1

section .data
s_read		equ 0x0
s_write		equ	0x1
s_mmap		equ 0x9
s_munmap	equ	0xb
s_exit		equ 0x60

section .text
_start:
		mov		rdi, [rsp]	; argc in rdi
		mov		rsi, rsp 	; argc address
		add		rsi, 8 		; argv in rsi (argc address + 8)	
		call	main
		mov		rdi, rax
		mov		rax, 60
		syscall
