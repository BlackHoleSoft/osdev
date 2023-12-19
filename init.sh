#!/bin/sh
#

# crucial mountpoints
mount -t proc none /proc
mount -t sysfs none /sys
mount -n tmpfs /var -t tmpfs -o size=17825792
mount -t tmpfs dev /dev

mknod /dev/console c 5 1
mknod /dev/ttyS0 c 4 64
mknod /dev/ttyS1 c 4 65

# setup console, consider using ptmx?
CIN=/dev/console
COUT=/dev/console
exec <$CIN &>$COUT

mknod /dev/null c 1 3
mknod /dev/gpio c 127 0

mknod /dev/zero c 1 5
mknod /dev/tty c 5 0
mknod /dev/tty0 c 4 0
mknod /dev/tty1 c 4 1
mknod /dev/random c 1 8
mknod /dev/urandom c 1 9
mknod /dev/ptmx c 5 2
mknod /dev/mem c 1 1

mknod /dev/watchdog c 10 130

mknod /dev/mtdblock0 b 31 0
mknod /dev/mtdblock1 b 31 1
mknod /dev/mtdblock2 b 31 2
mknod /dev/mtdblock3 b 31 3
mknod /dev/mtdblock4 b 31 4
mknod /dev/mtdblock5 b 31 5
mknod /dev/mtdblock6 b 31 6
mknod /dev/mtdblock7 b 31 7

mknod /dev/mtd0 c 90 0
mknod /dev/mtd1 c 90 2
mknod /dev/mtd2 c 90 4
mknod /dev/mtd3 c 90 6
mknod /dev/mtd4 c 90 8
mknod /dev/mtd5 c 90 10
mknod /dev/mtd6 c 90 12
mknod /dev/mtd7 c 90 14

mknod /dev/ttyUSB0 c 188 0
mknod /dev/ttyUSB1 c 188 1
mknod /dev/ttyUSB2 c 188 2
mknod /dev/ttyUSB3 c 188 3
mknod /dev/ttyUSB4 c 188 4
mknod /dev/ttyUSB5 c 188 5
mknod /dev/ttyUSB6 c 188 6

mknod /dev/ppp  c 108 0

mknod /dev/i2c-0 c 89 0
mknod /dev/i2c-1 c 89 1
mknod /dev/i2c-2 c 89 2
mknod /dev/i2c-3 c 89 3

#
# Create the ubnt-poll-host char dev entries
# The major number is 189
#
# NOTE: wifiN's minor number = N
#
mknod /dev/uph_wifi0 c 189 0

mkdir /dev/pts /dev/shm

# rest of the mounts
mount none /dev/pts -t devpts
if [ -e /proc/bus/usb ]; then
    mount none /proc/bus/usb -t usbfs
fi
echo "...mounts done"

mkdir -p /var/run /var/tmp /var/log /var/etc /var/etc/persistent /var/lock
echo "...filesystem init done"

# insert hal module
[ ! -f /lib/modules/*/ubnthal.ko ] || insmod /lib/modules/*/ubnthal.ko

# making sure that critical files are in place
mkdir -p /etc/rc.d /etc/init.d
# forced update
for f in inittab rc.d/rc.sysinit rc.d/rc rc.d/rc.stop ppp; do
        cp -f -r /usr/etc/$f /etc/$f
done
echo "...base ok"

mkdir -p /etc/udhcpc
# do not update if exist
for f in passwd group login.defs profile hosts host.conf \
fstab udhcpc/udhcpc startup.list; do
    if [ -e /etc/$f ]; then
        echo -n '.'
    else
        cp -f /usr/etc/$f /etc/$f
    fi
done
echo "...update ok"

mkdir -p /etc/sysinit
# make symlinks if do not exist
for f in services protocols shells mime.types ethertypes modules.d; do
    if [ -e /etc/$f ]; then
        echo -n '.'
    else
        ln -s /usr/etc/$f /etc/$f
    fi
done
echo "...symlinks ok"

mkdir -p /etc/httpd
# check if we have uploaded certificates
for f in server.crt server.key; do
    if [ -e /etc/persistent/https/$f ]; then
    ln -s /etc/persistent/https/$f /etc/httpd/$f
    else
    ln -s /usr/etc/$f /etc/httpd/$f
    fi
done
echo "...httpd ok"

CFG_SYSTEM="/tmp/system.cfg"
CFG_RUNNING="/tmp/running.cfg"
CFG_DEFAULT="/etc/default.cfg"

#Starting watchdog manager
/bin/watchdog -t 1 /dev/watchdog

# board data symlinks + default config
if [ -e /sbin/ubntconf ]; then
    /sbin/ubntconf -i $CFG_DEFAULT
    echo "ubntconf returned $?" >> /tmp/ubntconf.log
    echo "...detect ok"
fi

# System configuration
mkdir -p /etc/sysinit/
/sbin/cfgmtd -r -p /etc/ -f $CFG_RUNNING
if [ $? -ne 0 ]; then
    /sbin/cfgmtd -r -p /etc/ -t 2 -f $CFG_RUNNING
    if [ $? -ne 0 ]; then
        cp $CFG_DEFAULT $CFG_RUNNING
    fi
fi
sort $CFG_RUNNING | tr -d "\r" > $CFG_SYSTEM
cp $CFG_SYSTEM $CFG_RUNNING

# But for DFS testing, it's useful to be able to overide this
if [ -f /etc/persistent/enable_printk ]; then
    echo 9 > /proc/sys/kernel/printk
    dmesg -c
else
    # Do not clutter serial port, normally
    echo 1 > /proc/sys/kernel/printk
fi;

# Set device date to firmware build date
BDATE=201509091722
if [ ! -z $BDATE ]; then
    date -s $BDATE >/dev/null 2>&1
fi

# Run configuration parser
if [ -e /sbin/ubntconf ]; then
    /sbin/ubntconf
    echo "ubntconf returned $?" >> /tmp/ubntconf.log
fi

echo "...running /sbin/init"
exec /sbin/init

echo "INTERNAL ERROR!!! Cannot run /sbin/init."