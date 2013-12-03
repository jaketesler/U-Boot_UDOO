<br><br><a name="cheatsheet"></a>
#U-Boot File/Folder Cheat Sheet
###This section lists significant files and what they do, or what to do with them. I have tried to indicate what files I have changed.

*	/board/freescale/mx6\_udoo/mx6_udoo.h
	* This is the main U-Boot UART switch file for both Dual-Core and Quad-Core platforms. It essentially runs the entire U-Boot overarching sequence, but there is a def at the top [CONFIGJ\_USE\_UART\_(x)]. Use this switch to flip between UARTs 2 and 4. 
<br><br>

*	/include/configs/mx6dl_udoo.h
	*	(DUAL-CORE) This is the (most important file, but really the) universal U-Boot configuration file for the platform. Make all changes for U-Boot here, including config options and default environment variables for U-Boot
<br><br>

*	/include/configs/mx6q_udoo.h
	*	(QUAD-CORE) This is the universal U-Boot configuration file for the platform. Make all changes for U-Boot here.
<br><br>	

*	/include/config_aditions.h
	*	A custom added file for additional configuration options
<br><br>

*	/lib\_arm/board.c
	*	showrunner, executes the whole sequence
	*	insert whatever printf lines here
<br><br>	

*	/common
	*	This folder contains the source code for all the commands that can be used while in the U-Boot bootloader before the boot sequence. Additionally, it contains some important files used throughout the process. 
	*	/common/console.c
		*	console multiplexer, starts the sequence, don't modify!
	*	/common/cmd_udooconfig.c
		*	This is a custom-built command specifically for the UDOO. It can modify memory allocation (GPU-CPU), change the clock speed, etc. [I find that the attributes that the utility can change can be done by hand a LOT easier than the utility, which has known to cause boot stalls due to a mis-imprinted env var for the startup partiton].
	*	/common/iomux.c
		*	CONFIG\_SERIAL_MULTI is activated here
	*	/common/serial.h
		*	This file was moved here after a compiler error
<br><br>

*	/include/asm-arm/arch-mx6/mx6.h
	*	initializes various memory locations for...everything. [added two clocks for UARTS]
<br><br>

*	/tools/[crc32.c, md5.c, sha1.c]
	*	These files were added to the 'tools' folder after a compiler error. This may or may not be an isolated issue. 

*	/compile.sh
	*	UDOO-tailored make-file executor
<br><br>

*	/Makefile
	*	Custom Makefile for our purposes