#include "pico/stdlib.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <stdint.h>
//#include<windows.h>
#include "SuperSocoParser.h"


//using namespace std;

    const uint16_t MAX_DATAGRAM_LENGTH = 64;
    const uint8_t  DATAGRAM_HEADER_LEN = 5;
    const uint8_t  DATAGRAM_TAIL_LEN = 2;
    const uint8_t  DATAGRAM_ENVELOPE_LEN = DATAGRAM_HEADER_LEN + DATAGRAM_TAIL_LEN;

    const uint8_t READ_FIRST_BYTE = 0xB6;
    const uint8_t READ_SECOND_BYTE = 0x6B;

    const uint8_t WRITE_FIRST_BYTE = 0xC5;
    const uint8_t WRITE_SECOND_BYTE = 0x5C;



pktStat::pktStat()
{
    rcvd_total=0;
    rcvd_chunks=0;
    rcvd_BMS=0;
    rcvd_ECU=0;
    rcvd_GSMGPS=0;
    rcvd_UNKNOWN=0;
}


 enum States
    {
        NO_BLOCK,
        FIRST_BYTE,
        READING_BLOCK
    };

States state = NO_BLOCK;


 enum DatagramType
    {
        REQUEST,
        RESPONSE,
        UNKNOWN
    };

/// data buffer of the currently reading telegram
/// </summary>
//private byte[] data = new byte[MAX_DATAGRAM_LENGTH];
vector<uint8_t> sdata(MAX_DATAGRAM_LENGTH);

volatile SSinfo publish;

uint32_t offset = 0;

const uint8_t END_TELEGRAM = 0x0D;
const uint8_t POS_TYPE1 = 0;
const uint8_t POS_TYPE2 = 1;
const uint8_t POS_SRC = 2;
const uint8_t POS_DES = 3;
const uint8_t POS_LEN = 4;


struct {
    //uint8_t Type;
    uint8_t Source;
    uint8_t Destination;
    uint8_t Length;
    DatagramType Type;
} ParsedDatagram;

void ParseDatagram()
{
    
    printf("Parsing datagram length of %d\n", offset);

    uint8_t pos_last = sdata[POS_LEN] + DATAGRAM_ENVELOPE_LEN - 1;
    uint8_t iterator;
    uint8_t last_data;

        if( ! (((sdata[POS_TYPE1] == 0xC5) && (sdata[POS_TYPE2] == 0x5C)) || ((sdata[POS_TYPE1] == 0xB6) && (sdata[POS_TYPE2] == 0x6B))))
        {
            printf("Error unknown datagram type\n");
            return;
        }

        if(sdata[POS_LEN] + POS_LEN >= MAX_DATAGRAM_LENGTH) 
        {
          printf("Error data end beyond range\n");
          return;
        }
        
        if(sdata[pos_last] != END_TELEGRAM)
        {
          printf("Error datagram tail not found\n");
          //printf("%d\n", pos_last);
          return;
        }

        last_data = sdata[POS_LEN] + POS_LEN;

        uint8_t calcCheck = sdata[POS_LEN];
        
         for(iterator = POS_LEN + 1; iterator <= last_data; iterator++)
         {
            calcCheck ^= sdata[iterator];
            //printf("%x : %x\n", iterator, sdata[iterator]);
         }

        //printf("CS calculated %x\n", calcCheck);
        //printf("CS read %x\n", sdata[pos_last - 1]);

        if(calcCheck != sdata[pos_last - 1])
        {
            printf("Invalid checksum\n");
            return;
        }

        for(auto element : sdata)
        {
           // printf("%.2X ", element);
        }
        //printf("\n");

        ParsedDatagram.Length = sdata[POS_LEN];
        ParsedDatagram.Source = sdata[POS_SRC];
        ParsedDatagram.Destination = sdata[POS_DES];

        if((sdata[POS_TYPE1] == 0xC5) && (sdata[POS_TYPE2] == 0x5C))
        {
            ParsedDatagram.Type = REQUEST;
        }
        else if((sdata[POS_TYPE1] == 0xB6) && (sdata[POS_TYPE2] == 0x6B))
        {
            ParsedDatagram.Type = RESPONSE;
        }
        else
        {
            ParsedDatagram.Type = UNKNOWN;
        }

        if(ParsedDatagram.Type == RESPONSE)
        {   
            pktStat485.rcvd_total++;

            if(ParsedDatagram.Source == 0xAA && ParsedDatagram.Destination == 0x5A && ParsedDatagram.Length == 10)
            {
                printf("Battery Status:\n");
                
                printf("Voltage: %d ,",sdata[POS_LEN+1]);
                publish.Voltage = sdata[POS_LEN+1];

                printf("SOC: %d ,",sdata[POS_LEN+2]);
                publish.SOC = sdata[POS_LEN+2];

                printf("BMS Temp: %d ,",sdata[POS_LEN+3]);
                publish.BMStemp = sdata[POS_LEN+3];

                printf("Current: %d ,",sdata[POS_LEN+4]);
                publish.Bcurrent = (signed char)(sdata[POS_LEN+4]);
                
                printf("Cycle: %d ,", (uint16_t)( (sdata[POS_LEN+5] << 8) + sdata[POS_LEN+6]));
                publish.Cycle = (uint16_t)( (sdata[POS_LEN+5] << 8) + sdata[POS_LEN+6]);

                printf("FullCycle: %d ,", (uint16_t)( (sdata[POS_LEN+7] << 8) + sdata[POS_LEN+8]));
                publish.Fullcycle = (uint16_t)( (sdata[POS_LEN+7] << 8) + sdata[POS_LEN+8]);

                printf("Breaker state: %d ,",sdata[POS_LEN+9]);
                publish.Breaker = sdata[POS_LEN+9];
                
                printf("Charging: %d ,",sdata[POS_LEN+10]);
                publish.Charging = sdata[POS_LEN+10];
                
                printf("\n");

                pktStat485.rcvd_BMS++;

            }
            else if(ParsedDatagram.Source == 0xAA && ParsedDatagram.Destination == 0xDA && ParsedDatagram.Length == 10)
            {
                printf("ECU Status:\n");
                printf("Mode: %d ,",sdata[POS_LEN+1]);
                publish.Mode = sdata[POS_LEN+1];
                
                printf("Current: %d ,", (uint16_t)( (sdata[POS_LEN+2] << 8) + sdata[POS_LEN+3]));
                publish.Ecurrent = (uint16_t)(((sdata[POS_LEN+2] << 8) + sdata[POS_LEN+3]));
                
                printf("Speed: %d ,", (uint16_t)( (sdata[POS_LEN+4] << 8) + sdata[POS_LEN+5]));
                publish.Speed = (uint16_t)( (sdata[POS_LEN+4] << 8) + sdata[POS_LEN+5]);

                printf("ECU Temp: %d ,",sdata[POS_LEN+6]);
                publish.ECUtemp = sdata[POS_LEN+6];
                
                printf("Parking mode: %d ,",sdata[POS_LEN+9]);
                publish.Parkingmode = sdata[POS_LEN+9];

                printf("\n");
                pktStat485.rcvd_ECU++;
            }
            else
            {
                printf("Batt or ECU Status too long: %d\n", ParsedDatagram.Length);
            }
        }
        else if(ParsedDatagram.Type == REQUEST)
        {
            pktStat485.rcvd_total++;

            if(ParsedDatagram.Source == 0xBA && ParsedDatagram.Destination == 0xAA && ParsedDatagram.Length == 14)
            {
                //printf("GSM Status of len = %d\n", ParsedDatagram.Length);
                printf("GSM Status:\n");
                pktStat485.rcvd_GSMGPS++;
            }
            else 
            {
                //printf("GSM Status cond unmatch:\n");
            }
        }
        else 
        {
            pktStat485.rcvd_total++;

            printf("Unknown dg type:\n");
        }
        
        

 fill(sdata.begin(), sdata.end(), 0);
 offset = 0;
}

