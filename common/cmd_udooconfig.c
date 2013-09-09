/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Boot support
 */
#include <common.h>
#include <command.h>
#include <stdio_dev.h>

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <stdio_dev.h>
#include <timestamp.h>
#include <version.h>
#include <net.h>
#include <serial.h>
#include <nand.h>
#include <onenand_uboot.h>
#include <mmc.h>


#define DEF_memory "mem=1500M"
#define DEF_bootdev "mmc dev 2"
#define DEF_root "root=/dev/mmcblk2p1"
#define DEF_option "rootwait rw fixrtc rootflags=barrier=1"
#define DEF_setvideomode "setenv videomode video=mxcfb0:dev=hdmi,1920x1080M@60,if=RGB24 fbmem=30M"
#define DEF_lvdsres1 ""
#define DEF_lvdsres2 ""
#define DEF_cpu_freq "arm_freq=792"

#define getDefValue(var) DEF_ ## var 

#define getEnvStr(env) ( getenv(# env) ? getenv(# env) : DEF_ ## env )

#ifdef UDOO
	// device num for u-boot, to load the kernel
#define K_EXT_SD_DEV_ID "2"
	// device num for kernel, to mount the FileSystem
#define EXT_SD_DEV_ID "2"
#endif

int atoi(char *string)
{
	int length;
	int retval = 0;
	int i;
	int sign = 1;

	length = strlen(string);
	for (i = 0; i < length; i++) {
		if (0 == i && string[0] == '-') {
			sign = -1;
			continue;
		}
		if (string[i] > '9' || string[i] < '0') {
			break;
		}
		retval *= 10;
		retval += string[i] - '0';
	}
	retval *= sign;
	return retval;
}

static char *getline(void)
{
	static char buffer[100];
	char c;
	size_t i;

	i = 0;
	while (1) {
		buffer[i] = '\0';
		while (!tstc())
			continue;

		c = getc();
		/* Convert to uppercase */
		//if (c >= 'a' && c <= 'z')
		//	c -= ('a' - 'A');

		switch (c) {
		case '\r':	/* Enter/Return key */
		case '\n':
			puts("\n");
			return buffer;

		case 0x03:	/* ^C - break */
			return NULL;

		case 0x5F:
		case 0x08:	/* ^H  - backspace */
		case 0x7F:	/* DEL - backspace */
			if (i) {
				puts("\b \b");
				i--;
			}
			break;

		default:
			/* Ignore control characters */
			if (c < 0x20)
				break;
			/* Queue up all other characters */
			buffer[i++] = c;
			printf("%c", c);
			break;
		}
	}
}


char *do_ramsize(ulong min, ulong max) {
	/*while ((min + num_choices*step) <= max) {
		printf ("%d) %uMB\n", num_choices, min + num_choices*step);
		num_choices++;	
	}*/
	char *line;
	int size = 0;
	do {
		printf ("Chose the ram memory size to dedicate to the Kernel.\n");
		printf ("[min size: %uM - max size %uM]\n", min, max);
		printf ("> ");
		line = getline ();	
	}while ((ulong)atoi(line) < min || (ulong)atoi(line) > max);
	printf ("Will use %uM of RAM memory\n\n",(ulong)atoi(line));
	return line;
}


#define MIN_PARTITION_ID 1
#define MAX_PARTITION_ID 5

#ifdef UDOO

void do_bootdev(char *bootdev) {
	char *line;
	char ch;
	char device[2];
	do {
		printf ("Chose boot Device for Kernel.\n");
		printf ("1) SD   External.\n");
		printf ("> ");
		ch = getc();
		printf ("%c\n", ch);
	} while ((ch < '1') || (ch > '1'));
	switch (ch) {
		case '1':
			strcpy (device, K_EXT_SD_DEV_ID);
			break;
		default:
			printf ("\nInvalid boot device. Valid option are from 1 to 1.\n\n");
			strcpy (bootdev, "");
			break;
	}	
	if (ch < '2') {
		strcpy (bootdev, "mmc dev ");
		strcat (bootdev, device);
		strcat (bootdev, "; booti mmc");
		strcat (bootdev, device);
		strcat (bootdev, ";");
	}
	do {
		printf ("Chose the partition of the selected device.\n");
		printf ("> ");
		line = getline ();	
	}while ((ulong)atoi(line) < MIN_PARTITION_ID || (ulong)atoi(line) > MAX_PARTITION_ID);
	strcat (bootdev, line);
	printf ("\n\n");
}

