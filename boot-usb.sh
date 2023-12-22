#!/bin/sh

mkfs.ext4 /dev/sdd1

mkdir /media/sdcard
mount /dev/sdd1 /media/sdcard

echo 'sdcard mounted'

cd /media/sdcard
mkdir boot
cd boot

cp ~/rootfs.gz .
#cp /boot/vmlinuz-lts .
cp /mnt/src/bzImage .

echo 'rootfs and kernel was copyied'

grub-install --boot-directory=/media/sdcard/boot /dev/sdd

echo 'set timeout=10' > grub/grub.cfg
echo 'menuentry "Strelka (node based)" {' >> grub/grub.cfg
echo '  set gfxpayload=1024x768x32' >> grub/grub.cfg
#echo '  linux /boot/vmlinuz-lts root=/dev/sda1 rw' >> grub/grub.cfg
echo '  linux /boot/bzImage root=/dev/sda1 rw noapic' >> grub/grub.cfg
echo '  initrd /boot/rootfs.gz' >> grub/grub.cfg
echo '}' >> grub/grub.cfg

echo 'Configuration done. Reboot your system and boot from configured disk'