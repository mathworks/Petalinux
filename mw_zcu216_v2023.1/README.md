# Petalinux Project Creation and Linux Image Build for Zynq™ UltraScale+™ RFSoC ZCU216 Evaluation Kit
 

This Petalinux repo is the starting point to build the Linux image for Zynq™ UltraScale+™ RFSoC ZCU216 Evaluation Kit
 compatible to MathWorks applications. The Linux image build process also includes the use of meta-mathworks layer for the Linux image to work for MathWorks applications. The meta-mathworks layer is compatible to Petalinux version 2023.1.

# Dependencies

1. A Linux based host computer with minimum of 8GB RAM and 100GB of free workspace. 
2. An Ubuntu 20.04 LTS or Ubuntu 22.04 LTS Operating System.
2. Petalinux Installer v2023.1.

# Pre-requisites
Install all the required packages mentioned in [package_list](https://adaptivesupport.amd.com/s/article/000035006?language=en_US) that are used during image build.

Make sure you are working on the host machine in bash mode other than the dash mode. If you are in dash mode you can change your host system to bash mode by running the below command. For this, you need to have administrative privileges. For more information, see [Setting up your environment](https://docs.amd.com/r/2023.1-English/ug1144-petalinux-tools-reference-guide/Setting-Up-Your-Environment).

```
sudo dpkg-reconfigure dash
```

# Petalinux Tool Installation

Follow the below steps to install the Petalinux tool in your host machine using command line interface.

1. Download the [Petalinux installer v2023.1](https://www.xilinx.com/member/forms/download/xef.html?filename=petalinux-v2023.1-05012318-installer.run)
2. Run the below commands to install the Petalinux tool from the path where you downloaded the petalinux installer.

	```
	chmod 755 ./petalinux-v2023.1-05012318-installer.run
	./petalinux-v2023.1-05012318-installer.run
	```

3. Run the below command to source the installed Petalinux tool to use the petalinux tool commands for image build.

	```
	source <petalinux_installed_directory>/settings.sh
	```

# Build the Linux Image Using MathWorks Repository
This section explains how to build the MathWorks compatible Linux image using the MathWorks provided repository.

### Complete the Petalinux project setup through MathWorks repository
This section explains how you can setup the MathWorks Petalinux repository and add the meta-mathworks layer into your Petalinux configurations settings.

1. Clone the MathWorks Petalinux repository.

    ```
	git clone https://insidelabs-git.mathworks.com/EmbeddedLinux/Petalinux.git
	```

2. Checkout the latest branch after navigating to Petalinux directory.    

	```
	git checkout mathworks_R2026a
	```
Navigate to *mw_zcu216* directory. You can use `mw_build_zcu216.sh` script present in this folder to add the meta-mathworks layer to the Petalinux project and get the Linux image in a zip file directly in your working directory. Note that the script is run from its path only using below command.

	chmod +x mw_zcu216
	cd mw_zcu216
	chmod 777 *.sh
	./mw_build_zcu216.sh <petalinux_installed_directory> <mw_zcu216_directory>

 If you wish to build the Linux image manually, follow the steps mentioned in this section.

3. Add the `meta-mathworks` layer to the zcu216 Petalinux project using below command. Run this command from the *mw_zcu216* directory.			

	```
	git clone -b petalinux-v2023.1 https://github.com/mathworks/meta-mathworks.git project-spec/meta-mathworks
	```

4. Point the Petalinux project to the `mata-mathworks` layer. You can do this by using Petalinux config GUI. Run the below command to launch the Petalinux config GUI.

	```
	petalinux-config
	```

After running the above command, menuconfig opens up. Navigate to *Yocto Settings--> User Layers*. Select `User layer 0` and enter the below string and click `OK`. You can make use of navigation keys to move in the menuconfig GUI.

 ```
 ${PROOT}/project-spec/meta-mathworks
 ```
 
With this you are pointing the Petalinux project to use meta-mathworks layer in Linux image build process and this completes the Petalinux project setup. You can now navigate to the path *Petalinux/mw_zcu216*

Note: This repo is already setup with necessary configurations settings to build the bootable Linux image for zcu216. If you want to include any specific kernel drivers in your Linux image, you can refer to the **Configure the kernel** section in this file on different ways to configure the kernel.

Now, you can directly go to Linux Image Build section and continue the process to get the Linux image zip file.

If you want to build the Linux image starting from the BSP, you can follow the below mentioned steps and build the image.

# Build the Linux Image Using BSP
This section explains how to build the MathWorks compatible Linux image for zcu216 using from the bsp 

### Complete the Petalinux project setup with BSP

1. Download the [ AMD Zynq® UltraScale+™ MPSoC zcu216 Evaluation Kit](https://www.xilinx.com/member/forms/download/xef.html?filename=xilinx-zcu216-v2023.1-05080224.bsp).

2. Create the Petalinux project using the zcu216 BSP by running the below command.

	```
	petalinux-create -t project -s <path_to_downloaded_bsp>
	```
	After this a new project with default name *xilinx-zcu216-v2023.1* is created. You can also create the project with specific name in specific directory by providing extra arguments to the above command. For more information, see [Creating a Project from a BSP](https://docs.amd.com/r/2023.1-English/ug1144-petalinux-tools-reference-guide/Creating-a-Project-from-a-BSP).	

3. Add the `meta-mathworks` layer to the zcu216 Petalinux project using below command. Run this command from the *mw_zcu216* directory.	

	```
	git clone -b petalinux-v2023.1 https://github.com/mathworks/meta-mathworks.git project-spec/meta-mathworks
	```

4. Get the Xilinx Source Archive (xsa) to Petalinux project directory. You can generate xsa for your hardware design (bitstream included) using Export Hardware option in Xilinx Vivado.
### Add the meta-mathworks layer to the petalinux configuration settings

1.  Change directory to the Petalinux project(*xilinx-zcu216-v2023.1*)

2. 	Configure the petalinux project by pointing to your xsa file using below command.

	```
	petalinux-config --get-hw-description <path_to_xsa> --silentconfig
	```

After running the above command, menuconfig opens up. Navigate to *Yocto Settings--> User Layers*. Select `User layer 0` and enter the below string and click `OK`. You can make use of navigation keys to move in the menuconfig GUI.

 ```
 ${PROOT}/project-spec/meta-mathworks
 ```
 
With this you are pointing your Petalinux project to use meta-mathworks layer.

### Configure the Root File System (ROOTFS)

1. Open the `user-rootfsconfig` file present in the path *xilinx-zcu216-v2023.1/project-spec/meta-user/conf/*.
2. Copy the below packages to include in the Linux image and save the file.

	```
	CONFIG_libserialport
	CONFIG_libiio
	CONFIG_boost
	CONFIG_lttng-ust
	CONFIG_mw-fs-overlay
	CONFIG_boost-dev
	CONFIG_lttng-ust-bin
	CONFIG_mw-refdesign-dtb
	CONFIG_rfdc
	CONFIG_rfdc-read-write
	CONFIG_rfdc-selftest
	CONFIG_libsdfecusrintf
	CONFIG_rfdc-selftest
	CONFIG_mw-rf-init
	CONFIG_rftool-zcu216
	```
3. Open the `rootfs_config` file present in the path *xilinx-zcu216-v2023.1/project-spec/configs/rootfs_config*.
4. Copy the below packages to include in the Linux image and save the file.

	```
	CONFIG_libserialport=y
	CONFIG_libiio=y
	CONFIG_boost=y
	CONFIG_lttng-ust=y
	CONFIG_lttng-ust-bin=y
	CONFIG_mw-fs-overlay=y
	CONFIG_rfdc=y
	CONFIG_rfdc-read-write=y
	ONFIG_rfdc-selftest=y
	CONFIG_mw-rf-init=y
	CONFIG_rftool-zcu216=y
	```	
	
5. Similarly, you can include your custom packages into the root file system.
6. Navigate to Petalinux project path (*xilinx-zcu216-v2023.1*).
7. Open the rootfs menuconfig by running the below command.

	```
	petalinux-config -c rootfs
	```
After runnig the above commands, a menuconfig opens up. Now, navigate to the *User packages* folder and you can see the above packages. Select all the above package using spacebar button and then save and exit the menuconfig.


This is the place where you can also include other packages in your rootfs by choosing the available packages under *Filesystem Packages* and *Petalinux Package Groups* menu options.

### Configure the kernel

**Kernel Configuration**

Default kernel settings are used to build the Linux image for zcu216. If you wish to configure the kernel, you can follow below steps.

### Adding kernel drivers to the Linux image
This section explains how you can include the kernel drivers in your Linux image. There are different ways as mentioned below.

**Type-I**: Specify the drivers you want to include in `bsp.cfg` file present under *project-spec/meta-user/recipes-kernel/linux-xlnx* folder. 

**Type-II**: Create your own configuration (`<custom_name>.cfg`) file under */project-spec/meta-user/recipes-kernel/linux-xlnx* directory and specify the drivers in this `.cfg` file. Then, you can append this `custom_name.cfg` file to recipes-kernel using `linux-xlnx_%.bbapppend` file.

**Type-III**: Launch the Petalinux menuconfig GUI to add the kernel drivers into the image by running the below command.
```
petalinux-config -c kernel
```
The kernel drivers selected through GUI mode automatically creates a new .cfg file and adds this file to the `linux_xlnx_%.bbappend` file.


### Device tree settings	

This repo consists of `basic.dtsi` device tree source file to include the MathWorks devicetree nodes in the Linux image. This dtsi file is present under *project-spec/meta-user/recipes-bsp/device-tree/files*. You can also refer to *project-spec/meta-mathworks/recipe-apps/mw-refdesign-dtb/files* for more device tree files. You can include these device-tree files in your petalinux project based on your application and continue to build the Linux image. Make sure to append all the device tree source file paths in the in `device-tree.bbappend` file as well. This file is present in the path *project-spec/meta-user/recipes-bsp/device-tree*.

# Linux Image Build 

Once all the above settings are done, you can run the below command to build the Linux image.
 
1. Petalinux build command for complete build.

	```
	petalinux-build
	```

2. To clean the cache before evey build, use below command.

	```
	petalinux-build -x distclean
	```

3. To clean the entire build directory, use below command.
	
	```
	petalinux-build -x mrproper (Run this command before you run the actual build command)
	``` 

This completes the Linux image build for zcu216.Once the build is successful, you can see the Linux binaries present under the path *images/Linux*. 

You can package the boot image into a `BOOT.BIN` file by runnin the below command.

	
	petalinux-package --boot --u-boot --fpga --force
	

# Image Packaging

Copy files from /images/linux/ to the SD card 

	BOOT.BIN 
	boot.scr
	Image
	rootfs.cpio.gz.u-boot
	system.bit
	system.dtb  


Copy files from /mw_utils/ to the SD card

	dhcp.script
	init.sh
	rf-init
	stopsociod.service
	libboost-1-63-overlay.tar.gz
	interfaces
	

NOTE: If we use the script mw_build_zcu216.sh then Image Packaging step will be automatically handled and it will generate a xilinx-zcu216-2023.1_sdcard_zynq_mpsoc_ec_YYYY-MM-DD.zip in <proj-dir/images> directory
