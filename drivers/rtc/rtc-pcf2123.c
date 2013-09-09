/*
 * Command for accessing SPI flash.
 *
 * Copyright (C) 2008 Atmel Corporation
 */
#include <common.h>
#include <command.h>
#include <spi_flash.h>

#include <asm/io.h>
#include <spi.h>
#include <rtc.h>
#include <linux/time.h>


#define MAX_SPI_BYTES 			12     /* Maximum number of bytes we can handle */
#define TIME				4
#define DATE				4
#define RESET				4
#define YEAR				2


#define MAX_HZ 				1000000
#define SPI_MODE			SPI_CS_HIGH
#define CS				3
#define BUS				0				
#define CMD_READ			0x03
//#define LEN				8
//#define BITLEN				80

#define PCF2123_REG_CTRL1       (0x00)  /* Control Register 1 */
#define PCF2123_REG_CTRL2       (0x01)  /* Control Register 2 */
#define PCF2123_REG_SC          (0x02)  /* datetime */
#define PCF2123_REG_MN          (0x03)
#define PCF2123_REG_HR          (0x04)
#define PCF2123_REG_DM          (0x05)
#define PCF2123_REG_DW          (0x06)
#define PCF2123_REG_MO          (0x07)
#define PCF2123_REG_YR          (0x08)

#define PCF2123_SUBADDR         (1 << 4)
#define PCF2123_WRITE           ((0 << 7) | PCF2123_SUBADDR)
#define PCF2123_READ            ((1 << 7) | PCF2123_SUBADDR)

unsigned char           time[TIME];
unsigned char 		date[DATE];
unsigned char           reset[RESET];
unsigned char		year[YEAR];
u64			reg_imx6;

static unsigned bcd2bin (uchar n)
{
        return ((((n >> 4) & 0x0F) * 10) + (n & 0x0F));
}


static void spi_cs3_direction_output(void){

	/* Set as output */
        reg_imx6 = readl(GPIO2_BASE_ADDR + GPIO_GDIR);
        reg_imx6 |= (1 << 17);
        writel(reg_imx6, GPIO2_BASE_ADDR + GPIO_GDIR);

}

static void spi_cs3_low(void){

	/* Set Low */
        reg_imx6 = readl(GPIO2_BASE_ADDR + GPIO_DR);
        reg_imx6 &= ~(1 << 17);
        writel(reg_imx6, GPIO2_BASE_ADDR + GPIO_DR);

}


static void spi_cs3_high(void){

 	/* Set High */
        reg_imx6 = readl(GPIO2_BASE_ADDR + GPIO_DR);
        reg_imx6 |= (1 << 17);
        writel(reg_imx6, GPIO2_BASE_ADDR + GPIO_DR);


}

int rtc_set(struct rtc_time *tm)
{
        	printf("\nFunction not Implemented!\n"); 
	        return 0;
}

