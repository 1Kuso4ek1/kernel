nasm -f elf32 ../src/kernel.asm -o asmkernel.o
x86_64-linux-gnu-g++-12 -m32 -I../include -c ../src/kernel.cpp -o ckernel.o
x86_64-linux-gnu-ld -m elf_i386 -T ../link.ld -o kernel.bin asmkernel.o ckernel.o -z noexecstack
#qemu-system-x86_64 -kernel kernel
cp kernel.bin ../iso/boot
grub-mkrescue -o ../sus_os.iso ../iso
qemu-system-x86_64 -cdrom ../sus_os.iso -m 32M

