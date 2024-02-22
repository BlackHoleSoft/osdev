kernel_source_files := $(shell find src/kernel -name *.c)
kernel_object_files := $(patsubst src/kernel/%.c, build/kernel/%.o, $(kernel_source_files))

asm_source_files := $(shell find src/asm -name *.asm)
asm_object_files := $(patsubst src/asm/%.asm, build/%.o, $(asm_source_files))

$(kernel_object_files): build/kernel/%.o : src/kernel/%.c
	rm -rf build && \
	mkdir -p $(dir $@) && \
	x86_64-elf-gcc -c -ffreestanding $(patsubst build/kernel/%.o, src/kernel/%.c, $@) -o $@

$(asm_object_files): build/%.o : src/asm/%.asm
	mkdir -p $(dir $@) && \
	nasm -f elf64 $(patsubst build/%.o, src/asm/%.asm, $@) -o $@

.PHONY: build
build: $(kernel_object_files) $(asm_object_files)	
	mkdir -p dist/x86_64 && \
	x86_64-elf-ld -n -o dist/x86_64/kernel.bin -T targets/x86_64/linker.ld $(kernel_object_files) $(asm_object_files) && \
	cp dist/x86_64/kernel.bin targets/x86_64/iso/boot/kernel.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/x86_64/kernel.iso targets/x86_64/iso
