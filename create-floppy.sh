FLOPPY=./bin/floppy.img

dd if=/dev/zero of=$FLOPPY bs=1024 count=2880

losetup /dev/loop0 $FLOPPY
mkdosfs -F12 /dev/loop0

mkdir -p /mnt/myfloppy/boot
mkdir -p /mnt/myfloppy/boot/EFI
mount /dev/loop0 /mnt/myfloppy -t msdos -o "fat=12"
grub-install --force --target i386-pc --compress=xz --core-compress=xz \
 --locales="" \
 --themes="" \
 --install-modules="multiboot2 all_video" \
 --boot-directory=/mnt/myfloppy/boot /dev/loop0

mcopy -i $FLOPPY target/x86_64/iso/* ::/
mcopy -i $FLOPPY target/x86_64/iso/boot/grub/grub.cfg ::/boot/grub/

umount /mnt/myfloppy
rm -rf /mnt/myfloppy
losetup -d /dev/loop0
