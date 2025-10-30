# Используем базовый образ с установленными необходимыми инструментами
FROM rustlang/rust:nightly as builder

# Устанавливаем необходимые системные зависимости
RUN apt-get update && apt-get install -y \
    build-essential \
    curl \
    file \
    xorriso \
    grub-pc-bin \
    grub-common \
    mtools \
    && rm -rf /var/lib/apt/lists/*

# Устанавливаем компоненты Rust
RUN rustup component add rust-src llvm-tools-preview
RUN rustup target add x86_64-unknown-none

# Устанавливаем bootimage для создания образа ядра
RUN cargo install bootimage

# Создаем директорию для работы
WORKDIR /app

# Копируем файлы проекта
COPY coreboot/ ./coreboot/

# Переходим в директорию проекта
WORKDIR /app/coreboot

# Создаем исполняемый файл из lib.rs, переименовывая его в main.rs
# RUN mv src/lib.rs src/main.rs

# Собираем проект с помощью bootimage
RUN cargo bootimage --target x86_64-coreboot.json --release

# Создаем структуру для GRUB
RUN mkdir -p iso_root/boot/grub

# Копируем образ ядра
RUN cp target/x86_64-coreboot/release/bootimage-coreboot.bin iso_root/boot/coreboot.bin

# Создаем конфигурацию GRUB
RUN echo -e "set timeout=0\nset default=0\n\nmenuentry 'coreboot' {\n    insmod part_msdos\n    insmod ext2\n    set root=(loop)\n    multiboot2 /boot/coreboot.bin\n    boot\n}" > iso_root/boot/grub/grub.cfg

RUN mkdir -p vol

# Создаем ISO образ с помощью grub-mkrescue
CMD grub-mkrescue -o /app/coreboot/vol/coreboot.iso iso_root && cp iso_root/boot/coreboot.bin vol/coreboot.bin


