#include "ff.h"
#include <string>
using namespace std;

class SD{
    private:
    bool initialized;
    bool mounted;
    string logfile;
    bool logfile_is_open;
    FATFS fs;
    FIL fil;
    DIR dir;
    FILINFO filnfo;
    public:
    SD();
    void Init();
    bool is_initialized();
    bool is_mounted();
    void set_mounted();
    void set_unmounted();
    void StartLogging();
    void DoLogging(char* buffer, uint16_t count);
    bool is_logging();
    void StopLogging();
};

class Config{
    private:
    
    FRESULT fr;
    FATFS fs;
    FIL fil;
    DIR dir;
    int ret;
    char buf[100];
    const char* filename = "SocoIOT.cfg";

    public:
    Config();
    

};

class DataRecord{
    private:
    public:
};