#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <cstring>
#include <map>
using namespace std;
//vector<unsigned char> ConfigLine;
extern std::string ConfigLine;


class LWIPconfig{
        public:
        LWIPconfig();
        string WIFI_SSID1;
        string WIFI_PASSWORD1;
        string WIFI_SSID2;
        string WIFI_PASSWORD2;
        string MQTT_SERVER_ADDR1;
        string MQTT_SERVER_ADDR2;
        string MQTT_SERVER_PORT1;
        string MQTT_SERVER_PORT2;
        string MQTT_BROKER_USERID1;
        string MQTT_BROKER_USERID2;
        string MQTT_BROKER_PASSWORD1;
        string MQTT_BROKER_PASSWORD2;
        //void ParseCline(vector<unsigned char> line);
        //void ParseCline(string line);
        void ParseClineDumbVer(string line);
};

extern LWIPconfig LWIPconf;

