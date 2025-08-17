# Petalinux project creation and Linux image build for AMD Kria KV260 Vision AI Starter Kit

This Petalinux repo is the starting point to build the linux image for AMD Kria KV260 Vision AI Starter Kit compatible to MathWorks applications. It uses the Kria KV260 Petalinux v2023.1 BSP. The linux image build process also includes the use of meta-mathworks layer for the linux image to work for MathWorks applications. The meta-mathworks layer is compatible to Petalinux version 2023.1

# Dependencies

1. A Linux based host computer with minimum of 8GB RAM and 100GB of free workspace 
2. An Ubuntu 20.04 LTS or Ubuntu 22.04 LTS Operating System
2. Petalinux Installer v2023.1

# Pre-requisites
Install all the required packages mentioned in [package_list](https://adaptivesupport.amd.com/s/article/000035006?language=en_US) that are used during image build

Make sure you are working on the host machine in bash mode other than the dash mode. If you are in dash mode you can change your host system to bash mode by running the below command. For this, you need to have administrative privileges

```
sudo dpkg-reconfigure dash
```

# Petalinux tool installation

Follow the below steps to install the Petalinux tool

1. Download the [Petalinux installer v2023.1](https://www.xilinx.com/member/forms/download/xef.html?filename=petalinux-v2023.1-05012318-installer.run)
2. Install the Petalinux tool by running the below commands

	```
	chmod 755 ./petalinux-v2023.1-05012318-installer.run
	./petalinux-v2023.1-05012318-installer.run
	```

3. Source the installed Petalinux tool

	```
	source <petalinux_installed_directory>/settings.sh
	```

# Follow the below steps to build the image using MathWorks repository
This section explains how to build the MathWorks compatible linux image using the MathWorks provided repository

### Complete the Petalinux project setup through MathWorks repository

1. Clone the Petalinux repo:

    ```
	git clone https://insidelabs-git.mathworks.com/EmbeddedLinux/Petalinux.git
	```

2. Checkout the latest branch after navigating to Petalinux directory:    

	```
	git checkout mathworks_R2026a
	```

3. Add the meta-mathworks submodule:  			

	```
	git submodule add https://github.com/mathworks/meta-mathworks mw_kria_kv260/project-spec/meta-mathworks
	```

4. Copy the mw_kv260.patch to the path mentioned below:		

	```
	Petalinux/mw_kria_kv260/project-spec/meta-mathworks

5. Apply the patch by navigating to the above path:	

	```
	git apply mw_kv260.patch
	```


With this, petalinux project setup is complete. Now navigate to the path Petalinux/mw_kria_kv260

Note: This repo is already setup with necessary configurations settings to build the bootable linux image for KV260. You can directly go to Linux Image build section and continue the process to get the linux image zip file. If you want to build the Linux image from the beginning or based on your application which requires further configuration of uboot and kernel, you can follow the below mentioned steps and build the image

# Follow the below steps to build the Linux image through BSP
This section explains how to build the MathWorks compatible Linux image for KV260 starting from the BSP download to fetching the linux image zip file

### Complete the Petalinux project setup with BSP

1. Download the [AMD Kria KV260 Vision AI Starter kit Petalinux BSP](https://www.xilinx.com/member/forms/download/xef.html?filename=xilinx-kv260-starterkit-v2023.1-05080224.bsp)

2. Create the Petalinux project using the KV260 BSP by running the below command. 

	```
	petalinux-create -t project -s <path_to_downloaded_bsp>
	```

	After this a new project with default name *xilinx-kv260-starterkit-v2023.1* is created. You can also create the project with specific name in specific directory by providing extra arguments to the above command. For more information, see [AMD Technical documentation](https://docs.amd.com/r/2023.1-English/ug1144-petalinux-tools-reference-guide/Creating-a-Project-from-a-BSP)

3. Clone the meta-mathworks layer using the below command and place it under *xilinx-kv260-starterkit-2023.1/project-spec* folder

	```
	git clone https://github.com/mathworks/meta-mathworks.git
	```

4. Now copy the above patch `mw_kv260.patch`available in this repo to the same path where meta-mathworks layer is located. Now apply the patch using below command after navigating to the *xilinx-kv260-starterkit-2023.1/project-spec* 

	```
	git apply mw_kv260.patch
	```

5. Get the Xilinx Source Archive (xsa) file generated after exporting your hardware design(bitstream included) using Export Hardware option in Xilinx Vivado to the petalinux project directory.

### Add the meta-mathworks layer to the petalinux configuration settings

1. cd to the Petalinux project(*xilinx-kv260-starterkit-2023.1*)

2. Configure the petalinux project by pointing to your xsa file using below command
	
	```
	petalinux-config --get-hw-description <path_to_xsa> --silentconfig
	```

After running the above command, menuconfig opens up. Navigate to *Yocto Settings--> User Layers*. Select `User layer 0` and enter the below string and click `OK`. You can make use of navigation keys to move in the menuconfig GUI
 
 	`${PROOT}/project-spec/meta-mathworks`
 
With this you are pointing the meta-mathworks layer path in the Petalinux project.

### Petalinux System Configuration

1. Navigate to the *System AUTO Hardware Settings* where you can change memory, ethernet and serial settings. These settings are set default dependning on the processing system IP configuration in your Xilinx Vivado design.
2. **Change IP address**: Now change the IP Address by navigating to *System AUTO Hardware Settings-->Ethernet Settings*. Uncheck/Deselect the *Obtain IP address automatically* option using space-bar key. Once it is done, you can get the other option to set your *Static IP address*. At this stage, you can give the *Static IP address* as `192.168.1.101`
3. **Image Packaging Configuration**: Navigate to *Image Packaging Configuration* menu option and change the *Root file system* to `INITRD`
4. **Firmware Version Configuration**: Navigate to *Firmware Version Configuration* and change the *Host name* and *Product name* to `mw_kv260` or any other desirable name as it is displayed in the Linux console when you boot up the board

You can also adjust other configuration settings as per the requirement. For more information, see https://docs.amd.com/r/2023.1-English/ug1144-petalinux-tools-reference-guide/Importing-a-Hardware-Configuration on adjusting Hardware configuration settings

### Configure the ROOTFS settings

1. Open the `user-rootfsconfig` file present in the path *xilinx-kv260-starterkit-2023.1/project-spec/meta-user/conf/*
2. Copy the below packages to be included in the linux image and save the file

	```
	CONFIG_libserialport

	CONFIG_libiio

	CONFIG_boost

	CONFIG_lttng-ust

	CONFIG_mw-fs-overlay
	```
	
3. Navigate to Petalinux project path (*xilinx-kv260-starterkit-2023.1*)
4. Open the rootfs menuconfig by running the below command

	```
	petalinux-config -c rootfs
	```

After runnig the above commands, a menuconfig opens up. Now, navigate to the *User packages* folder and here you can see the above packages. Select all the above package using spacebar button and then save and exit the menuconfig.

To turn off the password security to your linux image, you can do this by navigating to the *Image Features* menu option and select the `empty-root-password` and `serial-autologin-root` options

This is the place where you can also include other packages in your rootfs by choosing the available packages under *Filesystem Packages* and *Petalinux Package Groups* menu options

### Configure the u-boot and kernel settings 

The modifications in the u-boot settings are already applied through the patch `mw_kv260.patch` and default kernel settings are used. But, if you wish to change or modify any of the uboot and kernel settings, use the below commands to open the respective menuconfig windows

1. Open the u-boot menuconfig for more configuration settings using below command
 	
	```
	petalinux-config -c u-boot
	```

2. Open the kernel menuconfig for more configuration settings using below command				

	```
	petalinux-config -c kernel
	```

### Device Tree Settings	

This repo consists of basic.dtsi device tree source file to include the MathWorks devicetree nodes in the linux image. This dtsi file is present under *project-spec/meta-user/recipes-bsp/device-tree/files*. You can also refer to project-spec/meta-mathworks/recipe-apps/mw-refdesign-dtb/files for more device tree files. You can include these device-tree files in your petalinux project based on your application and continue to build the linux image. Make sure to append all the device tree source file paths in the in `device-tree.bbappend` file as well. This file is present in the path *project-spec/meta-user/recipes-bsp/device-tree*

# Linux Image build 

Once all the above settings are configured, you can run the below command to build the linux image
 

1. Petalinux build command for complete build			

	```
	petalinux-build
	```

2. To clean the cache before evey build, use below command

	```
	petalinux-build -x distclean
	```

3. To clean the entire build directory, use below command
	
	```
	petalinux-build -x mrproper (Run this command before you run the actual build command)
	``` 

This completes the Linux image build for KV260.
Once the build is successful, you can see the linux binaries present under the path *images/linux*

# Image Packaging

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
	system-zynqpmp-sck-kv-g-revB.dtb
	
The name of the dtb overlay file is dependent on the bitstream file name. Both the file names should be in sync as these are loaded on to the target kit. The name of the bitstream file generated for KV260 using HDL Coder Default system refernece design is `system_wrapper.bit`. Hence, the name of the dtbo file is `system_wrapper.dtbo`
	
### Boot up the board with the generated BOOT.BIN file

The Kria Kits have factory settings applied by default and it uses Xilinx provided bootgen file for initial booting on every POR.
We can load our BOOT.BIN file generated by us using the Petalinux workflow with the help of the below sudo commands and it is a one time process. 
Ths SD card is mounted on /mnt folder and our BOOT.BIN file is also located in the same folder 

Load the `BOOT.BIN` file present in the */mnt* folder using below command

```
sudo xmutil bootfw_update -i /mnt/BOOT.BIN
```

Hard reset the board for the changes to apply.

You can see the board booting up as expected and try pinging on `192.168.1.101`

Note: Also, everytime you power cycle the board, MathWorks provided `BOOT.BIN` file only will be used for initial boot. However, you can change the .BIN file or point to Xilinx provided .BIN files by using the above command

















