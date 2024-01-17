### meta-mathworks
This is the MathWorks Yocto layer to make it easier to interface MATLAB(R)/Simulink(TM) SoC Blockset software feature set with AMD-Xilinx FPGA SoC hardware platforms (like AMD-Xilinx Zynq-7000, Xilinx ZynqMPSoC, Xilinx ZynqRFSoC, Xilinx Versal).

### Dependencies
This layer is added on top of the [PetaLinux BSPs provided by AMD-Xilinx](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/2347204609/2022.1+Release).


### Using the meta-mathworks Layer
PetaLinux ecosystem of Linux build system can be used to generate a new Embedded Linux image for use with the AMD-Xilinx FPGA SoC platforms. PetaLinux is a wrapper around the Yocto tools to make the configuration of the Embedded Linux image easier. For more information about the PetaLinux SDK refer to [PetaLinux User Guide](https://www.xilinx.com/support/documentation/sw_manuals/xilinx2022_1/ug1144-petalinux-tools-reference-guide.pdf) or [PetaLinux Wiki] (https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18842250/PetaLinux).

This layer supports PetaLinux 2022.1 version of PetaLinux tools.

To build a PetaLinux project using the MathWorks yocto layer, you need a Linux computer or a Virtual Machine running Ubuntu where you have downloaded and already installed [PetaLinux software](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18842250/PetaLinux).

Open a Linux terminal and run following commands:

```bash
source <path-to-petalinux-installation-root>/settings.sh
git clone https://insidelabs-git.mathworks.com/EmbeddedLinux/meta-mathworks -b petalinux-2022.1
petalinux-create -t project --template <PLATFORM> --name <PROJECT_NAME>
  Here <PLATFORM> can be any one of:
    * zynqMP (for Xilinx Zynq UltraScale+ MPSoC platform)
    * zynq (for Xilinx Zynq-7000 platform)
    * microblaze (for Xilinx soft-core MicroBlaze platform)
    * versal (for Xilinx Versal platform)
petalinux-config --get-hw-description=<path to XSA file>
  Here XSA, Xilinx Software Architecture, file is obtained by exporting the Vivado project for embedded software development.
```
Alternatively, you can use a [pre-built PetaLinux BSP file](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/2347204609/2022.1+Release) for your hardware board to create the project.

```bash
petalinux-create -t project -s <full-path-to-the-bsp-file>
cd <path-to-petalinux-project>
```
If you used the `petalinux-config --get-hw-description=<path to XSA file>` option, then a configuration menu will come up. In the menu, go to **Yocto settigns --> User layers** and add the full-file path to the meta-mathworks layer.

```bash
# @TOOD Add extra settings for device-tree, network configuration, uEnv.txt here using a configuration script.
# source <full-file-path-to-meta-mathworks-folder>/run-petalinux-config.sh <ip-address>
petalinux-build

petalinux-package --boot --fsbl --fpga --u-boot
```