void do_rootdev(char *root, char *option) {
	char *line;
	char ch;
	char device[2];
	do {
		printf ("Chose root Device for File System.\n");
		printf ("1) SD   External.\n");
		printf ("> ");
		ch = getc();
		printf ("%c\n", ch);
	} while ((ch < '1') || (ch > '1'));
	switch (ch) {
		case '1':
			strcpy (device, EXT_SD_DEV_ID);
			break;
		default:
			printf ("\nInvalid boot device. Valid option are from 1 to 1.\n\n");
			strcpy (root, "");
			strcpy (option, "");
			break;
	}
		strcpy (option, "rootwait rw fixrtc rootflags=barrier=1");
	if (ch < '2') {
		strcpy (root, "root=/dev/mmcblk");
		strcat (root, device);
		strcat (root, "p");
	}
	do {
		printf ("Chose the partition of the selected device.\n");
		printf ("> ");
		line = getline ();	
	}while ((ulong)atoi(line) < MIN_PARTITION_ID || (ulong)atoi(line) > MAX_PARTITION_ID);
	strcat (root, line);
	printf ("\n\n");
}
#endif

int do_videomenu(char *setvideomode) {
	char ch;
	int nlvds = 1;
	do {
		printf ("Chose video output device for UDOO board.\n");
		printf ("1) Primary LVDS Only.\n");
		printf ("2) HDMI Only.\n");
		printf ("3) LVDS + HDMI.\n");
		printf ("4) HDMI + LVDS.\n");
		printf ("5) LVDS + LVDS.\n");
		printf ("6) LVDS + LVDS + HDMI.\n");
		printf ("7) HDMI + LVDS + LVDS.\n");
		printf ("> ");
		ch = getc();
		printf ("%c\n", ch);
	} while ((ch < '1') || (ch > '7'));
	switch (ch) {
		case '1':
			strcpy (setvideomode, "setenv videomode video=mxcfb0:dev=ldb,${lvdsres1}");
			nlvds = 1;				
			break;
		case '2':
			strcpy (setvideomode, "setenv videomode video=mxcfb0:dev=hdmi,1920x1080M@60,if=RGB24");
			nlvds = 0;
			break;
		case '3':
			strcpy (setvideomode, "setenv videomode video=mxcfb0:dev=ldb,${lvdsres1} video=mxcfb1:dev=hdmi,1920x1080M@60,if=RGB24");
			nlvds = 1;
			break;
		case '4':
			strcpy (setvideomode, "setenv videomode video=mxcfb0:dev=hdmi,1920x1080M@60,if=RGB24 setenv video1 video=mxcfb1:dev=ldb,${lvdsres1}");
			nlvds = 1;
			break;
		case '5':
			strcpy (setvideomode, "setenv videomode video=mxcfb0:dev=ldb,${lvdsres1} video=mxcfb1:dev=ldb,${lvdsres2}");
			nlvds = 2;
			break;
		case '6':
			strcpy (setvideomode, "setenv videomode video=mxcfb0:dev=ldb,${lvdsres1} video=mxcfb1:dev=ldb,${lvdsres2} video=mxcfb2:dev=hdmi,1920x1080M@60,if=RGB24");
			nlvds = 2;
			break;
		case '7':
			strcpy (setvideomode, "setenv videomode video=mxcfb0:dev=hdmi,1920x1080M@60,if=RGB24 video=mxcfb1:dev=ldb,${lvdsres1} video=mxcfb2:dev=ldb,${lvdsres2}");
			nlvds = 2;
			break;
		default:
			printf ("\nInvalid option. Valid choice are from 1 to 7.\n\n");
			strcpy (setvideomode, "");
			nlvds = 0;
			break;
	}
	printf ("\n\n");
	return nlvds;
}

