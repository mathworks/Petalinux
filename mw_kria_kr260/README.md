### Petalinux project creation and Linux image build for AMD Kria KR260 Robotics Starter Kit

This Petalinux repo is the starting point to build the linux image for AMD Kria KR260 Robotics Starter Kit compatible to MathWorks applications. It uses the Kria KR260 Petalinux v2023.1 BSP. The linux image build process also includes the use of meta-mathworks layer for the linux image to work for MathWorks applications. The meta-mathworks layer is compatible to Petalinux version 2023.1

### Follow the below steps to setup the petalinux project

### Complete the Project setup by cloning the required repositories

1. Clone the Petalinux repo:                     	git clone https://insidelabs-git.mathworks.com/EmbeddedLinux/Petalinux.git
2. Checkout the latest branch after cd to Petalinux:    git checkout mathworks_R2025b
3. Add the meta-mathworks submodule:  			git submodule add https://insidelabs-git.mathworks.com/EmbeddedLinux/meta-mathworks mw_kria_kr260/project-spec/meta-mathworks
4. Copy the mw_kr260.patch to the path:			Petalinux/mw_kria_kr260/project-spec/meta-mathworks
5. Apply the patch by navigating to the above path:	git apply mw_kr260.patch 

With this, petalinux project setup is complete. Now navigate to the path Petalinux/mw_kria_kr260

Note: This repo is already setup with necessary configurations settings to build the bootable linux image for KR260. You can directly go to Linux Image build section and continue the process to get the linux image zip file. If you want to build the Linux image from the beginning or based on your application which requires further configuration of uboot and kernel, you can follow the below mentioned steps and build the image

### Follow the below steps to build the Linux image and boot the board

### Add the meta-mathworks layer to the configuration settings

1. source the Petalinux tool:				source <petalinux_installaed_directory>/settings.sh
2. Configure the project by pointing to the xsa file:	petalinux-config --get-hw-description <path_to_xsa> --silentconfig
 After running the above command, menuconfig opens up. Navigate to Yocto Settings--> User Layers. Select User layer 0 and enter the below string
 
 	${PROOT}/project-spec/meta-mathworks
 
With this you are pointing the meta-mathworks layer path.

You can also adjust other configuration settings as per the requirement. For more information, see https://docs.amd.com/r/2023.1-English/ug1144-petalinux-tools-reference-guide/Importing-a-Hardware-Configuration on adjusting Hardware configuration settings

### Configure the ROOTFS settings

1. Open the user-rootfsconfig file present in the path: project-spec/meta-user/conf/
2. Copy the below packages to be included in the linux image and save the file

	CONFIG_libserialport

	CONFIG_libiio

	CONFIG_boost

	CONFIG_lttng-ust

	CONFIG_mw-fs-overlay
	
3. Navigate to Petalinux project path:			Petalinux/mw_kria_kr260/
4. Open the rootfs menuconfig:				petalinux-config -c rootfs

After runnig the above commands, a menuconfig opens up. Now, navigate to the User packages folder and here you can see the above packages. Select all the above package using spacebar button and then save and exit the menuconfig

### Configure the u-boot and kernel settings 

This repo is already configured with required uboot and kernel settings. But, if you wish to change or modify any of the uboot and kernel settings, use the below commands to open the respective menuconfig windows

1. Configure the u-boot:				petalinux-config -c u-boot
2. Configure the kernel:				petalinux-config -c kernel

### Linux Image build 

Once all the settings are configured, you can run the below command to build the linux image

Note: This repo consists of basic.dtsi device tree source file to include the MathWorks devicetree nodes in the linux image. This dtsi file is present under project-spec/meta-user/recipes-bsp/device-tree/files. You can also refer to project-spec/meta-mathworks/recipe-apps/mw-refdesign-dtb/files for more device tree files. You can include these device-tree files in your petalinux project based on your application and continue to build the linux image

1. Petalinux build command:				petalinux-build
2. Clean the cache:					petalinux-build -x distclean
3. Clean the entire build directory:			petalinux-build -x mrproper (Run this command before you run the actual build command)

This completes the Linux image build for KR260.
Once the build is successful, you can see the linux binaries present under the path: images/linux

### Image Packaging

You can make use of the mw_package_binaries.sh script to pack the image into a zip file. The script is self explanatory and you can modify it based on your requirements.

Once you extract the ZIP file into the SD card, you can see the below contents

	BOOT.BIN
	boot.scr
	Image
	init.sh
	interfaces
	rootfs.cpio.gz.u-boot
	shell.json
	system.bit
	system.dtb
	system_wrapper.dtbo
	system-zynqpmp-sck-kr-g-revB.dtb
	
The name of the dtb overlay file is dependent on the bitstream file. Both the file names should be in sync as these should be loaded on to the target kit. The name of the bitstream file generated for KR260 using HDL Coder Default system refernece design is system_wrapper.bit
	
### Boot up the board with the generated BOOT.BIN file

The Kria Kits have factory settings applied by default and it uses Xilinx provided bootgen file for initial booting on every POR.
We can load our BOOT.BIN file generated by us using the Petalinux workflow with the help of the below sudo commands and it is a one time process. 
Ths SD card is mounted on /mnt folder and our BOOT.BIN file is also located in the same folder 

1. Load the BOOT.BIN file present in the /mnt folder:	sudo xmutil bootfw_update -i /mnt/BOOT.BIN

Hard reset the board for the changes to apply.

You can see the board booting up as expected. 

Note: Also, everytime you power cycle the board, our BOOT.BIN file only will be used for initial boot. However, you can change the .BIN file or point to Xilinx provided .BIN files by using the above command

















