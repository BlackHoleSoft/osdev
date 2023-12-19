#!/bin/sh
# Run in alpine linux
# Before running clone this repo to ~/osdev and build program with build.sh

mkdir ~/rootfs
cd ~/rootfs
mkdir -p bin dev mnt proc sys tmp sbin lib

echo '#!/bin/sh' > init
#echo 'dmesg -n 1' >> init
echo 'mount -t devtmpfs none /dev' >> init
echo 'mount -t proc none /proc' >> init
echo 'mount -t sysfs none /sys' >> init
echo 'mknod /dev/console c 5 1' >> init
echo 'mknod /dev/null c 1 3' >> init
echo 'echo "...running /sbin/init"' >> init
echo 'exec /sbin/init' >> init
echo 'echo "INTERNAL ERROR!!! Cannot run /sbin/init."' >> init

chmod +x ./init

cp /bin/sh ./bin
cp ~/osdev/a.out ./sbin/init
#cp /lib/ld-musl-x86_64.so.1 ./lib
#cp /lib/libc.musl-x86_64.so.1 ./lib
ldd /bin/sh | grep "=> /" | awk '{print $3}' | xargs -I '{}' cp -v '{}' ./lib

find . | cpio -R root:root -H newc -o | gzip > ~/rootfs.gz

cd /mnt
mkdir boot
cd boot

cp ~/rootfs.gz .
cp /boot/vmlinuz-lts .

grub-install --boot-directory=/mnt/boot /dev/sdb

echo 'set timeout=5' > grub/grub.cfg
echo 'menuentry "Strelka with custom rootfs" {' >> grub/grub.cfg
echo '  linux /boot/vmlinuz-lts root=/dev/sda1 rw' >> grub/grub.cfg
echo '  initrd /boot/rootfs.gz' >> grub/grub.cfg
echo '}' >> grub/grub.cfg

echo 'Configuration done. Reboot your system and boot from configured disk'