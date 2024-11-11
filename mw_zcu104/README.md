
### PetaLinux project creation for ZCU104
The files in this repository serve as a starting point to create a PetaLinux project using PetaLinux 2023.1 for ZCU104.

### Dependencies
The project requires the addition of the meta-mathworks layer on top of the Petalinux files provided by Xilinx for ZCU104 UltraScale+ MPSoC Evaluation Kit

Download the [meta-mathworks Yocto layer](https://github.com/mathworks/meta-mathworks/tree/petalinux-v2022.1) and place it in the project-spec folder

You can then use the XSA file and the project template provided to create the PetaLinux project and follow the instructions in the [meta-mathworks repository](https://github.com/mathworks/meta-mathworks/tree/petalinux-v2022.1) to build the image. 