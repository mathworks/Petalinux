# MathWorks&reg; Zynq Petalinux Repository

MathWorks Petalinux build repository for generating Zynq SoC SD card images.

#### Current Supported Boards
- ZCU111 Evaluation Kit
- ZCU216 Evaluation Kit

#### General build instructions
- Clone this repository and switch tags to the corresponding version. The most up to date tag for RFSoC is **mathworks_zynqrfsoc_R21.1.1**. The last set numbers of the tag "R21.1.0" is interperted as MATLAB R2021a where "R21" represents R2021 and "1.0" is for release 'a'. Version "2.0" would represent 'b'.

`$ git checkout mathworks_zynqrfsoc_R21.1.0`

- Setup the petalinux environment in your terminal (ie: `$ source /local/petalinux/settings.sh`).

   Ensure you are using `/bin/bash` before calling this.
   
- In the project directory, change directories to the board of interest then run the build command. As an example:

`$ cd zcu111`

`$ petalinux-build`

- After the build has succeeded, to create the SD image, run the helper script below

`$ mw_package_binaries.sh`

- A directory with the following files should appear

`$ ls mw_zcu111_2020-12-29_01-27-32/`

    BOOT.BIN
    system.bit
    devicetree.dtb
    Image
    uramdisk.image.gz
	
- Copy these files over to your SD card and boot the evaluation board
