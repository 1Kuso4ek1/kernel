nasm -f elf32 ../src/kernel.asm -o asmkernel.o
g++ -m32 -I../include -c ../src/kernel.cpp -o ckernel.o
g++ -m32 -I../include -c ../src/mouse.cpp -o mouse.o
ld -m elf_i386 -T ../link.ld -o kernel.bin asmkernel.o ckernel.o mouse.o -z noexecstack
cp kernel.bin ../iso/boot
grub-mkrescue -o ../sus_os.iso ../iso -d /usr/lib/grub/i386-pc
qemu-system-x86_64 -cdrom ../sus_os.iso -m 512M
