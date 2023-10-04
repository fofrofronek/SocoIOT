#pragma once
#ifndef _SUPERSOCOPARSER_H_
#define _SUPERSOCOPARSER_H_

#include <vector>
using namespace std;

void ParseChunk(vector<unsigned char> rawData);

class pktStat{
    public:
    pktStat();
    uint32_t rcvd_total;
    uint32_t rcvd_chunks;
    uint32_t rcvd_BMS;
    uint32_t rcvd_ECU;
    uint32_t rcvd_GSMGPS;
    uint32_t rcvd_UNKNOWN;
};

extern pktStat pktStat485;

class SSinfo{
    public:
    //Battery Status:
        unsigned char Voltage;
        unsigned char SOC;
        unsigned char BMStemp;
        signed char Bcurrent;
        unsigned int Cycle;
        unsigned int Fullcycle;
        unsigned char Charging;
        unsigned char Breaker;
                
    //ECU Status:
        unsigned char Mode;
        unsigned int Ecurrent;
        unsigned int Speed;
        signed char ECUtemp;
        unsigned char Parkingmode;
            
};

extern volatile SSinfo publish;

#endif