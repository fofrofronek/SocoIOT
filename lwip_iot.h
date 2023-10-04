#pragma once
#ifndef _LWIP_IOT_H_
#define _LWIP_IOT_H_


using namespace std;
#include <string>

void main_iot_core1();





class LWIPinfo{
    public:
    //Wireless Status:
        uint8_t hw_initialized;
        uint8_t AP1_ok;
        uint8_t AP2_ok;
        uint8_t init_client_instance;
        uint8_t init_broker_connection;
        uint32_t conn_timeouts;
        uint32_t publish_errors;
        //string current_AP;
        //string current_IP;
        //string broker_connected;            
};

extern volatile LWIPinfo LWIPstatus;

#endif