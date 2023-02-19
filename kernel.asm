bits 32
section .text
	align 4
	dd 0x1BADB002               ;magic
	dd 0x02                     ;flags
	dd -(0x1BADB002 + 0x02) ;checksum

	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	
	dd 0
	dd 640 ; width
	dd 400 ; height
	dd 32  ; bpp

global read_port
global write_port

global load_idt
global call_kb_handle

global start

extern kmain
extern kb_handle

read_port:
    mov edx, [esp + 4]
    in al, dx   
    ret

write_port:
    mov edx, [esp + 4]    
    mov al, [esp + 8]  
    out dx, al  
    ret

load_idt:
	mov edx, [esp + 4]
	lidt [edx]
	sti
	ret

call_kb_handle:
	call kb_handle
	iretd

start:
	cli
	mov esp, stack_space
	push 0x0
	popf
	push ebx ;mbi
	push eax ;magic
	call kmain
	hlt

section .bss
resb 8192
stack_space:
