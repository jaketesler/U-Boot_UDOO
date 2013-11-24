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


#define CONFIG_CMD_BSP		/* Board Specific functions	*/
#define CONFIG_CMD_ITEST	/* Integer (and string) test	*/
#define CONFIG_CMD_REGINFO	/* Register dump		*/
#define CONFIG_CMD_SDRAM	/* SDRAM DIMM SPD info printout */
#define CONFIG_CMD_SNTP		/* SNTP support		*/


#define CONFIG_LOOPW
		/*Add the "loopw" memory command. This only takes effect if
		the memory commands are activated globally (CONFIG_CMD_MEM).*/

//#define CONFIG_SYS_ALT_MEMTEST
		/*Enable an alternate, more extensive memory test.*/ 
//#define CONFIG_SYS_MEMTEST_SCRATCH
				/*Scratch address used by the alternate memory test
				You only need to set this if address zero isn't writeable*/
//##disabled because...we don't need it.
		


//#define CONFIG_CMD_DIAG		/* (POST?) Diagnostics			*/


//#define CONFIG_CMD_DTT		/* Digital Therm and Thermostat */ ##crashed compiler,
//##cmd_dtt.c: In function ‘do_dtt’:
//##cmd_dtt.c:34:28: error: ‘CONFIG_DTT_SENSORS’ undeclared (first use in this function)


//#define CONFIG_CMD_TERMINAL	/* built-in Serial Terminal	*/ ##crashed compiler, 
//##common/libcommon.a(cmd_terminal.o): In function `do_terminal':
//##/home/ubuntu/udoocode/U-Boot_UDOO/common/cmd_terminal.c:45: undefined reference to `serial_reinit_all'


//#define CONFIG_CMD_USB		/* USB Support			*/ ##crashed compiler, 
//##/home/ubuntu/udoocode/U-Boot_UDOO/include/usb.h:203:2: error: #error USB Lowlevel not defined


#endif	/* _CONFIG_CMD_ADDITIONS_H */


/* OTHER COMMANDS POSSIBLY TO INTEGRATE
- Show boot progress:
		CONFIG_SHOW_BOOT_PROGRESS

		Defining this option allows to add some board-
		specific code (calling a user-provided function
		"show_boot_progress(int)") that enables you to show
		the system's boot progress on some display (for
		example, some LED's) on your board. At the moment,
		the following checkpoints are implemented:*/