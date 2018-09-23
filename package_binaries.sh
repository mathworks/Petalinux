
#petalinux-package --force --boot --fsbl rdf0476-zcu111-rf-dc-eval-tool-2018-2/apu/rfsoc_petalinux_bsp/images/linux/zynqmp_fsbl.elf  --fpga rdf0476-zcu111-rf-dc-eval-tool-2018-2/apu/rfsoc_petalinux_bsp/images/linux/system.bit --pmufw rdf0476-zcu111-rf-dc-eval-tool-2018-2/apu/rfsoc_petalinux_bsp/images/linux/pmufw.elf --u-boot rdf0476-zcu111-rf-dc-eval-tool-2018-2/apu/rfsoc_petalinux_bsp/images/linux//u-boot.elf

#build_dir=/zcu111_beta
current_dir="$(pwd)"
#current_dir+=$build_dir
cd $current_dir/images/linux/
cmd="petalinux-package --force --boot --fsbl zynqmp_fsbl.elf  --fpga system.bit --pmufw pmufw.elf --u-boot u-boot.elf"
echo $cmd
$cmd
