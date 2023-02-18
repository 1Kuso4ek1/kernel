nasm -f elf32 ../kernel.asm -o asmkernel.o
x86_64-linux-gnu-gcc -m32 -c ../kernel.c -o ckernel.o
x86_64-linux-gnu-ld -m elf_i386 -T ../link.ld -o kernel asmkernel.o ckernel.o
qemu-system-x86_64 -kernel kernel
