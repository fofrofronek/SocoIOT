
#include <stdio.h>
#include <stdlib.h>
#include "ff.h"


#define SDCARD_PIN_SPI0_CS=22
#define	SDCARD_PIN_SPI0_SCK=5
#define SDCARD_PIN_SPI0_MOSI=18
#define	SDCARD_PIN_SPI0_MISO=19
#define SDCARD_PIO=pio1
#define SDCARD_PIO_SM=0

FATFS fs;
FIL fil;
FRESULT fr;
FIL afil;
DIR dir;

#define RDLEN 80

char rdbuff[RDLEN]={0};


int mount_sd(void) {
	fr = f_mount(&fs, "", 1);
	if (fr != FR_OK) {
		printf("Failed to mount SD card, error: %d\n", fr);
		return 1;
	}

	FILINFO file;
	//auto dir = new DIR();
	printf("Listing /\n");
	f_opendir(&dir, "/");
	while(f_readdir(&dir, &file) == FR_OK && file.fname[0]) {
		printf("%s %lld\n", file.fname, file.fsize);
	}
	f_closedir(&dir);

	return 0;
}

int cat_file(void)
{
	printf("cat Config.txt\n");

	fr = f_open(&fil, "Config.txt", FA_READ);
		if (fr != FR_OK) {
		printf("File not found, error: %d\n", fr);
		return 1;
	}

	while(f_gets(rdbuff,RDLEN, &fil))
	{
		printf("%s",rdbuff);
	}

	f_close(&fil);

}