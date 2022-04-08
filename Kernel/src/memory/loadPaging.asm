BITS 64

section .init
	pop rbp
	ret


global  _init:function
_init:
	push rbp
	mov rbp, rsp


section .fini
	pop rbp
	ret

global  _fini:function
_fini:
	push rbp
	mov rbp, rsp