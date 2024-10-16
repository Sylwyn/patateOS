GPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -fno-stack-protector

ASPARAMS = --32

LDPARAMS = -melf_i386

objects = loader.o gdt.o port.o kernel.o


#compilation of cpp files
%.o: %.cpp
	g++ $(GPARAMS) -o $@ -c $<

#compilation of assembly files
%.o: %.s
	as $(ASPARAMS) -o $@ $<

#linking into kernel binary file
mykernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

#creation of iso file for vm
mykernel.iso: mykernel.bin
	mkdir --parents iso
	mkdir --parents iso/boot
	mkdir --parents iso/boot/grub
	cp $< iso/boot
	echo 'set timeout=0' >> iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	#echo 'GRUB_TERMINAL_INPUT="console serial"' >> iso/boot/grub/grub.cfg
	#echo 'GRUB_GFXMODE=text' >> iso/boot/grub/grub.cfg
	#echo 'set pager=1' >> iso/boot/grub/grub.cfg
	#echo 'set deubg=all' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "MyOS UwU" {' >> iso/boot/grub/grub.cfg
	echo ' multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
	echo ' boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso


#launch VM containing the os ( only after having configurated it in virtual box ) 
run: mykernel.iso
	(killall VirtualBoxVM && sleep 1) || true
	VirtualBoxVM --startvm "newos2" &

.PHONY: clean
clean:
	rm -f $(objects) mykernel.bin mykernel.iso
