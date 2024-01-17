/*
-- Company:     Trenz Electronic
-- Engineer:     Oleksandr Kiyenko / John Hartfiel / Mohsen Chamanbaz

-- Code REV01
-- REV00 to REV01 changes:
    - Changes by Mohsen Chamanbaz (MC) in Sep. 2022
    - Definition two i2c bus (I2C0 --> Connected to PLL chip,I2C1 --> CPLD) instead of one i2c bus
    - Reading and writing via i2c1 from/on CPLD registers
    - While booting displayed boot mode/CPLD revision/PUDC status/boot mode variants of programmed jed file
    - The following files was changed: te_xfsbl_hooks_te0820.c / te_iic_platform.c / te_iic_platform.h/ te_si5338.c/ te_si5338.h/ 
        te_iic_define_te0820.h/
*/
/******************************************************************************
*
* 
*

******************************************************************************/

/*****************************************************************************/
/**
*
* @file te_xfsbl_hooks.c

******************************************************************************/
/***************************** Include Files *********************************/
//rename to correct board name
#include "te_xfsbl_hooks_te0820.h"

#include "psu_init.h"
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/
u32 TE_XFsbl_HookBeforeBSDownload_Custom(void )
{
  u32 Status = XFSBL_SUCCESS;

    xil_printf("\r\n--------------------------------------------------------------------------------\r\n");
    xil_printf("TE0820 TE_XFsbl_HookBeforeBSDownload_Custom\r\n"); 
  /* Add the code here */
    xil_printf("\r\n--------------------------------------------------------------------------------\r\n");

  return Status;
}

u32 TE_XFsbl_HookAfterBSDownload_Custom(void )
{
  u32 Status = XFSBL_SUCCESS;
    xil_printf("\r\n--------------------------------------------------------------------------------\r\n");
    xil_printf("TE0820 TE_XFsbl_HookAfterBSDownload_Custom\r\n");
  /* Add the code here */
    
   
  // for use with XIICPS_DEVICE_ID_2 (second I2C)
  // // ------------------------------------------------------
  // unsigned char chip_addr;
  // unsigned char reg_addr;
  // unsigned char  data;

  // u8 cpld_rev;
  // u8 cpld_bm;
  // u8 boot;
  // u8 pudc;
  // u8 boot_gen;

  // char* pudc_mode;
  // char* boot_mode;
  // char* bootmode_gen;
  // char* cpld_bootmode;
  // Status = XST_SUCCESS;
  // //---------------- Boot mode display via I2C interface -----------------
  // xil_printf("Initializing I2C Bus ... \r\n");

  // Status = iic_init(i2c1);
  // if (Status != XST_SUCCESS) {
    // xil_printf("Error:I2C Init\r\n");
    // goto END;
  // }

  // chip_addr = 0x20;
  // reg_addr  = 0x00;
    
    // Status = iic_write8(chip_addr,reg_addr,0x00,i2c1);   
    // if (Status != XST_SUCCESS)
    // {
        // xil_printf("\r\n***************************************************************************************************************\r\n");
        // xil_printf("\r\n***************************************************************************************************************\r\n");
        // xil_printf("\r\nPlease check the firmware version, CPLD firmware version is older than REV05, please update the CPLD firmware!\r\n");
        // xil_printf("\r\n***************************************************************************************************************\r\n");
        // xil_printf("\r\n***************************************************************************************************************\r\n");
        // sleep (3);
        // return XST_SUCCESS;
    // } else
    // {
        // xil_printf("Reading CPLD register \r\n");
        
        // reg_addr  = 0x00;
        // iic_read8(chip_addr,reg_addr,&data,i2c1);        
        // cpld_rev = data;
        
        // reg_addr = 0x01;
        // iic_read8(chip_addr,reg_addr,&data,i2c1);
        // boot_gen = (data >> 4) & 0x3;
        // if (boot_gen == 0b00)
        // {
          // bootmode_gen = "QSPI/SD";
        // }else if (boot_gen == 0b01)
        // {
          // bootmode_gen = "QSPI/JTAG";
        // }else if (boot_gen == 0b10)
        // {
          // bootmode_gen = "JTAG/SD";
        // }else if (boot_gen == 0b11)
        // {
          // bootmode_gen = "default QSPI/JTAG/SD/eMMC";
        // }else
        // {
          // bootmode_gen = "undefined";
        // }

        // pudc = (data >> 3) & 0x1;
        // if (pudc==1)
        // {
          // pudc_mode = "Pull-up deactivated";
        // } else
        // {
          // pudc_mode = "Pull-up activated";
        // }

        // cpld_bm = (data >> 2) & 0x1;
        // if (cpld_bm == 0)
        // {
          // cpld_bootmode = "Deactive";
        // }else
        // {
          // cpld_bootmode = "Active";
        // }

        // boot = (data >> 0) & 0x3;
        // if (boot == 0b01)
        // {
          // boot_mode = "JTAG";
        // }
        // else if (boot == 0b10)
        // {
          // boot_mode = "QSPI";
        // }
        // else if (boot == 0b11)
        // {
          // boot_mode = "SD Card";
        // }
        // else if (boot == 0b00)
        // {
          // boot_mode = "eMMC";
        // }
        // xil_printf("\n\rCPLD_REV=%02x\n\r",cpld_rev);
        // xil_printf("\n\rCPLD_BM=%s(%x)\n\r",cpld_bootmode, cpld_bm);
        // xil_printf("\n\rBOOTMOD_GEN=%x(%s)\n\r", boot_gen, bootmode_gen);
        // xil_printf("\n\rPUDC_MODE=%s(%d)\n\r", pudc_mode, pudc);
        // xil_printf("\n\rBOOT_MODE=%s(%x)\n\r", boot_mode, boot);
        // xil_printf("\n\r----------------------------\n\r");

        // return Status;
    // }
    // END:
    xil_printf("\r\n--------------------------------------------------------------------------------\r\n");

}

