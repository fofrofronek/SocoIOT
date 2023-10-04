#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <cstring>
#include <map>
#include "lwip_iot.h"
#include "ConfigParser.h"
using namespace std;

std::string ConfigLine;

LWIPconfig LWIPconf;

enum cfg_key {     o_DEFAULT, 
                   o_WIFI_SSID1,
                   o_WIFI_SSID2,
                   o_WIFI_PASSWORD1,
                   o_WIFI_PASSWORD2,
                   o_MQTT_SERVER_ADDR1,
                   o_MQTT_SERVER_ADDR2,
                   o_MQTT_SERVER_PORT1,
                   o_MQTT_SERVER_PORT2,
                   o_MQTT_BROKER_USERID1,
                   o_MQTT_BROKER_USERID2,
                   o_MQTT_BROKER_PASSWORD1,
                   o_MQTT_BROKER_PASSWORD2
                   };

//unknown string is mapped into first member

static std::map<std::string, cfg_key> cfg_options;

LWIPconfig::LWIPconfig()
{
    /*
    cfg_options["WIFI_SSID1"]=o_WIFI_SSID1;
    cfg_options["WIFI_SSID2"]=o_WIFI_SSID2;
    cfg_options["WIFI_PASSWORD1"]=o_WIFI_PASSWORD1;
    cfg_options["WIFI_PASSWORD2"]=o_WIFI_PASSWORD2;
    cfg_options["MQTT_SERVER_ADDR1"]=o_MQTT_SERVER_ADDR1;
    cfg_options["MQTT_SERVER_ADDR2"]=o_MQTT_SERVER_ADDR2;
    cfg_options["MQTT_SERVER_PORT1"]=o_MQTT_SERVER_PORT2;
    cfg_options["MQTT_SERVER_PORT2"]=o_MQTT_SERVER_PORT2;
    cfg_options["MQTT_BROKER_USERID1"]=o_MQTT_BROKER_USERID1;
    cfg_options["MQTT_BROKER_USERID2"]=o_MQTT_BROKER_USERID2;
    cfg_options["MQTT_BROKER_PASSWORD1"]=o_MQTT_BROKER_PASSWORD1;
    cfg_options["MQTT_BROKER_PASSWORD2"]=o_MQTT_BROKER_PASSWORD2;
    */
}

/*
void LWIPconfig::ParseCline(string line)
{
    

    line.erase(std::remove_if(line.begin(), line.end(), (int(*) (int)) std::isspace), line.end());

            if( line.empty() || line[0] == '#' )
            {
                return;
            }
            auto endPos = line.find("\n");
            auto delimiterPos = line.find("=");
            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1, endPos);
            //std::cout << "[" << name << "] [" << value << "]"<< '\n';
            std::cout << "[" << cfg_options[name] << "] ";
            switch(cfg_options[name])
            {
                case o_DEFAULT:    
                    std::cout << "--Fault[" << name << "] [" << value << "]"<< '\n';
                break;
                case o_WIFI_SSID1:
                    LWIPconf.WIFI_SSID1 = value;
                    std::cout << "--1[" << name << "] [" << LWIPconf.WIFI_SSID1 << "]"<< '\n';
                break;
               
                case o_WIFI_PASSWORD1:
                    LWIPconf.WIFI_PASSWORD1 = value;
                    std::cout << "--2[" << name << "] [" << LWIPconf.WIFI_PASSWORD1 << "]"<< '\n';
                break;

                case o_WIFI_SSID2:
                    LWIPconf.WIFI_SSID2 = value;
                    std::cout << "--3[" << name << "] [" << LWIPconf.WIFI_SSID2 << "]"<< '\n';
                break;

                case o_WIFI_PASSWORD2:
                    LWIPconf.WIFI_PASSWORD2 = value;
                    std::cout << "--4[" << name << "] [" << LWIPconf.WIFI_PASSWORD2 << "]"<< '\n';
                break;

                case o_MQTT_SERVER_ADDR1:
                    LWIPconf.MQTT_SERVER_ADDR1 = value;
                    std::cout << "--5[" << name << "] [" << LWIPconf.MQTT_SERVER_ADDR1 << "]"<< '\n';
                break;

                case o_MQTT_SERVER_ADDR2:
                    LWIPconf.MQTT_SERVER_ADDR2 = value;
                    std::cout << "--6[" << name << "] [" << LWIPconf.MQTT_SERVER_ADDR2 << "]"<< '\n';
                break;

                case o_MQTT_SERVER_PORT1:
                    LWIPconf.MQTT_SERVER_PORT1 = value;
                    std::cout << "--7[" << name << "] [" << LWIPconf.MQTT_SERVER_PORT1 << "]"<< '\n';
                break;

                case o_MQTT_SERVER_PORT2:
                    LWIPconf.MQTT_SERVER_PORT2 = value;
                    std::cout << "--8[" << name << "] [" << LWIPconf.MQTT_SERVER_PORT2 << "]"<< '\n';
                break;

                case o_MQTT_BROKER_USERID1:
                    LWIPconf.MQTT_BROKER_USERID1 = value;
                    std::cout << "--9[" << name << "] [" << LWIPconf.MQTT_BROKER_USERID1 << "]"<< '\n';
                break;

                case o_MQTT_BROKER_USERID2:
                    LWIPconf.MQTT_BROKER_USERID2 = value;
                    std::cout << "--10[" << name << "] [" << LWIPconf.MQTT_BROKER_USERID2 << "]"<< '\n';
                break;

                case o_MQTT_BROKER_PASSWORD1:
                    LWIPconf.MQTT_BROKER_PASSWORD1 = value;
                    std::cout << "--11[" << name << "] [" << LWIPconf.MQTT_BROKER_PASSWORD1 << "]"<< '\n';
                break;

                case o_MQTT_BROKER_PASSWORD2:
                    LWIPconf.MQTT_BROKER_PASSWORD2 = value;
                    std::cout << "--12[" << name << "] [" << LWIPconf.MQTT_BROKER_PASSWORD2 << "]"<< '\n';
                break;

                default:
                    std::cout << "Unknown value name string" << "[" << name << "] [" << value << "]"<< '\n';
                break;
            }


}
*/
//As above causes MCU hang at constructor, on PC it is working, need to investigate deeper