int rtc_get(struct rtc_time *tm){

 	struct spi_slave *spi;
        //struct rtc_time *tm = malloc(sizeof(*tm));
        int     j,ret;
        unsigned char cmd[MAX_SPI_BYTES],time[7];
        int counter;

	spi = spi_setup_slave (BUS, CS, MAX_HZ, SPI_MODE);
	if(!spi){
		printf("Error in setup slave!");
		return -1;
	}
	
	ret = spi_claim_bus(spi);
	if( ret < 0 ){
		printf("Error Claiming Bus!\n");
        	printf("Release Bus and Claiming Again");
		spi_release_bus(spi);
		printf("\nClaiming Again");
		ret = spi_claim_bus(spi);
		if(!ret){
			printf("\nError Claiming Bus!\n");
			return ret;
		}
	}

	/* Set Direction */
	spi_cs3_direction_output();

	/*READ TIME OPERATION*/	

	cmd[0] = 0x00;
        cmd[1] = 0x00;
	cmd[2] = 0x00;
	cmd[3] = PCF2123_READ | PCF2123_REG_SC;
	
	for (j = 4; j < MAX_SPI_BYTES; j++){
        cmd[j] = 0x0;
        }

        for (j = 0; j < TIME; j++){
        time[j] = 0x0;
        }		
	
	/* Set High */
        spi_cs3_high();

		if(spi_xfer(spi, TIME*8, cmd, time,
				 SPI_XFER_BEGIN | SPI_XFER_END) != 0) {
			printf("Error with the SPI transaction.\n");
			return -1;
		}
	
	/* Set Low */
	spi_cs3_low();	

		tm->tm_sec     = bcd2bin(time[2] & 0x7f);
                tm->tm_min     = bcd2bin(time[1] & 0x7f);
                tm->tm_hour    = bcd2bin(time[0] & 0x3f);

	/*READ DATE OPERATION*/

	for (j = 0; j < MAX_SPI_BYTES; j++){
        cmd[j] = 0x0;
        }

        cmd[0] = 0x00;
	cmd[1] = 0x00;
	cmd[2] = 0x00;
	cmd[3] = PCF2123_READ | PCF2123_REG_DM;
        
	for (j = 4; j < MAX_SPI_BYTES; j++){
        cmd[j] = 0x0;
        }

	for (j = 0; j < DATE; j++){
        date[j] = 0x0;
        }

        /* Set High */
        spi_cs3_high();

                if(spi_xfer(spi, DATE*8, cmd, date,
                                 SPI_XFER_BEGIN | SPI_XFER_END) != 0) {
                        printf("Error with the SPI transaction.\n");
                        return -1;
                }

        /* Set Low */
        spi_cs3_low();

	/*READ YEAR OPERATION*/

        for (j = 0; j < MAX_SPI_BYTES; j++){
        cmd[j] = 0x0;
        }

        cmd[0] = 0x00;
        cmd[1] = PCF2123_READ | PCF2123_REG_YR;
        for (j = 2; j < MAX_SPI_BYTES; j++){
        cmd[j] = 0x0;
        }

        for (j = 0; j < YEAR; j++){
        year[j] = 0x0;
        }

        /* Set High */
        spi_cs3_high();

                if(spi_xfer(spi, YEAR*8, cmd, year,
                                 SPI_XFER_BEGIN | SPI_XFER_END) != 0) {
                        printf("Error with the SPI transaction.\n");
                        return -1;
                }

        /* Set Low */
        spi_cs3_low();
	spi_release_bus(spi);
        spi_free_slave(spi);

		tm->tm_mday    = bcd2bin(date[2] & 0x3f);
		tm->tm_wday    = date[1] & 0x07;
		tm->tm_mon     = bcd2bin(date[0] & 0x1f);
		tm->tm_year    = bcd2bin(year[0]);
		if (tm->tm_year < 38)
        	      	tm->tm_year += 2000;
		else
			tm->tm_year += 1900;
        	 tm->tm_yday = 0;
               	 tm->tm_isdst = 0;
	
	return 0;	


}
void rtc_reset(void){

	struct spi_slave *spi;
        int     j,ret;
        unsigned char cmd[MAX_SPI_BYTES];
	
	printf("\nRTC - Sending Reset Commnand\n");

	spi = spi_setup_slave (BUS, CS, MAX_HZ, SPI_MODE);
	
	if(!spi){
		printf("Error in setup slave!");
		return;
	}
	
	//printf("\nSlave correctly Set\n");

	ret = spi_claim_bus(spi);
	if( ret < 0 ){
		printf("Error Claiming Bus!\n");
        	printf("Release Bus and Claiming Again");
		spi_release_bus(spi);
		printf("\nClaiming Again");
		ret = spi_claim_bus(spi);
		if(!ret){
			printf("\nError Claiming Bus!\n");
			return;
		}
	}

	/* Set Direction */
	spi_cs3_direction_output();

        /* Set Low */
        spi_cs3_low();	
	
	/*SOFTWARE RESET*/

        cmd[0] = 0x58;
	cmd[1] = PCF2123_WRITE | PCF2123_REG_CTRL1;
        for (j = 2; j < MAX_SPI_BYTES; j++){
        cmd[j] = 0x0;
        }

        for (j = 0; j < RESET; j++){
        reset[j] = 0x0;
        }	
	
	 /* Set High */
        spi_cs3_high();

                if(spi_xfer(spi, 2*8, cmd, reset,
                                 SPI_XFER_BEGIN | SPI_XFER_END) != 0) {
                        printf("Error with the SPI transaction.\n");
                        return;
                }
        
	/* Set Low */
        spi_cs3_low();	
	spi_release_bus(spi);
        spi_free_slave(spi);

	printf("\nSuccess!\n");
	

}
        