u32 TE_XFsbl_HookBeforeHandoff_Custom(u32 EarlyHandoff)
{
  u32 Status = XFSBL_SUCCESS;

    xil_printf("\r\n--------------------------------------------------------------------------------\r\n");
    xil_printf("TE0820 TE_XFsbl_HookBeforeHandoff_Custom\r\n"); 
  /* Add the code here */
    xil_printf("\r\n--------------------------------------------------------------------------------\r\n");

    return Status;
}

/*****************************************************************************/
/**
 * This is a hook function where user can include the functionality to be run
 * before FSBL fallback happens
 *
 * @param none
 *
 * @return error status based on implemented functionality (SUCCESS by default)
 *
  *****************************************************************************/

u32 TE_XFsbl_HookBeforeFallback_Custom(void)
{
  u32 Status = XFSBL_SUCCESS;


  xil_printf("\r\n--------------------------------------------------------------------------------\r\n");
  xil_printf("TE0820 TE_XFsbl_HookBeforeFallback_Custom\r\n"); 
  /* Add the code here */
  xil_printf("\r\n--------------------------------------------------------------------------------\r\n");

  return Status;
}

/*****************************************************************************/
/**
 * This function facilitates users to define different variants of psu_init()
 * functions based on different configurations in Vivado. The default call to
 * psu_init() can then be swapped with the alternate variant based on the
 * requirement.
 *
 * @param none
 *
 * @return error status based on implemented functionality (SUCCESS by default)
 *
  *****************************************************************************/
