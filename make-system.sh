#!/bin/sh
# Run in alpine linux
# Before running install node and npm (via nvs) and clone this repo to ~/osdev

mkfs.ext4 /dev/sdb1
mount /dev/sdb1 /mnt

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
echo 'echo "Running /sbin/init"' >> init
echo 'exec /sbin/init' >> init
echo 'echo "INTERNAL ERROR!!! Cannot run /sbin/init."' >> init

chmod +x ./init

echo '#!/bin/sh' > ./sbin/init
echo 'node /test.js' >> ./sbin/init

chmod +x ./sbin/init

echo 'console.log("Test script running..."); while (true) {}' > ./test.js

cp /bin/sh ./bin
cp /bin/ls ./bin
cp /bin/mount ./bin
cp /bin/mknod ./bin
cp /bin/mkdir ./bin
cp /bin/echo ./bin
cp /usr/bin/node ./bin

echo 'Copying libs...'
#cp /lib/ld-musl-x86_64.so.1 ./lib
#cp /lib/libc.musl-x86_64.so.1 ./lib
ldd /usr/bin/node | grep "=> /" | awk '{print $3}' | xargs -I '{}' cp -v '{}' ./lib

find . | cpio -R root:root -H newc -o | gzip > ~/rootfs.gz

cd /mnt
mkdir boot
cd boot

cp ~/rootfs.gz .
cp /boot/vmlinuz-lts .

grub-install --boot-directory=/mnt/boot /dev/sdb

echo 'set timeout=10' > grub/grub.cfg
echo 'menuentry "Strelka (node based)" {' >> grub/grub.cfg
echo '  linux /boot/vmlinuz-lts root=/dev/sda1 rw' >> grub/grub.cfg
echo '  initrd /boot/rootfs.gz' >> grub/grub.cfg
echo '}' >> grub/grub.cfg

echo 'Configuration done. Reboot your system and boot from configured disk'