void do_videoresmenu(char *lvdsres1, char *lvdsres2, int num_video) {
	char ch = '0';
	int i;
	strcpy (lvdsres1, "");
	strcpy (lvdsres2, "");
	for (i = 0 ; i < num_video ; i++){
		printf ("Chose video output resolution for N°%x LVDS display on UDOO board.\n", i+1);
		do {
			printf ("1) WVGA [800x480].\n");
			printf ("2) SVGA [800x600].\n");
			printf ("3) XGA  [1024x768].\n");
			printf ("4) WXGA [1368x768].\n");
			printf ("> ");
			ch = getc();
			printf ("%c\n", ch);
		} while ((ch < '1') || (ch > '4'));
		switch (ch) {
			case '1':
				if(i==0) strcpy (lvdsres1, "LDB-WVGA,if=RGB666");
				if(i==1) strcpy (lvdsres2, "LDB-WVGA,if=RGB666");
				break;
			case '2':
				if(i==0) strcpy (lvdsres1, "LDB-SVGA,if=RGB666");
				if(i==1) strcpy (lvdsres2, "LDB-WVGA,if=RGB666");
				break;
			case '3':
				if(i==0) strcpy (lvdsres1, "LDB-XGA,if=RGB666");
				if(i==1) strcpy (lvdsres2, "LDB-XGA,if=RGB666");
				break;
			case '4':
				if(i==0) strcpy (lvdsres1, "LDB-WXGA,if=RGB24");
				if(i==1) strcpy (lvdsres2, "LDB-WVGA,if=RGB666");
				break;
			default:
				printf ("\nInvalid option. Valid choice are from 1 to 4.\n\n");
				if(i==0) strcpy (lvdsres1, "");
				if(i==1) strcpy (lvdsres2, "");
				break;
		}
	}
	printf ("\n\n");
}

void do_maxcpuspeed(char *cpu_freq) {
	char ch;
	do {
		printf ("Chose MAX cpu frequency [default = 792 MHz].\n");
		printf ("1) 396 MHz.\n"); 
		printf ("2) 792 MHz.\n");
		printf ("3) 996 MHz.\n");
		printf ("> ");
		ch = getc();
		printf ("%c\n", ch);
	} while ((ch < '1') || (ch > '5'));
	switch (ch) {
		case '1':
			strcpy(cpu_freq, "arm_freq=396");
			break;
		case '2':
			strcpy(cpu_freq, "arm_freq=792");
			break;
		case '3':
			strcpy(cpu_freq, "arm_freq=996");
			break;
		default:
			printf ("\nInvalid cpu speed frequency. Valid level are from 1 to 3.\n\n");
			strcpy(cpu_freq, "");
			break;
	}
	printf ("\n\n");
}

void do_nfs_root(char *ip_local, char *ip_server, char *nfs_path) {
	char *line;
	
	do {
		printf ("Insert an address ip for this board\n");
		printf ("> ");
		line = getline ();
		printf ("%c\n", line);
	} while (0);
	strcpy(ip_local, line);
	
	do {
		printf ("Insert the address ip of the host machine\n");
		printf ("> ");
		line = getline ();
		printf ("%c\n", line);
	} while (0);
	strcpy(ip_server, line);
	
	do {
		printf ("Insert the nfs path of the host machine\n");
		printf ("> ");
		line = getline ();
		printf ("%c\n", line);
	} while (0);
	strcpy(nfs_path, line);
	printf ("\n\n");
}

void do_powermenu() {
	printf ("Chose power consumption level for UDOO board (lower level means lower consumption).\n");
	printf ("1) Level 1 (no pcie, no ethernet, no canbus, no SD, no HDMI).\n");
	printf ("2) Level 2 (no pcie, no canbus, no HDMI).\n");
	printf ("3) Level 3 (no pcie, no canbus).\n");
	printf ("4) Level 4 (Full peripheral power ).\n");
	printf (">");
}


