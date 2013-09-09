/*
 * Command for accessing SPI flash.
 *
 * Copyright (C) 2008 Atmel Corporation
 */
#include <common.h>
#include <spi_flash.h>

#include <asm/io.h>
#include <spi.h>
#include <rtc.h>
#include <linux/time.h>

DECLARE_GLOBAL_DATA_PTR;

static int do_rtc(int argc, char *argv[])
{
	
	struct spi_slave *spi;
	struct rtc_time *tm = malloc(sizeof(*tm));
	u32 counter,utc_time;
	int err = 0;
	
	if ( gd->bd->board_revision[1] < 0x31 ){
		printf("No rtc - board revision %x\n",gd->bd->board_revision[1]);
		return 0;
	}
	
	/*--Reading RTC SPI--*/
	err = rtc_get(tm);
	if ( err != 0 || 
	     (tm->tm_sec == 0  &&
	     tm->tm_min == 0  &&
	     tm->tm_hour == 0 &&
	     tm->tm_mday == 0 &&
	     tm->tm_mon  == 0 &&
	     tm->tm_wday == 0)){
		printf("No SPI RTC\n...waiting 1 sec and check again...\n");
	  	udelay (1000*1000);
		if ( err != 0 ||
		     (tm->tm_sec == 0  &&
		     tm->tm_min == 0  &&
		     tm->tm_hour == 0 &&
		     tm->tm_mday == 0 &&
		     tm->tm_mon  == 0 &&
		     tm->tm_wday == 0)){
			printf("Can Get Time from SPI RTC\n");	
			return -1;		
		}
	}
	
	printf("\nSpi Time: %d:%d:%d",tm->tm_hour,tm->tm_min,tm->tm_sec);
	printf("\nSpi Date: %d-%d-%d day of week %d",tm->tm_mday,tm->tm_mon,tm->tm_year,tm->tm_wday);
        printf("\n");

	/*--Reading RTC iMX6--*/
	rtc_internal_get(&counter);

	utc_time = mktime (tm->tm_year,
			   tm->tm_mon,
        		   tm->tm_mday,
			   tm->tm_hour,
			   tm->tm_min,
			   tm->tm_sec);

	printf("\nUTC from Spi : %lu\n",utc_time);
	
	/*--Setting Time from SPI to iMX6--*/
	rtc_internal_set(utc_time);	

	return 0;

}

U_BOOT_CMD(
	rtc,	5,	1,	do_rtc,
	"RTC reader sub-system",
	"probe [bus:]cs [hz] [mode]	- init flash device on given SPI bus\n"
	"				  and chip select\n"
	"sf read addr offset len 	- read `len' bytes starting at\n"
	"				  `offset' to memory at `addr'\n"
	"sf write addr offset len	- write `len' bytes from memory\n"
	"				  at `addr' to flash at `offset'\n"
	"sf erase offset len		- erase `len' bytes from `offset'"
);
