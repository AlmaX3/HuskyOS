# Husky Operating System

A hobby operating system that runs on x86-64. The main goal is to have a modern general purpose Operating System with the philosophy of "An Operating System is a friend and a tool."

### Goals
#### Near Future
- [x] "Hello World" on screen
- [x] Paging
- [ ] Framebuffer
- [ ] Global Descriptor Table
- [ ] IDT
- [ ] ISR
- [ ] Interrupts
- [ ] Keyboard support
- [ ] Mouse support
- [ ] ACPI
- [ ] VFS
- [ ] File System (Ext2)
- [ ] Elf Loader

#### Future
- [ ] Userspace
- [ ] GUI
- [ ] Custom executables

### Specifications
Husky uses stivale2 and limine. It boots up a UEFI application, which will start the kernel. If the motherboard has only a BIOS on it, then, stivale2 will boot up the BIOS version of the Operating System