#ifdef USE_TE_PSU_FOR_SI_INIT
 u32 TE_XFsbl_TPSU_MODIFIED(void)
{
  // note Xilinx PSU must be disabled in tx_xfsbl_hooks.h
  // used to configure SI on the module. Only needed on this stage if SI CLKs are used for PS
  u32 Status;

  u32 DDRRegVal = 0;
  #ifdef XFSBL_ENABLE_DDR_SR
    /* Check if DDR is in self refresh mode */
    DDRRegVal = Xil_In32(XFSBL_DDR_STATUS_REGISTER_OFFSET) &
      DDR_STATUS_FLAG_MASK;
  #endif
  //Status = (u32)psu_init(); --> replace with inner code from psu_init
  // code from psu_init():  
  int psu_status = 1;

  psu_status &= psu_mio_init_data();
  psu_status &=  psu_peripherals_pre_init_data();
  psu_status &=   psu_pll_init_data();
  psu_status &=   psu_clock_init_data();
  psu_status &=  psu_ddr_init_data();
  

  // modified to use same code for DDR with and without self refresh mode, diff between  psu_init_ddr_self_refresh and psu_init
  if (DDRRegVal) {
    //do nothing
  } else {
    psu_status &=  psu_ddr_phybringup_data();  // not used on XFSBL_ENABLE_DDR_SR
  }
  
  psu_status &=  psu_peripherals_init_data();
  
  if (psu_status == 0) {
    Status= psu_status;
    // exit on error
    goto END;
  }

  
  //add code to initialize SI on module for GTR periphery
  xil_printf("\r\n--------------------------------------------------------------------------------\r\n");
  xil_printf("TE0820 TE_XFsbl_HookPsuInit_Custom\r\n"); 
  /* Add the code here */
  // ------------------------------------------------------
  // reset PCI and USB
    u32 dataVal = 0;

    // USB reset
    /* Set MIO25 direction as output */
    XFsbl_Out32(GPIO_DIRM_0, XFsbl_In32(GPIO_DIRM_0) | GPIO_MIO25_MASK);

    /* Set MIO30 output enable */
    XFsbl_Out32(GPIO_OEN_0, XFsbl_In32(GPIO_OEN_0) | GPIO_MIO25_MASK);  
    
    /* Set MIO30 to LOW */
    dataVal = XFsbl_In32(GPIO_DATA_0) & ~(GPIO_MIO25_MASK);
    XFsbl_Out32(GPIO_DATA_0, dataVal);
    
    // eth reset
    /* Set MIO24 direction as output */
    XFsbl_Out32(GPIO_DIRM_0, XFsbl_In32(GPIO_DIRM_0) | GPIO_MIO24_MASK);

    /* Set MIO24 output enable */
    XFsbl_Out32(GPIO_OEN_0, XFsbl_In32(GPIO_OEN_0) | GPIO_MIO24_MASK);


    /* Set MIO24 to LOW */
    dataVal = XFsbl_In32(GPIO_DATA_0) & ~(GPIO_MIO24_MASK);
    XFsbl_Out32(GPIO_DATA_0, dataVal);

  // ------------------------------------------------------
  Status = iic_init(i2c0);                      // Configure I2C Bus 0 driver instance
  if (Status != XFSBL_SUCCESS) {
     xil_printf("Error:I2C Init\r\n");
     goto END;
  }
  xil_printf("Configure PLL: ");

  Status= si5338_version(0x70,i2c0);
  if (Status != XFSBL_SUCCESS) {
     xil_printf("Error: Read Version of PLL\r\n");
     goto END;
  }
  
  Status = si5338_init(0x70,i2c0);                      // Configure clocks
  if (Status != XFSBL_SUCCESS) {
     xil_printf("Error: Configure CLK\r\n");
     goto END;
  }
  Status= si5338_status_wait(0x70,i2c0);
  if (Status != XFSBL_SUCCESS) {
     xil_printf("Error: Read Status of PLL and wait for calibration\r\n");
     goto END;
  }

  // ------------------------------------------------------
  // release ETH and USB reset
  /* Set MIO25 to HIGH */
  dataVal = XFsbl_In32(GPIO_DATA_0) | GPIO_MIO25_MASK;
  XFsbl_Out32(GPIO_DATA_0, dataVal);
  xil_printf("USB Reset Complete \r\n");
  

  /* Set MIO24 to HIGH */
  dataVal = XFsbl_In32(GPIO_DATA_0) | GPIO_MIO24_MASK;
  XFsbl_Out32(GPIO_DATA_0, dataVal);
  xil_printf("ETH Reset Complete\r\n");

  // ------------------------------------------------------
  xil_printf("\r\n--------------------------------------------------------------------------------\r\n");
  /* wait after initialization*/
  (void)usleep(DELAY_AFTER_US);
  
  
  //note: SDK with default FSBL will not generate init_serdes(), when no serdes are available
#if defined(SERDES_PLL_REF_SEL0_PLLREFSEL0_DEFVAL) || defined(SERDES_PLL_REF_SEL1_PLLREFSEL1_DEFVAL) || defined(SERDES_PLL_REF_SEL2_PLLREFSEL2_DEFVAL)  ||  defined(SERDES_PLL_REF_SEL3_PLLREFSEL3_DEFVAL)
  //psu_status &=  init_serdes();  --> replace with inner code
    // code from init_serdes():
    psu_status &= psu_resetin_init_data();
    psu_status &= serdes_fixcal_code();
    //psu_status &= serdes_enb_coarse_saturation();  --> replace with inner code
    // code from serdes_enb_coarse_saturation():  
      /*Enable PLL Coarse Code saturation Logic*/
      Xil_Out32(0xFD402094, 0x00000010);
      Xil_Out32(0xFD406094, 0x00000010);
      Xil_Out32(0xFD40A094, 0x00000010);
      Xil_Out32(0xFD40E094, 0x00000010);
    // finished code from serdes_enb_coarse_saturation()
    psu_status &=  psu_serdes_init_data();
    psu_status &=  psu_resetout_init_data();
  // finished code from init_serdes()
#endif
  //init_peripheral();  --> replace with inner code
    // code from init_peripheral():
    /*SMMU_REG Interrupt Enable: Following register need to be written all the time to properly catch SMMU messages.*/
    //PSU_Mask_Write(0xFD5F0018, 0x8000001FU, 0x8000001FU);  --> replace with inner code
      // code from serdes_enb_coarse_saturation():  
      unsigned long RegVal = 0x0;

      RegVal = Xil_In32(0xFD5F0018);
      RegVal &= ~(0x8000001FU);
      RegVal |= (0x8000001FU & 0x8000001FU);
      Xil_Out32(0xFD5F0018, RegVal);
      // finished code from serdes_enb_coarse_saturation()
  // finished code from init_peripheral():

  psu_status &=  psu_peripherals_powerdwn_data();
  psu_status &=  psu_afi_config();
  psu_ddr_qos_init_data();

// code from psu_init():
  if (psu_status == 0) {
    Status= psu_status;
    // exit on error
    goto END;
  }
  
  END:
  
  return Status;
 
}  
#else
u32 TE_XFsbl_XPSU_Default(void)
{
  //copy from Xilinx default PSU, xilinx default PSU from Xilinx xfsbl_hooks must be deactivated when TE modified variant is used
    u32 Status;
  #ifdef XFSBL_ENABLE_DDR_SR
    u32 RegVal;
  #endif
    
  #ifdef XFSBL_ENABLE_DDR_SR
    /* Check if DDR is in self refresh mode */
    RegVal = Xil_In32(XFSBL_DDR_STATUS_REGISTER_OFFSET) &
      DDR_STATUS_FLAG_MASK;
    if (RegVal) {
      Status = (u32)psu_init_ddr_self_refresh();
    } else {
      Status = (u32)psu_init();
    }
  #else
    Status = (u32)psu_init();
  #endif
  
    return Status;
}
#endif
  
