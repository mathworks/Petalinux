/*
-- Company: 		Trenz Electronic
-- Engineer: 		Oleksandr Kiyenko / John Hartfiel / Mohsen Chamanbaz
 
-- Code REV01
-- REV00 to REV01 changes:
    -- Changes implemented by Mohsen Chamanbaz (MC) in Sep. 2022
    -- Function predefinition changed. 
 */

#ifndef SRC_SI5338_H_
#define SRC_SI5338_H_

#define code

#include "te_iic_platform.h"
#ifdef CLOCK_SI5338


// #define SI5338_CHIP_ADDR		0x70

#define TEST_REG_ADDR			0x00

#define LOS_MASK_IN1IN2IN3		0x04
#define LOS_MASK				LOS_MASK_IN1IN2IN3
#define PLL_LOL					0x10
#define LOS_FDBK				0x08
#define LOS_CLKIN				0x04
#define SYS_CAL					0x01
#define LOCK_MASK				(PLL_LOL | LOS_CLKIN | SYS_CAL)
#define FCAL_OVRD_EN			0x80
#define SOFT_RESET				0x02
#define EOB_ALL					0x10
#define DIS_LOL					0x80

//enable register read back and printf
//#define DEBUG_REG	

//max delay for calibration from SI documentation 300ms
#define TIME_CHECK_PLL_CONFIG_US	0x50000U
//delay
#define DELAY_AFTER_PLL_CONFIG_US	0x20000U 
// #define DELAY_AFTER_PLL_CONFIG_US	0x100000U 

int si5338_version(unsigned char chip_addr,unsigned char bus);
int si5338_status_wait(unsigned char chip_addr,unsigned char bus);
int si5338_init(unsigned char chip_addr,unsigned char bus);

#endif /* CLOCK_SI5338 */
#endif /* SRC_SI5338_H_ */
