
### PetaLinux project creation for TE0820
The files in this repository serve as a starting point to create a PetaLinux project using PetaLinux 2022.2 for the TE0820 board.

### Dependencies
The project requires the addition of the meta-mathworks layer on top of the [Trenz TE0820 Test Board - Petalinux files provided by Trenz.](https://wiki.trenz-electronic.de/display/PD/TE0820+Test+Board#TE0820TestBoard-SoftwareDesign-PetaLinux)

Download the [meta-mathworks Yocto layer](https://github.com/mathworks/meta-mathworks/tree/petalinux-v2022.1) and place it in the project-spec folder. Use the 'trenzPatch.patch' file customize the layer for the Trenz board. 

You can apply the patch by placing it inside the meta-mathworks folder and using the following command: 

```bash
git apply trenzPatch.patch
```

You can then use the XSA file and the project template provided to create the PetaLinux project and follow the instructions in the [meta-mathworks repository](https://github.com/mathworks/meta-mathworks/tree/petalinux-v2022.1) to build the image. 