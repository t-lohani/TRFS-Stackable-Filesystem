#!/bin/sh
set -x
# WARNING: this script doesn't check for errors, so you have to enhance it in case any of the commands
# below fail.
make clean
make
cd ../../../../..
umount /mnt/trfs/
rmmod /usr/src/hw2-tlohani/fs/trfs/trfs.ko
insmod /usr/src/hw2-tlohani/fs/trfs/trfs.ko
mount -t trfs -o tfile=/tmp/tfile1.txt /usr/src/lower/ /mnt/trfs
