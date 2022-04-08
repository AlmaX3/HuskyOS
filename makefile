CC=x86_64-elf-g++
CFLAGS=-ffreestanding -m64 -O2 -Wall -Wextra -fno-exceptions -fno-rtti -mno-red-zone -nostdlib -lgcc -g -r -fno-stack-protector -fno-pic -mabi=sysv -mno-80387 -mno-mmx -mno-3dnow  -mno-red-zone -mcmodel=kernel -MMD
INTERNALLDFLAGS=-ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -mno-red-zone -nostdlib -lgcc -g -z max-page-size=0x1000

hi:
	$(CC) -I Kernel/include/ Kernel/src/*.cpp Kernel/src/klib/*.cpp Kernel/src/memory/*.cpp Kernel/src/terminal/*.cpp Kernel/src/cpu/gdt/*.cpp -o hkernel.o $(CFLAGS)
	nasm -f elf64 Kernel/src/cpu/gdt/*.asm -o gdt.o
	nasm -f elf64 Kernel/src/memory/*.asm -o memory.o
	$(CC) -T linker.ld -o husky.elf $(INTERNALLDFLAGS) hkernel.o gdt.o memory.o  


makeIso:
	cp -v husky.elf limine.cfg limine/limine.sys \
      limine/limine-cd.bin limine/limine-eltorito-efi.bin iso_root/
	
	cp -v limine/BOOTX64.EFI iso_root/EFI/BOOT/

	xorriso -as mkisofs -b limine-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-eltorito-efi.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso_root -o husky.iso
	
	./limine/limine-s2deploy husky.iso


run:
	qemu-system-x86_64 -cpu qemu64 -drive format=raw,media=cdrom,file=husky.iso -no-reboot -no-shutdown -m 8G -vga std -d int

doall:
	make hi
	make makeIso