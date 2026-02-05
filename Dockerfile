# Linux build example
# https://github.com/masoudr/embedded-linux-image-builder

FROM debian:bookworm

RUN apt-get -y update
RUN apt-get -y install --no-install-recommends \
    linux-image-amd64
RUN apt-get -y install xorriso grub-pc-bin grub-common
RUN apt-get -y install curl build-essential

# Установка rustup + немедленное использование cargo в том же слое
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | \
    sh -s -- -y --profile minimal --default-toolchain stable && \
    /root/.cargo/bin/cargo --version && \
    /root/.cargo/bin/rustc --version

# Постоянное добавление в PATH для всех последующих слоёв
ENV PATH="/root/.cargo/bin:${PATH}"

# Install WASMI
RUN cargo install wasmi_cli

RUN mkdir -p /output/iso/boot
RUN mkdir /initrd

WORKDIR /initrd

COPY ./iso /output/iso
COPY ./init /initrd
COPY ./add.wasm /initrd

RUN mkdir -p bin dev mnt proc sys tmp sbin lib lib64 usr/bin usr/lib
# Debug
RUN cp -rf /bin/* /initrd/bin
RUN cp -rf /lib/* /initrd/lib
RUN cp -rf /lib64/* /initrd/lib64
# RUN cp -rf /usr/* /initrd/usr


# Release
# RUN cp /bin/sh /initrd/bin/sh
# RUN ldd /bin/sh | grep "=> /" | awk '{print $3}' | xargs -I '{}' cp -v '{}' /initrd/lib
# linux-vdso.so.1
# RUN cp /lib64/ld-linux-x86-64.so.2 /initrd/lib64/ld-linux-x86-64.so.2
# RUN ldd /usr/bin/ld | grep "=> /" | awk '{print $3}' | xargs -I '{}' cp -v '{}' /initrd/lib

RUN cp /root/.cargo/bin/wasmi_cli /initrd/bin/wasmi_cli
RUN ldd /root/.cargo/bin/wasmi_cli | grep "=> /" | awk '{print $3}' | xargs -I '{}' cp -v '{}' /initrd/lib

RUN chmod +x /initrd/init

RUN find . | cpio -R root:root -H newc -o | gzip > /output/iso/boot/rootfs.gz

RUN cp /vmlinuz /output/iso/boot

RUN grub-mkrescue /usr/lib/grub/i386-pc -o /output/strelka.iso /output/iso

CMD cp /output/strelka.iso /vol