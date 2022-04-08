# Husky Operating System

A hobby operating system that runs on x86-64.

### Goals
#### Near Future
- [x] "Hello World" on screen
- [ ] Paging
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
Husky uses stivale2 and limine. It boots up a UEFI application, which will start the kernel.