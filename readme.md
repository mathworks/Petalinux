# MathWorks&reg; Zynq Petalinux Repository

MathWorks Petalinux build repository for generating Zynq SoC SD card images.

#### Current Supported Boards
- ZCU111 Evaluation Kit
- ZCU216 Evaluation Kit

#### General build instructions
- Clone this repository and switch tags to the corresponding version. The release tags are labeled as **mathworks_zynqrfsoc_R2x.y.z**, where the "R2x" denotes the year of the MATLAB release and "y" denotes the 'a' or 'b' release. The "z" is used for updates to the image within a particular MATLAB release. For example, the tag "R22.1.0" is interperted as MATLAB R2022a where "R22" represents R2022 and "1.0" is for release 'a'. If there were updates to the image within the R2022a release, the versions would be "R22.1.1", "R22.1.2", etc. The MATLAB R2022b release would then start with the tag "R22.2.0".

`$ git checkout mathworks_zynqrfsoc_R22.1.0`

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