void LWIPconfig::ParseClineDumbVer(string line)
{
    

    line.erase(std::remove_if(line.begin(), line.end(), (int(*) (int)) std::isspace), line.end());

            if( line.empty() || line[0] == '#' )
            {
                return;
            }
            auto endPos = line.find("\n");
            auto delimiterPos = line.find("=");
            auto name = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1, endPos);
            //std::cout << "[" << name << "] [" << value << "]"<< '\n';
            
            
            
                 if(name == "WIFI_SSID1")
                 {
                    LWIPconf.WIFI_SSID1 = value;
                    std::cout << "--1[" << name << "] [" << LWIPconf.WIFI_SSID1 << "]"<< '\n';
                 }
                else if(name == "WIFI_PASSWORD1"){
                    LWIPconf.WIFI_PASSWORD1 = value;
                    std::cout << "--2[" << name << "] [" << LWIPconf.WIFI_PASSWORD1 << "]"<< '\n';
                }

                else if(name == "WIFI_SSID2"){
                    LWIPconf.WIFI_SSID2 = value;
                    std::cout << "--3[" << name << "] [" << LWIPconf.WIFI_SSID2 << "]"<< '\n';
                }

                else if(name == "WIFI_PASSWORD2"){
                    LWIPconf.WIFI_PASSWORD2 = value;
                    std::cout << "--4[" << name << "] [" << LWIPconf.WIFI_PASSWORD2 << "]"<< '\n';
                }

                else if(name == "MQTT_SERVER_ADDR1"){
                    LWIPconf.MQTT_SERVER_ADDR1 = value;
                    std::cout << "--5[" << name << "] [" << LWIPconf.MQTT_SERVER_ADDR1 << "]"<< '\n';
                }

                else if(name == "MQTT_SERVER_ADDR2"){
                    LWIPconf.MQTT_SERVER_ADDR2 = value;
                    std::cout << "--6[" << name << "] [" << LWIPconf.MQTT_SERVER_ADDR2 << "]"<< '\n';
                }

                else if(name == "MQTT_SERVER_PORT1"){
                    LWIPconf.MQTT_SERVER_PORT1 = value;
                    std::cout << "--7[" << name << "] [" << LWIPconf.MQTT_SERVER_PORT1 << "]"<< '\n';
                }

                else if(name == "MQTT_SERVER_PORT2"){
                    LWIPconf.MQTT_SERVER_PORT2 = value;
                    std::cout << "--8[" << name << "] [" << LWIPconf.MQTT_SERVER_PORT2 << "]"<< '\n';
                }

                else if(name == "MQTT_BROKER_USERID1"){
                    LWIPconf.MQTT_BROKER_USERID1 = value;
                    std::cout << "--9[" << name << "] [" << LWIPconf.MQTT_BROKER_USERID1 << "]"<< '\n';
                }

                else if(name == "MQTT_BROKER_USERID2"){
                    LWIPconf.MQTT_BROKER_USERID2 = value;
                    std::cout << "--10[" << name << "] [" << LWIPconf.MQTT_BROKER_USERID2 << "]"<< '\n';
                }

                else if(name == "MQTT_BROKER_PASSWORD1"){
                    LWIPconf.MQTT_BROKER_PASSWORD1 = value;
                    std::cout << "--11[" << name << "] [" << LWIPconf.MQTT_BROKER_PASSWORD1 << "]"<< '\n';
                }

                else if(name == "MQTT_BROKER_PASSWORD2"){
                    LWIPconf.MQTT_BROKER_PASSWORD2 = value;
                    std::cout << "--12[" << name << "] [" << LWIPconf.MQTT_BROKER_PASSWORD2 << "]"<< '\n';
                }
                else std::cout << "Unknown value name string" << "[" << name << "] [" << value << "]"<< '\n';
                
            


}

