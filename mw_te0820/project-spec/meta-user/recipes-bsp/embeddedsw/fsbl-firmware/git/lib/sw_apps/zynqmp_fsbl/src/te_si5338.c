/*
-- Company: 		Trenz Electronic
-- Engineer: 		Oleksandr Kiyenko / John Hartfiel / Mohsen Chamanbaz
 
-- Code REV01
-- REV00 to REV01 changes:
    -- Changes implemented by Mohsen Chamanbaz (MC) in Sep. 2022
    -- For all functions defined an additional input variable(unsigned char bus).
 */

#include "te_si5338.h"

#ifdef CLOCK_SI5338
#include "te_uart.h"
#include "te_Si5338-Registers.h"



int si5338_version(unsigned char chip_addr,unsigned char bus){
	u8 reg_val;
	int Status;
	unsigned char interface = bus;
    if (interface == i2c0)
    {
    	Status = iic_read8( chip_addr, 2, &reg_val,i2c0);
        xil_printf("SI53%i",reg_val);
        Status = iic_read8( chip_addr, 0, &reg_val, i2c0);
        if (reg_val==0) {
          xil_printf("-A\r\n");
        } else if (reg_val==1) {
          xil_printf("-B\r\n");
        } else {
          xil_printf("-%x\r\n",reg_val);
        }
    } else if (interface == i2c1)
    {
    	Status = iic_read8( chip_addr, 2, &reg_val,i2c1);
        xil_printf("SI53%i",reg_val);
        Status = iic_read8( chip_addr, 0, &reg_val,i2c1);
        if (reg_val==0) {
          xil_printf("-A\r\n");
        } else if (reg_val==1) {
          xil_printf("-B\r\n");
        } else {
          xil_printf("-%x\r\n",reg_val);
        }
	}

	  return Status;
}

int si5338_status_wait(unsigned char chip_addr, unsigned char bus){
	u8 reg_val;
	unsigned int  cnt=0, tmp;
	int Status;
	unsigned char interface = bus;
  (void)usleep(0x5U);
  if (interface == i2c0)
  {
	  Status = iic_read8( chip_addr, 218, &reg_val,i2c0);
  }else if (interface == i2c1)
  {
	  Status = iic_read8( chip_addr, 218, &reg_val,i2c1);
  }
  
  
  tmp = 1;
  // Wait until internal calibration is not busy
  while (tmp ==1) {
    cnt=cnt+0x100U ;
    (void)usleep(0x100U);
    if (interface == i2c0)
    {
    	Status = iic_read8( chip_addr, 218, &reg_val,i2c0);
    }else if (interface == i2c1)
    {
    	Status = iic_read8( chip_addr, 218, &reg_val,i2c1);
    }

    tmp =((reg_val) & (0x01));
    
    if ((cnt % 0x100U) == 0) {
      xil_printf("Status 218:0x%x (...waiting for calibration...%i us).\r",reg_val,cnt);
    }
    
    if (cnt >= TIME_CHECK_PLL_CONFIG_US) {
        xil_printf("Status 218:0x%x (...calibration not finished after %i us...exit...).\r\n",reg_val,cnt);
        xil_printf("Status 218:0x%x (cal bit0:%i) will be checked one time again after %i us\r\n",reg_val,tmp,DELAY_AFTER_PLL_CONFIG_US);
        tmp = 0;
    }
    
  }
   //sleep need for PCIe
  (void)usleep(DELAY_AFTER_PLL_CONFIG_US);
    
  if (interface == i2c0)
  {
	  Status = iic_read8( chip_addr, 218, &reg_val,i2c0);
  }else if (interface == i2c1)
  {
	  Status = iic_read8( chip_addr, 218, &reg_val,i2c1);
  }

  xil_printf("PLL Status Register 218:0x%x                                         \r\n",reg_val);

	  return Status;
}



