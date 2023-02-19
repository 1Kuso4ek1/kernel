nasm -f elf32 ../kernel.asm -o asmkernel.o
x86_64-linux-gnu-gcc -m32 -c ../kernel.c -o ckernel.o
x86_64-linux-gnu-ld -m elf_i386 -T ../link.ld -o kernel asmkernel.o ckernel.o
#qemu-system-x86_64 -kernel kernel
sudo mount /dev/loop11p1 ../tmp
sudo cp kernel ../tmp
sudo umount /dev/loop11p1
qemu-system-x86_64 -drive format=raw,file=../hdd.img -vga cirrus