u32 TE_XFsbl_HookPsuInit_Custom(void)
{
  u32 Status = XFSBL_SUCCESS;
  
  #ifdef USE_TE_PSU_FOR_SI_INIT
    Status = TE_XFsbl_TPSU_MODIFIED();
  #else
    Status = TE_XFsbl_XPSU_Default();
  #endif

  return Status;
}


/*****************************************************************************/
/**
  * for xsfbl_board.h
  *****************************************************************************/

/*****************************************************************************/
/**
 * This function does board specific initialization.
 * If there isn't any board specific initialization required, it just returns.
 *
 * @param none
 *
 * @return
 *     - XFSBL_SUCCESS for successful configuration
 *     - errors as mentioned in xfsbl_error.h
 *
 *****************************************************************************/
 
 
u32 TE_XFsbl_BoardInit_Custom(void)
{
  u32 Status = XFSBL_SUCCESS;
  u32 RegVal = 0;
  u32 temp = 0;

    xil_printf("\r\n--------------------------------------------------------------------------------\r\n");
    xil_printf("TE0820 TE_XFsbl_BoardInit_Custom\r\n");

    #ifdef USE_TE_PSU_FOR_SI_INIT

    #else

    // USB reset  
    /* Set MIO25 direction as output */
    XFsbl_Out32(GPIO_DIRM_0, XFsbl_In32(GPIO_DIRM_0) | GPIO_MIO25_MASK);

    /* Set MIO30 output enable */
    XFsbl_Out32(GPIO_OEN_0, XFsbl_In32(GPIO_OEN_0) | GPIO_MIO25_MASK);  
    
    /* Set MIO30 to LOW */
    RegVal = XFsbl_In32(GPIO_DATA_0) & ~(GPIO_MIO25_MASK);
    XFsbl_Out32(GPIO_DATA_0, RegVal);
    
    // eth reset
    /* Set MIO24 direction as output */
    XFsbl_Out32(GPIO_DIRM_0, XFsbl_In32(GPIO_DIRM_0) | GPIO_MIO24_MASK);

    /* Set MIO24 output enable */
    XFsbl_Out32(GPIO_OEN_0, XFsbl_In32(GPIO_OEN_0) | GPIO_MIO24_MASK);


    /* Set MIO24 to LOW */
    RegVal = XFsbl_In32(GPIO_DATA_0) & ~(GPIO_MIO24_MASK);
    XFsbl_Out32(GPIO_DATA_0, RegVal);
    //hold low for some time
    (void)usleep(DELAY_32_US);
    // release ETH and USB reset
    /* Set MIO25 to HIGH */
    RegVal = XFsbl_In32(GPIO_DATA_0) | GPIO_MIO25_MASK;
    XFsbl_Out32(GPIO_DATA_0, RegVal);
    xil_printf("USB Reset Complete \r\n");
    

    /* Set MIO24 to HIGH */
    RegVal = XFsbl_In32(GPIO_DATA_0) | GPIO_MIO24_MASK;
    XFsbl_Out32(GPIO_DATA_0, RegVal);
    xil_printf("ETH Reset Complete\r\n");
    
  #endif
  
  
  /* Add the code here */
  //check USB,PCIe Reset
  RegVal = XFsbl_In32(GPIO_DATA_0) ;
  temp = ((RegVal) & (GPIO_MIO25_MASK))>>25;
  if (temp!=0x1) {
    xil_printf("USB is hold into reset. (GPIO_DATA_0, Val:%x)\r\n", RegVal);
  }
  temp = ((RegVal) & (GPIO_MIO24_MASK))>>24;
  if (temp!=0x1) {
    xil_printf("ETH is hold into reset. (GPIO_DATA_0, Val:%x)\r\n", RegVal);
  }  
  
  
  //check serdes(gtr)
#if defined(SERDES_PLL_REF_SEL0_PLLREFSEL0_DEFVAL)
  RegVal = Xil_In32(0xFD4023E4);
  temp = ((RegVal) & (0x0030))>>4;
  if(temp!=0x3) {
    xil_printf("GTR Lane0 LOCK Status failed. (Reg:0xFD4023E4,Val:0x%x)\r\n", RegVal);
  }
#endif
#if defined(SERDES_PLL_REF_SEL1_PLLREFSEL1_DEFVAL)
  RegVal = Xil_In32(0xFD4063E4);
  temp = ((RegVal) & (0x0030))>>4;
  if(temp!=0x3) {
    xil_printf("GTR Lane1 LOCK Status failed. (Reg:0xFD4063E4,Val:0x%x)\r\n", RegVal);
  }
#endif
#if defined(SERDES_PLL_REF_SEL2_PLLREFSEL2_DEFVAL)
  RegVal = Xil_In32(0xFD40A3E4);
  temp = ((RegVal) & (0x0030))>>4;
  if(temp!=0x3) {
    xil_printf("GTR Lane2 LOCK Status failed. (Reg:0xFD40A3E4,Val:0x%x)\r\n", RegVal);
  }
#endif
#if defined(SERDES_PLL_REF_SEL3_PLLREFSEL3_DEFVAL)
  RegVal = Xil_In32(0xFD40E3E4);
  temp = ((RegVal) & (0x0030))>>4;
  if(temp!=0x3) {
    xil_printf("GTR Lane3 LOCK Status failed. (Reg:0xFD40E3E4,Val:0x%x)\r\n", RegVal);
  }
#endif
//  END:
  xil_printf("\r\n--------------------------------------------------------------------------------\r\n");

  
  return Status;
}
