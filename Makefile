kernel_source_files := $(shell find src/kernel -name *.c)
kernel_object_files := $(patsubst src/kernel/%.c, build/kernel/%.o, $(kernel_source_files))

asm_source_files := $(shell find src/asm -name *.asm)
asm_object_files := $(patsubst src/asm/%.asm, build/%.o, $(asm_source_files))

$(kernel_object_files): build/kernel/%.o : src/kernel/%.c
	mkdir -p $(dir $@) && \
	gcc -m32 -c -ffreestanding -O0 $(patsubst build/kernel/%.o, src/kernel/%.c, $@) -o $@

$(asm_object_files): build/%.o : src/asm/%.asm
	mkdir -p $(dir $@) && \
	nasm -f elf32 $(patsubst build/%.o, src/asm/%.asm, $@) -o $@

.PHONY: build clear
build: $(kernel_object_files) $(asm_object_files)
	mkdir -p dist/x86 && \
	ld -m elf_i386 -n -o dist/x86/kernel.bin -T targets/x86/linker.ld $(kernel_object_files) $(asm_object_files) && \
	cp dist/x86/kernel.bin targets/x86/iso/boot/kernel.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/x86/kernel.iso targets/x86/iso

clear:
	rm -rf build
