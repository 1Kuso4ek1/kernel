bits 32
section .multiboot
	align 4
	dd 0x1BADB002               ;magic
	dd 7                     ;flags
	dd -(0x1BADB002 + 7) ;checksum

	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	
	dd 0
	dd 640 ; width
	dd 480 ; height
	dd 32  ; bpp

section .text

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
	pushad
    cld
    call kb_handle
    popad
    iretd

start:
	lgdt [gdtr]

	jmp CODE32_SEL:.setcs
.setcs:
    mov ax, DATA32_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
	mov esp, stack_space
	mov ecx, stack_space
	push 0x0
	popf
	push ebx ;mbi
	push eax ;magic
	push ecx ;stack pointer
	call kmain
	hlt

%define MAKE_GDT_DESC(base, limit, access, flags) \
    (((base & 0x00FFFFFF) << 16) | \
    ((base & 0xFF000000) << 32) | \
    (limit & 0x0000FFFF) | \
    ((limit & 0x000F0000) << 32) | \
    ((access & 0xFF) << 40) | \
    ((flags & 0x0F) << 52))

section .data
align 4
gdt_start:
    dq MAKE_GDT_DESC(0, 0, 0, 0)
gdt32_code:
    dq MAKE_GDT_DESC(0, 0x00ffffff, 10011010b, 1100b)
gdt32_data:
    dq MAKE_GDT_DESC(0, 0x00ffffff, 10010010b, 1100b)
end_of_gdt:

gdtr:
    dw end_of_gdt - gdt_start - 1
    dd gdt_start

CODE32_SEL equ gdt32_code - gdt_start
DATA32_SEL equ gdt32_data - gdt_start

section .bss
resb 8192
stack_space:
