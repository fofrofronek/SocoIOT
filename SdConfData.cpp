#include "pico/stdlib.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include "sd_card.h"
#include "ff.h"
#include "SdConfData.h"
#include "ConfigParser.h"
#include "LEDs.h"
using namespace std;

extern SD Card;
extern LEDs Leds;


SD::SD()
{
    
}

void SD::Init()
{
    // Initialize SD card not in constructor due to timing
    logfile_is_open = false;
    initialized = false;
    mounted = false;
    if (!sd_init_driver()) {
        printf("ERROR: Could not initialize SD card\r\n");
    }
    else
    {
     printf("SD card initialized...\r\n");
     initialized = true;
    }
}

bool SD::is_initialized()
{
    return initialized;
}

bool SD::is_mounted()
{
    return mounted;
}

void SD::set_mounted()
{
    mounted = true;
}

void SD::set_unmounted()
{
    mounted = false;
}



Config::Config()
{
    // Mount drive
    fr = f_mount(&fs, "0:", 1);
    if (fr != FR_OK) {
        printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
        return;    
    }
    else
    { 
     printf("SD card mounted as drive 0:\r\n");
     Card.set_mounted();

    FILINFO file;
	//auto dir = new DIR();
	printf("Listing drive 0:/\n");
	f_opendir(&dir, "/");
	while(f_readdir(&dir, &file) == FR_OK && file.fname[0]) {
	printf("%s %lld\n", file.fname, file.fsize);
    f_closedir(&dir);
	}
	

    // Open file for reading
    fr = f_open(&fil, filename, FA_READ);
    if (fr != FR_OK) {
        printf("ERROR: Could not open file (%d)\r\n", fr);
        return;
    }

    // Print every line in file
    printf("Reading from file '%s':\r\n", filename);
    printf("---\r\n");
    while (f_gets(buf, sizeof(buf), &fil)) {
        //printf(buf);
        ConfigLine.assign(std::begin(buf), std::end(buf));
        ConfigLine.resize(strlen(buf));
        //LWIPconf.ParseCline(ConfigLine);
        LWIPconf.ParseClineDumbVer(ConfigLine);
    }
    printf("\r\n---\r\n");

    // Close file
    fr = f_close(&fil);
    if (fr != FR_OK) {
        printf("ERROR: Could not close file (%d)\r\n", fr);
        return;
    }

    }
    fr = f_unmount("0:");
    if (fr != FR_OK) {
        printf("ERROR: Could not umount filesystem (%d)\r\n", fr);
        return;
    }
    else
    {
        Card.set_unmounted();
        printf("SD card umounted successfully\r\n");
    }
}


void SD::StartLogging(){
    FRESULT fr;
    uint16_t namecounter;

    if(Card.is_mounted())
    {
        //Do nothing
    }
    else {
        // Mount drive
        fr = f_mount(&fs, "0:", 1);
        if (fr != FR_OK) {
            printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
            return;    
        }
        else
        { 
            printf("SD card mounted as drive 0:\r\n");
            Card.set_mounted();
        }
    }

    namecounter=0;
    do{
        logfile=to_string(namecounter);
        logfile += ".log";
        fr = f_stat(logfile.c_str(), &filnfo);
        namecounter++;
    }
    while (fr==FR_OK);

    // Open file for writing
    fr = f_open(&fil, logfile.c_str(), FA_CREATE_ALWAYS | FA_WRITE);
    if (fr != FR_OK) {
        printf("ERROR: Could not open file (%d) %s\r\n", fr, logfile.c_str());
        return;
    }

    logfile_is_open = true;
    Leds.USRled_on();




}

void SD::DoLogging(char* buffer, uint16_t count)
{
 UINT bytes_written;
 FRESULT fr;
 if(logfile_is_open) fr = f_write(&fil, buffer, count, &bytes_written);
 std::cout << "Log r=" << fr << " bytes:" << bytes_written << '\n';
}

bool SD::is_logging()
{
    return logfile_is_open;
}

void SD::StopLogging(){

    FRESULT fr;

    // Close file
    if(logfile_is_open)
    {
        fr = f_close(&fil);
        if (fr != FR_OK) {
            printf("ERROR: Could not close file (%d)\r\n", fr);
            return;
        }
        logfile_is_open = false;
    }

    if(Card.is_mounted())
    {
        fr = f_unmount("0:");
        if (fr != FR_OK) {
         printf("ERROR: Could not umount filesystem (%d)\r\n", fr);
         return;
        }
        else
        {   
            Card.set_unmounted();
            printf("SD card umounted successfully\r\n");
            Leds.USRled_off();
        }
    }    

}