# MathWorks&reg; Zynq PetaLinux Projects

MathWorks PetaLinux projects and helper scripts for building SD-card images targeting AMD/ADI reference boards. Each board directory contains a ready-to-build PetaLinux project plus utilities (init scripts, env configs, device-tree overlays, packaging helpers) tailored for MathWorks workflows.

## Supported boards

| Board | Folder (README) | PetaLinux version | Helper script(s) |
| --- | --- | --- | --- |
| ADI RF SOM (ADRV9361-Z7035) | [mw_adirfsom/README.md](mw_adirfsom/README.md) | 2023.1 | [mw_adirfsom/mw_build_adirfsom.sh](mw_adirfsom/mw_build_adirfsom.sh) |
| AMD Kria KR260 Robotics Starter Kit | [mw_kria_kr260/README.md](mw_kria_kr260/README.md) | 2023.1 | [mw_kria_kr260/mw_build_kr260.sh](mw_kria_kr260/mw_build_kr260.sh) |
| AMD Kria KV260 Vision AI Starter Kit | [mw_kria_kv260/README.md](mw_kria_kv260/README.md) | 2023.1 | [mw_kria_kv260/mw_build_kv260.sh](mw_kria_kv260/mw_build_kv260.sh) |
| PicoZed with Imageon carrier | [mw_picozed/README.md](mw_picozed/README.md) | 2023.1 | [mw_picozed/mw_build_picozed.sh](mw_picozed/mw_build_picozed.sh) |
| ZedBoard Zynq-7000 | [mw_zedboard/README.md](mw_zedboard/README.md) | 2023.1 | [mw_zedboard/mw_build_zedboard.sh](mw_zedboard/mw_build_zedboard.sh) |
| ZC702 Zynq-7000 | [mw_zc702/README.md](mw_zc702/README.md) | 2023.1 | [mw_zc702/mw_build_zc702.sh](mw_zc702/mw_build_zc702.sh) |
| ZC706 Zynq-7000 | [mw_zc706/README.md](mw_zc706/README.md) | 2023.1 | [mw_zc706/mw_build_zc706.sh](mw_zc706/mw_build_zc706.sh) |
| ZCU102 MPSoC | [mw_zcu102/README.md](mw_zcu102/README.md) | 2023.1 | [mw_zcu102/mw_build_zcu102.sh](mw_zcu102/mw_build_zcu102.sh) |
| ZCU104 MPSoC | [mw_zcu104/README.md](mw_zcu104/README.md) | 2023.1 | (manual steps) |
| ZCU111 RFSoC | [mw_zcu111_v2021.2/README.md](mw_zcu111_v2021.2/README.md) | 2021.2 | [mw_zcu111_v2021.2/mw_build_zcu111.sh](mw_zcu111_v2021.2/mw_build_zcu111.sh) |
| ZCU208 RFSoC | [mw_zcu208/README.md](mw_zcu208/README.md) | 2023.1 | [mw_zcu208/mw_build_zcu208.sh](mw_zcu208/mw_build_zcu208.sh) |
| ZCU216 RFSoC | [mw_zcu216_v2023.1/README.md](mw_zcu216_v2023.1/README.md) | 2023.1 | [mw_zcu216_v2023.1/mw_build_zcu216.sh](mw_zcu216_v2023.1/mw_build_zcu216.sh) |
| ZCU670 RFSoC DFE | [mw_zcu670/README.md](mw_zcu670/README.md) | 2023.2 | [mw_zcu670/mw_build_zcu670.sh](mw_zcu670/mw_build_zcu670.sh) |
| Trenz TE0820 | [mw_te0820/README.md](mw_te0820/README.md) | 2022.2 | (manual steps) |
| Legacy ZCU216 BSP template | [mw_zcu216/README](mw_zcu216/README) | (Xilinx BSP reference) | (manual steps) |


## When building manually

Every board README documents how to add the meta-mathworks layer, apply board-specific patches, configure kernel and u-boot, and package the SD image. Follow those steps if you need to customize the project beyond the provided helper scripts.
