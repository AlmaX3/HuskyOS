CC=x86_64-elf-g++
LD=x86_64-elf-ld
CFLAGS=-ffreestanding -m64 -O2 -Wall -Wextra -fno-exceptions -fno-rtti -mno-red-zone -nostdlib -lgcc -g -r -fno-stack-protector -fno-pic -mabi=sysv -mno-80387 -mno-mmx -mno-3dnow  -mno-red-zone -mcmodel=kernel -MMD
INTERNALLDFLAGS=-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -mno-red-zone -nostdlib -lgcc -g -z max-page-size=0x1000

hi:
	$(CC) -I Kernel/include/ Kernel/src/*.cpp Kernel/src/klib/*.cpp Kernel/src/drivers/*.cpp Kernel/src/memory/*.cpp Kernel/src/terminal/*.cpp Kernel/src/cpu/gdt/*.cpp Kernel/src/cpu/idt/*.cpp Kernel/src/cpu/isr/*.cpp Kernel/src/cpu/pci/*.cpp  Kernel/src/cpu/acpi/*.cpp Kernel/src/cpu/pic/*.cpp Kernel/src/time/*.cpp Kernel/src/fs/*.cpp -o hkernel.o $(CFLAGS)
	nasm -f elf64 Kernel/src/cpu/gdt/*.asm -o gdt.o
	nasm -f elf64 Kernel/src/cpu/idt/*.asm -o idt.o
	nasm -f elf64 Kernel/src/cpu/isr/*.asm -o isr.o
	nasm -f elf64 Kernel/src/cpu/coprocessor/*.asm -o coprocessor.o
	$(LD) -r -b binary -o unifont.o Kernel/src/font/unifont.sfn
	$(CC) -T linker.ld -o husky.elf $(INTERNALLDFLAGS) hkernel.o gdt.o coprocessor.o unifont.o idt.o isr.o


iso:
	cp -v husky.elf limine.cfg limine/limine.sys \
      limine/limine-cd.bin limine/limine-eltorito-efi.bin iso_root/
	
	cp -v limine/BOOTX64.EFI iso_root/EFI/BOOT/

	xorriso -as mkisofs -b limine-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-eltorito-efi.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso_root -o husky.iso
	
	./limine/limine-s2deploy husky.iso


Huskyrd:
	rm iso_root/initrd.img
	genext2fs -b 1024 -d initrd iso_root/initrd.img

run:
	qemu-system-x86_64 -machine q35 -cpu qemu64  -drive file=husky.iso -drive file=test.img -no-reboot -no-shutdown -m 4G -vga std -serial mon:stdio 

doall:
	make hi
	make Huskyrd
	make iso
