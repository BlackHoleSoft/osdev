FROM alpine:3.14

RUN apk update
RUN apk add linux-lts
RUN apk add xorriso grub grub-bios

# building nodejs
# RUN apk add g++ make
# RUN wget https://nodejs.org/dist/v14.16.1/node-v14.16.1.tar.gz
# RUN tar -xvf node-v14.16.1.tar.gz
# WORKDIR /node-v14.16.1
# RUN ./configure
# RUN make build -j4
# RUN make install


WORKDIR /output
COPY ./iso ./iso
COPY ./init .
COPY ./entry.js .
COPY ./node ./node

# Put all to /strelkasys
# Generates something in /output
RUN mkdir /strelkasys
RUN mkdir /strelkasys/initrd
WORKDIR /strelkasys/initrd
RUN mkdir -p bin dev mnt proc sys tmp sbin lib

RUN cp -rf /bin/* /strelkasys/initrd/bin
RUN cp /output/node/node /strelkasys/initrd/bin
RUN cp /output/node/lib/* /strelkasys/initrd/lib
#RUN ldd /bin/sh | grep "=> /" | awk '{print $3}' | xargs -I '{}' cp -v '{}' /strelkasys/initrd/lib
RUN cp /output/init /strelkasys/initrd
RUN cp /output/entry.js /strelkasys/initrd
RUN chmod +x /strelkasys/initrd/init

RUN find . | cpio -R root:root -H newc -o | gzip > /strelkasys/rootfs.gz

RUN cp /strelkasys/rootfs.gz /output/iso/boot
RUN cp /boot/vmlinuz-lts /output/iso/boot
# original initrd: initramfs-lts
RUN cp -rf /strelkasys/initrd/* /output/iso
RUN grub-mkrescue /usr/lib/grub/i386-pc -o /output/strelka.iso /output/iso

CMD cp /output/strelka.iso /vol

