
timestamp() {
      date +"%Y-%m-%d_%H-%M-%S"
}

build/tmp/sysroots-components/x86_64/u-boot-mkimage-native/usr/bin/mkimage -A arm64 -T ramdisk -C gzip -d images/linux/rootfs.cpio.gz images/uramdisk.image.gz
petalinux-package --boot --format BIN --bif boot.bif -o ./images/BOOT.BIN --force

src_dir="$(pwd)"
get_timestamp=$(timestamp)
dir_prefix=${PWD##*/}_
dst_dir=./MW_$dir_prefix$get_timestamp
echo $dst_dir
mkdir_cmd="mkdir -p $dst_dir"
cp_cmd1="cp $src_dir/images/BOOT.BIN $dst_dir"
cp_cmd2="cp $src_dir/images/uramdisk.image.gz $dst_dir"
cp_cmd3="cp $src_dir/images/linux/system.dtb $dst_dir/devicetree.dtb"
cp_cmd4="cp $src_dir/images/linux/Image $dst_dir"
cp_cmd5="cp $src_dir/images/linux/system.bit $dst_dir"
echo $mkdir_cmd
echo $cp_cmd1
echo $cp_cmd2
echo $cp_cmd3
echo $cp_cmd4
echo $cp_cmd5

$mkdir_cmd

$cp_cmd1
$cp_cmd2
$cp_cmd3
$cp_cmd4
$cp_cmd5

print_dst=$(echo $dst_dir | sed 's/\//\\/g')
echo "Copied location: \\$print_dst"
