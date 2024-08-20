.set MAGIC, 0x1badb002  ;  #mandatory to boot with grub 
.set FLAGS, (1<<0 | 1<<1) 
.set CHECKSUM, -(MAGIC + FLAGS) ; #checksum + magic + flags = 0

.section .multiboot
  .long MAGIC
  .long FLAGS
  .long CHECKSUM

.section .text
.extern kernelMain
.extern callConstructors
.global loader

loader:
  mov $kernel_stack , %esp
  
  call callConstructors

  push %eax
  push %ebx ; # send multiboot structure and magic number to kernel main
  call kernelMain

_stop: ; # infinite loop in case kernel die
  cli
  hlt
  jmp _stop

.section .bss
.space 2*1024*1024 ; # 2 MiB 
kernel_stack: 

