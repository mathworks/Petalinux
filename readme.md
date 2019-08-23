# MathWorks&reg; Zynq RFSoC Petalinux

MathWorks Petalinux build system

Current Supported Boards
- ZCU111 Zynq RFSoC

General build instructions
- Clone this repository and switch tags to the matching board and Vivado release, for instance:

`$ git checkout zcu111_2018.3`

- Setup the petalinux 2018.3 environment in your terminal (ie

`$ source /local/petalinux_2018_3/settings.sh`).

   Ensure you are using /bin/bash
- In the project directory, run the build command

`$ petalinux-build`

- After the build has succeeded, to create the SD image, run the helper script below

`$ mw_package_binaries.sh`

- A directory with the following files should appear

`$ ls MW_Petalinux_2018-12-01_18-38-58/`

    BOOT.BIN
    system.bit
	devicetree.dtb
    Image
    uramdisk.image.gz
	
- Copy these files over to your SD card and boot the evaluation board
