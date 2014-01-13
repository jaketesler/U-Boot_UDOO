[altpin]: http://udoo.org/download/files/pinout/UDOO_pinout_alternate_table.pdf "UDOO Alternate Pin Table"
#U-Boot Readme
##Info:
To flash the U-Boot image, use the `dd` utility directly to the main disk. 
For example, if the main disk (e.g. SD card) is `disk1` on your system, unmount any partitions and run this in a command line:
	
	$ cd U-Boot_UDOO
	$ sudo dd if=u-boot.bin of=/dev/disk1 bs=512 seek=2 skip=2



<br><br><a name="cheatsheet"></a>
#U-Boot File/Folder Cheat Sheet
###This section lists significant files and what they do, or what to do with them. I have tried to indicate what files I have changed.
###To change from one serial (radio) UART/tty to a different one, please modify files marked with a __**(U)**__ in the manner noted. Be sure to do this here and for the U-BOOT source as well, as modifying one source (e.g. U-BOOT) and compiling without changing the other (e.g. kernel) will crash the UDOO. <br>I have tried to be as specific as possible. When modifying, please try to update the comments as well.<br>Also, keep in mind that UART4==ttymxc3==imx-uart.3, i.e. UART(#)==ttymxc(#-1)==imx-uart.(#-1).

Hierarchy:

*	board/				[Files specific to platforms]
	*	freescale/		[Freescale platform]
		*	mx6\_udoo/	[UDOO board]
*	include/			[Header files for UDOO]
	*	asm-arm/		[ARM]
		*	arch-mx6/	[i.MX6 architecture]
	*	configs/		[Configuration files for a number of different platforms/boards]
*	lib\_arm/			[Files specific to booting ARM systems]
*	common/
	*	This folder contains the source code for all the commands that can be used while in the U-Boot bootloader before the boot sequence. Additionally, it contains some important files used throughout the process. 
*	tools/				[Tools for compiling]

<br><br>

Files:

*	/board/
	*	freescale/
		*	mx6\_udoo/
			*	mx6_udoo.c
				*	This is the main U-Boot UART switch file for both Dual-Core and Quad-Core platforms. It 
				essentially runs the entire U-Boot overarching sequence, but there is a def at the top 
				[CONFIGJ\_USE\_UART\_(x)]. Use this switch to flip between UARTs 2 and 4. 
				*	(U) Change lines 404-407 and 420-423. Lookup the correct KERPin either in the kernel header or in the [Alt Pin Table][altpin]. Change to reflect new UART. Don't change the \#define line.
				<br><br>

*	/include/
	*	config_aditions.h
		*	A custom added file for additional configuration options
		<br><br>
	*	asm-arm/
		*	arch-mx6/
			*	mx6.h
				*	initializes various memory locations for...everything. [added two clocks for UARTs]
				*	(U) Do not change anything in this file, but this file is needed to get the MIMO location of specific UARTs, needed for another change. 
				<br><br>

	*	configs/
		*	mx6dl_udoo.h
			*	(DUAL-CORE) This is the (most important file, but really the) universal U-Boot configuration 
			file for the platform. Make all changes for U-Boot here, including config options and default 
			environment variables for U-Boot
			*	(U) Here is the big file. I recommend making a new copy and working from that, as line numbers may change as the file is modified. The steps here refer to the original file line numbers. 
				*	Comment lines 130 and 131. Add two lines in place using correct UART Address values gathered from /include/asm-arm/arch-mx6/mx6.h. 
				*	Comment line 136. Added replacement reflecting new UART. 
				*	Change lines 149-151 to reflect new tty.
				*	Line 218-259: replace all instances of ttymxc# with the new #. Modify MMIO value with value from [the MMIO list below](#mmio).
			<br><br>

		*	mx6q_udoo.h
			*	(QUAD-CORE) This is the universal U-Boot configuration file for the platform. Make all 
			changes for U-Boot here.
			*	(U) Mirror steps from dual-core. 
			<br><br>

*	/lib\_arm/
	*	board.c
		*	showrunner, executes the whole sequence
		*	insert whatever printf lines here
	<br><br>	

*	/common/
	*	console.c
		*	console multiplexer, starts the sequence, don't modify!
<br><br>
	*	cmd_udooconfig.c
		*	This is a custom-built command specifically for the UDOO. It can modify memory allocation (GPU-CPU), change the clock speed, etc. [I find that the attributes that the utility can change can be done by hand a LOT easier than the utility, which has known to cause boot stalls due to a mis-imprinted env var for the startup partiton].
	*	iomux.c
		*	CONFIG\_SERIAL_MULTI is activated here
<br><br>
	*	serial.h
		*	This file was moved here after a compiler error
<br><br>

*	/tools/
	*	[crc32.c, md5.c, sha1.c]
		*	These files were added to the 'tools' folder after a compiler error. This may or may not be an isolated issue. 
<br><br>
*	/compile.sh
	*	UDOO-tailored make-file executor
<br><br>

*	/Makefile
	*	Custom Makefile for our purposes
<br><br>
*	/MAKEALL
	*	Never use this. Ever. Don't do it. It makes EVERYTHING, and we have removed a lot of the rules and 
	dependencies. 
	
<br>
<br><br><a name="mmio"></a>
###MMIO Values:
*	UART1 imx-uart.0: ttymxc0 at MMIO 0x2020000 (irq = 58) is a IMX
*	UART2 imx-uart.1: ttymxc1 at MMIO 0x21e8000 (irq = 59) is a IMX
*	UART3 imx-uart.2: ttymxc2 at MMIO 0x21ec000 (irq = 60) is a IMX
*	UART4 imx-uart.3: ttymxc3 at MMIO 0x21f0000 (irq = 61) is a IMX
*	UART5 imx-uart.4: ttymxc4 at MMIO 0x21f4000 (irq = 62) is a IMX