void ParseStreamed(uint8_t byte)
    {   
        static volatile uint32_t expected_datagram_length = MAX_DATAGRAM_LENGTH;
            switch (state)
            {
                case NO_BLOCK:
                    //printf("State.NO_BLOCK %d\n", offset);
                    if (byte == READ_FIRST_BYTE || byte == WRITE_FIRST_BYTE)
                    {
                        sdata[offset++] = byte;
                        state = FIRST_BYTE;
                    }
                    break;

                case FIRST_BYTE:
                    //printf("State.FIRST_BYTE %d\n", offset);
                    
                    if (byte == READ_SECOND_BYTE || byte == WRITE_SECOND_BYTE) //we got expected second byte
                    {   
                        
                        sdata[offset++] = byte;
                        state = READING_BLOCK;
                    }
                    else //we got something else as second byte
                    {
                        //reset SM and buffer to initial
                        state = NO_BLOCK;
                        offset = 0;
                        expected_datagram_length = MAX_DATAGRAM_LENGTH;
                        fill(sdata.begin(), sdata.end(), 0);

                    }
                    break;

                case READING_BLOCK:
                    //printf("State.READING_BLOCK %d\n", offset);
                    
                    if (offset == POS_LEN) 
                    {
                        //printf("expectd datagram length %d \n", byte + DATAGRAM_ENVELOPE_LEN);
                        (byte + DATAGRAM_ENVELOPE_LEN) >= MAX_DATAGRAM_LENGTH ? expected_datagram_length = MAX_DATAGRAM_LENGTH : expected_datagram_length = byte + DATAGRAM_ENVELOPE_LEN; 
                    }
                    sdata[offset++] = byte;
                    if (offset >= expected_datagram_length) //we've probably received a full datagram 
                    {
                        ParseDatagram();
                        //reset SM and buffer to initial
                        state = NO_BLOCK;
                        offset = 0;
                        expected_datagram_length = MAX_DATAGRAM_LENGTH;
                        fill(sdata.begin(), sdata.end(), 0);
                    }
                    break;

                default:
                    //log.Error($"Unknown state {state}");
                        //printf("State unknown %d\n", offset);
                        //reset SM and buffer to initial
                        state = NO_BLOCK;
                        offset = 0;
                        expected_datagram_length = MAX_DATAGRAM_LENGTH;
                        fill(sdata.begin(), sdata.end(), 0);
                    break;
            }
        
    }




void ParseChunk(vector<uint8_t> rawData)
    {
        uint32_t idx = 0;
        cout << "Parsing chunk" << endl;
        for(auto b: rawData)
        {
            ParseStreamed(b);
            idx++;
           // if (idx==256) break;
        }
    }