int si5338_init(unsigned char chip_addr,unsigned char bus)
{
	int i;
	u8 reg_val;
	Reg_Data rd;
	int Status;
	unsigned char interface = bus;
#ifdef DEBUG_REG
  u8 readback_test;
#endif

    // p_printf(("Si5338 Init Start.\r\n"));
    // iic_init();
    p_printf(("Si5338 Init Registers Write.\r\n"));
    if (interface == i2c0)
    {
    	// I2C Programming Procedure
    		iic_write8( chip_addr, 246, 0x01, i2c0);					//Hard reset
    		// Disable Outputs
    		iic_write8_mask( chip_addr, 230, EOB_ALL, EOB_ALL, i2c0);	// EOB_ALL = 1
    		// Pause LOL
    		iic_write8_mask( chip_addr, 241, DIS_LOL, DIS_LOL, i2c0);	// DIS_LOL = 1
    		// Write new configuration to device accounting for the write-allowed mask
    		for(i=0; i<NUM_REGS_MAX; i++){
    			rd = Reg_Store[i];
    			iic_write8_mask( chip_addr, rd.Reg_Addr, rd.Reg_Val, rd.Reg_Mask, i2c0);
    	    #ifdef DEBUG_REG
    	      Status = iic_read8( chip_addr ,rd.Reg_Addr, &readback_test, i2c0);
    	      if(Status != XST_SUCCESS) {
    	        p_printf(("si5338_init: Can't read register\r\n"));
    	        return Status;
    	      }
    	      if (rd.Reg_Val != readback_test) {
    	        p_printf(("address 0x%04X: Write 0x%02X to  and read 0x%02X  ----- Difference detected please check.\r\n",rd.Reg_Addr, rd.Reg_Addr, readback_test));
    	      } else {
    	        p_printf(("address 0x%04X: Write 0x%02X to  and read 0x%02X.\r\n",rd.Reg_Addr, rd.Reg_Addr, readback_test));
    	      }
    	    #endif
    		}
    		// Validate clock input status
    	//	reg_val = iic_read8( chip_addr , 218) & LOS_MASK;
    		do{
    			Status = iic_read8( chip_addr , 218, &reg_val, i2c0);
    	        if(Status != XST_SUCCESS) {
    	            p_printf(("si5338_init: Can't read register\r\n"));
    	            return Status;
    	        }
    		}
    		while((reg_val & LOS_MASK) != 0);

    		// Configure PLL for locking
    		iic_write8_mask( chip_addr, 49, 0, FCAL_OVRD_EN, i2c0);	//FCAL_OVRD_EN = 0
    		// Initiate Locking of PLL
    		iic_write8( chip_addr, 246, SOFT_RESET, i2c0);			//SOFT_RESET = 1
    		iic_delay(25);											// Wait 25 ms
    		// Restart LOL
    		iic_write8_mask( chip_addr, 241, 0, DIS_LOL, i2c0);		// DIS_LOL = 0
    		iic_write8( chip_addr, 241, 0x65, i2c0);				// Set reg 241 = 0x65
    		// Confirm PLL lock status
    		do{
    			Status = iic_read8( chip_addr, 218, &reg_val, i2c0);
    	        if(Status != XST_SUCCESS) {
    	            p_printf(("si5338_init: Can't read register\r\n"));
    	            return Status;
    	        }
    		}
    		while((reg_val & LOCK_MASK) != 0);
    		//copy FCAL values to active registers
    		Status = iic_read8( chip_addr, 237, &reg_val,i2c0);
    		iic_write8_mask( chip_addr, 47, reg_val, 0x03,i2c0);	// 237[1:0] to 47[1:0]
    		Status = iic_read8( chip_addr, 236, &reg_val,i2c0);
    		iic_write8( chip_addr, 46, reg_val,i2c0);	// 236[7:0] to 46[7:0]
    		Status = iic_read8( chip_addr, 235, &reg_val,i2c0);
    		iic_write8( chip_addr, 45, reg_val,i2c0);	// 235[7:0] to 45[7:0]
    		iic_write8_mask( chip_addr, 47, 0x14, 0xFC,i2c0);		// Set 47[7:2] = 000101b
    		// Set PLL to use FCAL values
    		iic_write8_mask( chip_addr, 49, FCAL_OVRD_EN, FCAL_OVRD_EN,i2c0);	//FCAL_OVRD_EN = 1
    		// Enable Outputs
    		iic_write8( chip_addr, 230, 0x00,i2c0);					//EOB_ALL = 0
    }else if (interface == i2c1)
    {
    	// I2C Programming Procedure
    		iic_write8( chip_addr, 246, 0x01,i2c1);					//Hard reset
    		// Disable Outputs
    		iic_write8_mask( chip_addr, 230, EOB_ALL, EOB_ALL,i2c1);	// EOB_ALL = 1
    		// Pause LOL
    		iic_write8_mask( chip_addr, 241, DIS_LOL, DIS_LOL,i2c1);	// DIS_LOL = 1
    		// Write new configuration to device accounting for the write-allowed mask
    		for(i=0; i<NUM_REGS_MAX; i++){
    			rd = Reg_Store[i];
    			iic_write8_mask( chip_addr, rd.Reg_Addr, rd.Reg_Val, rd.Reg_Mask,i2c1);
    	    #ifdef DEBUG_REG
    	      Status = iic_read8( chip_addr ,rd.Reg_Addr, &readback_test,i2c1);
    	      if(Status != XST_SUCCESS) {
    	        p_printf(("si5338_init: Can't read register\r\n"));
    	        return Status;
    	      }
    	      if (rd.Reg_Val != readback_test) {
    	        p_printf(("address 0x%04X: Write 0x%02X to  and read 0x%02X  ----- Difference detected please check.\r\n",rd.Reg_Addr, rd.Reg_Addr, readback_test));
    	      } else {
    	        p_printf(("address 0x%04X: Write 0x%02X to  and read 0x%02X.\r\n",rd.Reg_Addr, rd.Reg_Addr, readback_test));
    	      }
    	    #endif
    		}
    		// Validate clock input status
    	//	reg_val = iic_read8( chip_addr , 218) & LOS_MASK;
    		do{
    			Status = iic_read8( chip_addr , 218, &reg_val,i2c1);
    	        if(Status != XST_SUCCESS) {
    	            p_printf(("si5338_init: Can't read register\r\n"));
    	            return Status;
    	        }
    		}
    		while((reg_val & LOS_MASK) != 0);

    		// Configure PLL for locking
    		iic_write8_mask( chip_addr, 49, 0, FCAL_OVRD_EN,i2c1);	//FCAL_OVRD_EN = 0
    		// Initiate Locking of PLL
    		iic_write8( chip_addr, 246, SOFT_RESET,i2c1);			//SOFT_RESET = 1
    		iic_delay(25);											// Wait 25 ms
    		// Restart LOL
    		iic_write8_mask( chip_addr, 241, 0, DIS_LOL,i2c1);		// DIS_LOL = 0
    		iic_write8( chip_addr, 241, 0x65,i2c1);				// Set reg 241 = 0x65
    		// Confirm PLL lock status
    		do{
    			Status = iic_read8( chip_addr, 218, &reg_val,i2c1);
    	        if(Status != XST_SUCCESS) {
    	            p_printf(("si5338_init: Can't read register\r\n"));
    	            return Status;
    	        }
    		}
    		while((reg_val & LOCK_MASK) != 0);
    		//copy FCAL values to active registers
    		Status = iic_read8( chip_addr, 237, &reg_val,i2c1);
    		iic_write8_mask( chip_addr, 47, reg_val, 0x03,i2c1);	// 237[1:0] to 47[1:0]
    		Status = iic_read8( chip_addr, 236, &reg_val,i2c1);
    		iic_write8( chip_addr, 46, reg_val,i2c1);	// 236[7:0] to 46[7:0]
    		Status = iic_read8( chip_addr, 235, &reg_val,i2c1);
    		iic_write8( chip_addr, 45, reg_val,i2c1);	// 235[7:0] to 45[7:0]
    		iic_write8_mask( chip_addr, 47, 0x14, 0xFC,i2c1);		// Set 47[7:2] = 000101b
    		// Set PLL to use FCAL values
    		iic_write8_mask( chip_addr, 49, FCAL_OVRD_EN, FCAL_OVRD_EN,i2c1);	//FCAL_OVRD_EN = 1
    		// Enable Outputs
    		iic_write8( chip_addr, 230, 0x00,i2c1);					//EOB_ALL = 0
    }

    p_printf(("Si5338 Init Complete\r\n"));
	return XST_SUCCESS;
}

#endif
