/*
 * Copyright (C) 2007-2011 Freescale Semiconductor, Inc.
 *
 * This file is licensed under the terms of the GNU General Public
 * License Version 2. This file is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef _CONFIG_CMD_ADDITIONS_H
#define _CONFIG_CMD_ADDITIONS_H

/*
 * Alphabetical list of all commands that are configured by default.
 * This is essentially all commands minus those that are considered
 * "non-standard" for some reason (memory hogs, requires special
 * hardware, not fully tested, etc.).
 */

/*
 * These commands were added here by us to experiment with the UDOO U-Boot image. 
 */

//#define CONFIG_CMD_DIAG		/* (POST?) Diagnostics			*/
#define CONFIG_CMD_BSP		/* Board Specific functions	*/
#define CONFIG_CMD_DTT		/* Digital Therm and Thermostat */
#define CONFIG_CMD_ITEST	/* Integer (and string) test	*/
#define CONFIG_CMD_REGINFO	/* Register dump		*/
#define CONFIG_CMD_SDRAM	/* SDRAM DIMM SPD info printout */
#define CONFIG_CMD_SNTP		/* SNTP support		*/
#define CONFIG_CMD_TERMINAL	/* built-in Serial Terminal	*/

//#define CONFIG_CMD_USB		/* USB Support			*/ ##crashed compiler, 
//##/home/ubuntu/udoocode/U-Boot_UDOO/include/usb.h:203:2: error: #error USB Lowlevel not defined



#endif	/* _CONFIG_CMD_ADDITIONS_H */
