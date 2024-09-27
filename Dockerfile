FROM alpine:3.14

RUN apk update
RUN apk add linux-lts
RUN apk add xorriso grub grub-bios

# install glibc
#RUN wget -q -O /etc/apk/keys/sgerrand.rsa.pub https://alpine-pkgs.sgerrand.com/sgerrand.rsa.pub
#RUN wget https://github.com/sgerrand/alpine-pkg-glibc/releases/download/2.35-r1/glibc-2.35-r1.apk
#RUN apk add glibc-2.35-r1.apk

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
COPY ./strelka-build/strelka-electron-0.1.0.AppImage .
COPY ./node ./node
COPY ./xorg ./xorg
COPY ./startx ./xorg
COPY ./X ./xorg
COPY ./xorg-conf ./xorg/conf
COPY ./xinitrc .
COPY ./xorg.conf .
COPY ./prebuilt ./electron

# Put all to /strelkasys
# Generates something in /output
RUN mkdir /strelkasys
RUN mkdir /strelkasys/initrd
WORKDIR /strelkasys/initrd
RUN mkdir -p bin dev/pts dev/shm mnt proc sys tmp sbin lib usr var/log usr/lib/xorg/modules usr/share/X11 etc/X11 electron

RUN cp -rf /bin/* /strelkasys/initrd/bin
RUN cp /output/node/node /strelkasys/initrd/bin
RUN cp -rf /output/node/lib/* /strelkasys/initrd/lib
RUN cp /output/init /strelkasys/initrd
RUN cp /output/xorg/startx /strelkasys/initrd/bin
RUN cp /output/xorg/X /strelkasys/initrd/bin
RUN cp /output/xorg/mcookie /strelkasys/initrd/bin
RUN cp /output/xorg/xinit /strelkasys/initrd/bin
RUN cp /output/xorg/xterm /strelkasys/initrd/bin
RUN cp -rf /output/xorg/lib/* /strelkasys/initrd/lib
RUN cp -rf /output/xorg/usr/* /strelkasys/initrd/usr
RUN cp -rf /output/xorg/modules/* /strelkasys/initrd/usr/lib/xorg/modules
RUN cp /output/xinitrc /strelkasys/initrd
RUN cp /output/xorg.conf /strelkasys/initrd/etc/X11
RUN cp -rf /usr/lib/* /strelkasys/initrd/lib
RUN cp -rf /output/xorg/share/* /strelkasys/initrd/usr/share/X11
RUN cp -rf /output/xorg/conf/* /strelkasys/initrd/usr/share/X11/xorg.conf.d

#RUN cp /output/strelka-electron-0.1.0.AppImage /strelkasys/initrd/strelka.AppImage
#RUN sed 's|AI\x02|\x00\x00\x00|g' -i /strelkasys/initrd/strelka
RUN chmod +x /strelkasys/initrd/init
#RUN chmod +x /strelkasys/initrd/strelka.AppImage

RUN cp -rf /output/electron/* /strelkasys/initrd/electron

RUN ldd /strelkasys/initrd/strelka.AppImage | grep "=> /" | awk '{print $3}' | xargs -I '{}' cp -v '{}' /strelkasys/initrd/lib

RUN find . | cpio -R root:root -H newc -o | gzip > /strelkasys/rootfs.gz

RUN cp /strelkasys/rootfs.gz /output/iso/boot
RUN cp /boot/vmlinuz-lts /output/iso/boot
# original initrd: initramfs-lts
#RUN cp -rf /strelkasys/initrd/* /output/iso
RUN grub-mkrescue /usr/lib/grub/i386-pc -o /output/strelka.iso /output/iso

CMD cp /output/strelka.iso /vol