int do_udooconfig (cmd_tbl_t * cmd, int flag, int argc, char *argv[]) {
	char buf [50];	
	int nlvds = 0;
	ulong min_size = 512;
	ulong max_size;

	ulong size = ram_size;
	max_size = size / (1 << 20); // get size in MB
	max_size -= 256; // 256M reserved to video memory
	
	char *line;
	
	char bootdev[100]; 
	char root[100];
	char option[50];
	
	char setvideomode[300];
	
	char lvdsres1[100];
	char lvdsres2[100];
	
	char cpu_freq[100];
	
	char ip_local[50];
	char ip_server[50];
	char nfs_path[300];
	
	int valid_param = 0;
	int nfs_set = 0;
	
	printf ("UDOO interactive configuration utility.\n");
		
	if (argc > 2)
		return cmd_usage(cmd);
	
	
	if (argc == 2 && strcmp(argv[1], "help") == 0) {
		return cmd_usage(cmd);
	}
	
	if (argc == 2) {
				
		 if (strcmp(argv[1], "default") == 0) {
		 	strcpy (buf, getDefValue(memory));
			strcpy (bootdev, getDefValue(bootdev));
			strcpy (root, getDefValue(root));
			strcpy (option, getDefValue(option));
			strcpy (setvideomode, getDefValue(setvideomode));
			strcpy (lvdsres1, getDefValue(lvdsres1));
			strcpy (lvdsres2, getDefValue(lvdsres2));
			strcpy (cpu_freq, getDefValue(cpu_freq));
			printf ("Default configuration setted\n");
			valid_param = 1;
		}  else {
			strcpy (buf, getEnvStr(memory));
			strcpy (bootdev, getEnvStr(bootdev));
			strcpy (root, getEnvStr(root));
			strcpy (option, getEnvStr(option));
			strcpy (setvideomode, getEnvStr(setvideomode));
			strcpy (lvdsres1, getEnvStr(lvdsres1));
			strcpy (lvdsres2, getEnvStr(lvdsres2));
			strcpy (cpu_freq, getEnvStr(cpu_freq));
		}

		if (strcmp(argv[1], "bootdev") == 0) {
			/* set Kernel source */
			do_bootdev (bootdev);	
			valid_param = 1;	
		} 
			
		if (strcmp(argv[1], "rootdev") == 0) {
			/* set File System source */
			do_rootdev (root, option);	
			valid_param = 1;	
		} 
	
		if (strcmp(argv[1], "memsize") == 0) {
			/* set memory size for the kernel */
			line = do_ramsize (min_size, max_size);
			sprintf (buf, "mem=%sM", line);
			valid_param = 1;
		} 
	
		if (strcmp(argv[1], "video") == 0) {
			nlvds = do_videomenu (setvideomode);
			/* set lvds resolution */
			do_videoresmenu (lvdsres1, lvdsres2, nlvds);
			valid_param = 1;
		}
	
		if (strcmp(argv[1], "cpufreq") == 0) {
			/* set cpu frequency */
			do_maxcpuspeed (cpu_freq);
			valid_param = 1;
		}
		
		if (strcmp(argv[1], "nfs") == 0) {
			do_nfs_root (ip_local, ip_server, nfs_path);
			nfs_set = 1;
			valid_param = 1;
		}
		
		if (valid_param == 0) {
			printf ("\nERROR: argument not valid!\n");
			return cmd_usage(cmd);
		}
	
	}
	
	if (argc == 1) {
		/* set memory size for the kernel */
		line = do_ramsize (min_size, max_size);
		sprintf (buf, "mem=%sM", line);
		/* set boot device and file system source device */
		do_bootdev (bootdev);
		do_rootdev (root, option);
		nlvds = do_videomenu (setvideomode);
		
		/* set lvds resolution */
		do_videoresmenu (lvdsres1, lvdsres2, nlvds);
		
		/* set cpu frequency */
		do_maxcpuspeed (cpu_freq);
		
	}
	
	if (nfs_set == 0) {
		setenv ("memory", buf);	
		setenv ("bootdev", bootdev);
		setenv ("root", root);
		setenv ("option", option);
		setenv ("setvideomode", setvideomode);
		setenv ("lvdsres1", lvdsres1);
		setenv ("lvdsres2", lvdsres2);
		setenv ("cpu_freq", cpu_freq);
		setenv ("run_from_nfs", "0");
		setenv ("fastboot_dev", "mmc2");
	} else {
		setenv ("ip_local", ip_local);
		setenv ("ip_server", ip_server);
		setenv ("nfs_path", nfs_path);
		setenv ("run_from_nfs", "1");
	}

	setenv("setbootargs_nfs", "setenv bootargs console=ttymxc1,115200 root=/dev/nfs nfsroot=${ip_server}:${nfs_path}nolock,wsize=4096,rsize=4096 ip=${ip_local} ${memory} ${cpu_freq} ${videomode}");
	setenv("setbootargs", "setenv bootargs console=ttymxc1,115200 ${root} ${option} ${memory} ${cpu_freq} ${videomode}");
	setenv("setbootdev", "setenv boot_dev ${bootdev}");
	setenv("bootcmd", "run setvideomode; run setbootargs; run setbootdev; run boot_dev;");
	saveenv();
	printf ("\n");
	return 0;
}

/***************************************************/

U_BOOT_CMD(
	udooconfig, 3, 1,	do_udooconfig,
	"Interactive setup for UDOO configuration.",
	"           - set whole environment\n"
	"udooconfig [default] - use default configuration\n"
	"udooconfig [bootdev] - set only boot device (Kernel source) and use remaining predefined parameters \n"
	"udooconfig [rootdev] - set only root device (File System source) and use remaining predefined parameters \n"
	"udooconfig [memsize] - set only RAM memory to use and use remaining predefined parameters \n"
	"udooconfig [video]   - set only video settings and use remaining predefined parameters \n"
	"udooconfig [cpufreq] - set only CPU's frequency and use remaining predefined parameters \n"
	"udooconfig [nfs]     - set the environment to use a NFS File System and use remaining predefined parameters \n"
);
