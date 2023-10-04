#include "hardware/structs/rosc.h"

#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "lwip/inet.h"

#include "lwip/altcp_tcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/apps/mqtt.h"

#include "lwip/apps/mqtt_priv.h"

#include "SuperSocoParser.h"
#include "lwip_iot.h"
#include "ConfigParser.h"




#define DEBUG_printf printf

#define WIFI_SSID2 LWIPconf.WIFI_SSID2.c_str() 
#define WIFI_PASSWORD2 LWIPconf.WIFI_PASSWORD2.c_str()

#define WIFI_SSID1 LWIPconf.WIFI_SSID1.c_str()
#define WIFI_PASSWORD1 LWIPconf.WIFI_PASSWORD1.c_str()



u16_t MQTT_SERVER_PORT;
char BROKER_USERID[16];
char BROKER_PASSWORD[16];
string SERVER_HOSTNAME;

#define MQTT_TLS 0 // needs to be 1 for AWS IoT. Also set published QoS to 0 or 1
//#define CRYPTO_MOSQUITTO_LOCAL
#define CRYPTO_AWS_IOT
#include "crypto_consts.h"

volatile LWIPinfo LWIPstatus;


#if MQTT_TLS
#ifdef CRYPTO_CERT
const char *cert = CRYPTO_CERT;
#endif
#ifdef CRYPTO_CA
const char *ca = CRYPTO_CA;
#endif
#ifdef CRYPTO_KEY
const char *key = CRYPTO_KEY;
#endif
#endif


ip4_addr SERVER_ADDR;

extern volatile uint8_t core1_wd_flag;


void convert_ipaddr(string addr)
{   
    SERVER_ADDR.addr = 0;

    if(inet_aton(addr.c_str(), &SERVER_ADDR))
    { //valid address

    }
    else
    { //invalid address, assume it is a hostname
      SERVER_HOSTNAME = addr;
    }

}




typedef struct MQTT_CLIENT_T_ {
    ip_addr_t remote_addr;
    mqtt_client_t *mqtt_client;
    u32_t received;
    u32_t counter;
    u32_t reconnect;
} MQTT_CLIENT_T;
 
err_t mqtt_test_connect(MQTT_CLIENT_T *state);

// Perform initialisation
static MQTT_CLIENT_T* mqtt_client_init(void) {
    MQTT_CLIENT_T *state = (MQTT_CLIENT_T*) calloc(1, sizeof(MQTT_CLIENT_T));
    if (!state) {
        DEBUG_printf("failed to allocate state\n");
        return NULL;
    }
    state->received = 0;
    return state;
}

void dns_found(const char *name, const ip_addr_t *ipaddr, void *callback_arg) {
    MQTT_CLIENT_T *state = (MQTT_CLIENT_T*)callback_arg;
    DEBUG_printf("DNS query finished with resolved addr of %s.\n", ip4addr_ntoa(ipaddr));
    state->remote_addr = *ipaddr;
}

void run_dns_lookup(MQTT_CLIENT_T *state) {
    DEBUG_printf("Running DNS query for %s.\n", SERVER_HOSTNAME);

    cyw43_arch_lwip_begin();
    err_t err = dns_gethostbyname(SERVER_HOSTNAME.c_str(), &(state->remote_addr), dns_found, state);
    cyw43_arch_lwip_end();

    if (err == ERR_ARG) {
        DEBUG_printf("failed to start DNS query\n");
        return;
    }

    if (err == ERR_OK) {
        DEBUG_printf("no lookup needed");
        return;
    }

    while (state->remote_addr.addr == 0) {
        cyw43_arch_poll();
        sleep_ms(1);
    }
}

u32_t data_in = 0;

u8_t buffer[1025];
u8_t data_len = 0;

static void mqtt_pub_start_cb(void *arg, const char *topic, u32_t tot_len) {
    DEBUG_printf("mqtt_pub_start_cb: topic %s\n", topic);

    if (tot_len > 1024) {
        DEBUG_printf("Message length exceeds buffer size, discarding");
    } else {
        data_in = tot_len;
        data_len = 0;
    }
}

static void mqtt_pub_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags) {
    if (data_in > 0) {
        data_in -= len;
        memcpy(&buffer[data_len], data, len);
        data_len += len;

        if (data_in == 0) {
            buffer[data_len] = 0;
            DEBUG_printf("Message received: %s\n", &buffer);
        }
    }
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status != 0) {
        DEBUG_printf("Error during connection: err %d.\n", status);
    } else {
        DEBUG_printf("MQTT connected.\n");
    }
}

void mqtt_pub_request_cb(void *arg, err_t err) {
    MQTT_CLIENT_T *state = (MQTT_CLIENT_T *)arg;
    DEBUG_printf("mqtt_pub_request_cb: err %d\n", err);
    state->received++;
}

void mqtt_sub_request_cb(void *arg, err_t err) {
    DEBUG_printf("mqtt_sub_request_cb: err %d\n", err);
}

err_t mqtt_test_publish(MQTT_CLIENT_T *state)
{
  char buffer[32];
  char topic[32];

  //sprintf(buffer, "{\"message\":\"hello from picow %d / %d\"}", state->received, state->counter);

  err_t err;
  u8_t qos = 0; /* 0 1 or 2, see MQTT specification.  AWS IoT does not support QoS 2 */
  u8_t retain = 0;

  static u8_t item = 0;

  switch(item)
  {
    case 0:
        snprintf(buffer,32,"%d", publish.SOC);
        snprintf(topic,32,"tc/soc");
        break;
    case 1:
        snprintf(buffer,32,"%d", publish.Voltage);
        snprintf(topic,32,"tc/voltage");
        break;
    case 2:
        snprintf(buffer,32,"%d", publish.Bcurrent);
        snprintf(topic,32,"tc/current");
        break;
    case 3:
        snprintf(buffer,32,"%d", publish.Charging);
        snprintf(topic,32,"tc/charging_state");
        break;
    case 4:
        snprintf(buffer,32,"%d", publish.BMStemp);
        snprintf(topic,32,"tc/bms_temp");
        break;
    case 5:
        snprintf(buffer,32,"%d", publish.Cycle);
        snprintf(topic,32,"tc/cycles");
        break;
    case 6:
        snprintf(buffer,32,"%d", publish.Fullcycle);
        snprintf(topic,32,"tc/fullcycles");
        break;
    case 7:
        snprintf(buffer,32,"%d", pktStat485.rcvd_chunks);
        snprintf(topic,32,"tc/485rcvd_pkts");
        break;        
    default:
        snprintf(buffer,32,"%d", publish.SOC);
        snprintf(topic,32,"tc/soc");
        break;
  }

  printf("publishing item %d Topic= %.6s msg= %.6s\n", item, topic, buffer);
  item++;
  if(item >= 8) item = 0;

  cyw43_arch_lwip_begin();
  
  
  err = mqtt_publish(state->mqtt_client, topic, buffer, strlen(buffer), qos, retain, mqtt_pub_request_cb, state);
  
  if(err != ERR_OK) {
    DEBUG_printf("Publish err: %d\n", err);
  }


  cyw43_arch_lwip_end();
  return err; 
  //return 0;
}

err_t mqtt_test_connect(MQTT_CLIENT_T *state) {
    struct mqtt_connect_client_info_t ci;
    err_t err;

    memset(&ci, 0, sizeof(ci));

    ci.client_id = "PicoBike";
    ci.client_user = BROKER_USERID;
    ci.client_pass = BROKER_PASSWORD;
    ci.keep_alive = 0;
    ci.will_topic = NULL;
    ci.will_msg = NULL;
    ci.will_retain = 0;
    ci.will_qos = 0;

    #if MQTT_TLS

    struct altcp_tls_config *tls_config;
  
    #if defined(CRYPTO_CA) && defined(CRYPTO_KEY) && defined(CRYPTO_CERT)
    DEBUG_printf("Setting up TLS with 2wayauth.\n");
    tls_config = altcp_tls_create_config_client_2wayauth(
        (const u8_t *)ca, 1 + strlen((const char *)ca),
        (const u8_t *)key, 1 + strlen((const char *)key),
        (const u8_t *)"", 0,
        (const u8_t *)cert, 1 + strlen((const char *)cert)
    );
    // set this here as its a niche case at the moment.
    // see mqtt-sni.patch for changes to support this.
    ci.server_name = MQTT_SERVER_HOST;
    #elif defined(CRYPTO_CERT)
    DEBUG_printf("Setting up TLS with cert.\n");
    tls_config = altcp_tls_create_config_client((const u8_t *) cert, 1 + strlen((const char *) cert));
    #endif

    if (tls_config == NULL) {
        DEBUG_printf("Failed to initialize config\n");
        return -1;
    }

    ci.tls_config = tls_config;
    #endif

    const struct mqtt_connect_client_info_t *client_info = &ci;

    err = mqtt_client_connect(state->mqtt_client, &(state->remote_addr), MQTT_SERVER_PORT, mqtt_connection_cb, state, client_info);
    
    if (err != ERR_OK) {
        DEBUG_printf("mqtt_connect return %d\n", err);
    }

    return err;
}

int16_t mqtt_run_test(MQTT_CLIENT_T *state) {
    uint32_t error_counter1 = 0;
    uint32_t error_counter2 = 0;
    LWIPstatus.init_client_instance = 0;
    LWIPstatus.init_broker_connection = 0;

    state->mqtt_client = mqtt_client_new();

    state->counter = 0;  

    if (state->mqtt_client == NULL) {
        DEBUG_printf("Failed to create new mqtt client\n");
        LWIPstatus.init_client_instance = 0;
        return 1;
    } 
    else
    {
        LWIPstatus.init_client_instance = 1;
    }
    // psa_crypto_init();
    if (mqtt_test_connect(state) == ERR_OK) {
        LWIPstatus.init_broker_connection = 1;
        absolute_time_t timeout = nil_time;
        bool subscribed = false;
        mqtt_set_inpub_callback(state->mqtt_client, mqtt_pub_start_cb, mqtt_pub_data_cb, 0);

        while (true) {
            core1_wd_flag = 0xFF;
            cyw43_arch_poll();
            absolute_time_t now = get_absolute_time();
            if (is_nil_time(timeout) || absolute_time_diff_us(now, timeout) <= 0) {
                if (mqtt_client_is_connected(state->mqtt_client)) {       
                    
                    error_counter1 = 0;
             
                    cyw43_arch_lwip_begin();

                    if (!subscribed) {
                        mqtt_sub_unsub(state->mqtt_client, "tc/recv", 0, mqtt_sub_request_cb, 0, 1);
                        subscribed = true;
                    }
                    
                    if (mqtt_test_publish(state) == ERR_OK) {
                        error_counter2 = 0;
                        if (state->counter != 0) {
                            DEBUG_printf("published %d\n", state->counter);
                        }
                        timeout = make_timeout_time_ms(5000);
                        state->counter++;
                    } // else ringbuffer is full and we need to wait for messages to flush.
                    else
                    {
                        timeout = make_timeout_time_ms(5000);
                        error_counter2++;
                    }
                    
                    cyw43_arch_lwip_end();
                } else { 
                     DEBUG_printf("Not connected\n");
                    error_counter1++;
                }
            }

            LWIPstatus.conn_timeouts = error_counter1;
            LWIPstatus.publish_errors = error_counter2;

            if(error_counter1 > 65535) return 3;
            if(error_counter2 > 10) return 4;

        }
    }
    return 2;
}

void error_handler(uint8_t error_number)
{   
    switch(error_number)
    {
        case 0: //unused
              break;
        case 1: // cyw43_arch init error
                break;
        case 2: //AP1 connection fail
                break;
        case 3: //AP2 connection fail
                break;
        case 4: //MQTT error;
                break;
        default:
                break;
        
    }
    

    core1_wd_flag = 0xFF;
    busy_wait_us(5*1000*1000); //5s
    if(error_number > 1)
    {
        cyw43_arch_deinit();
    }
}

void main_iot_core1() {

    uint8_t AP_no = 0; 

respawn:
    core1_wd_flag = 0xFF;

    LWIPstatus.hw_initialized = 0;
    LWIPstatus.AP1_ok = 0;
    LWIPstatus.AP2_ok = 0;

    if (cyw43_arch_init()) {
        DEBUG_printf("failed to initialise\n");
        LWIPstatus.hw_initialized = 0;
        error_handler(1);
        goto respawn;
    }
    LWIPstatus.hw_initialized = 1;

    //cyw43_arch_gpio_put(1,1);
    
    cyw43_arch_enable_sta_mode();

    DEBUG_printf("Connecting to WiFi... %d\n", AP_no);

    switch(AP_no)
    {
        case 0:
                    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID1, WIFI_PASSWORD1, CYW43_AUTH_WPA2_AES_PSK, 6000) == 0)
                    {
                      LWIPstatus.AP1_ok = 1;
                      DEBUG_printf("Connected to AP1.\n");
                    }
                    else
                    {
                      LWIPstatus.AP1_ok = 0;
                      DEBUG_printf("failed to  connect WiFi AP1.\n");
                      error_handler(2);
                      AP_no = 1;
                      goto respawn;
                    } 
                break;
        default:
                    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID2, WIFI_PASSWORD2, CYW43_AUTH_WPA2_AES_PSK, 6000) == 0) 
                    {
                      LWIPstatus.AP2_ok = 1;
                      DEBUG_printf("Connected to AP2.\n");
                    }
                    else
                    {
                      LWIPstatus.AP2_ok = 0;
                      DEBUG_printf("failed to  connect WiFi AP2.\n");
                      error_handler(3);
                      AP_no = 0;
                      goto respawn;
                    }
                break;


    }

    core1_wd_flag = 0xFF;

    MQTT_CLIENT_T *state = mqtt_client_init();

    core1_wd_flag = 0xFF;
     
    //run_dns_lookup(state);
    //IP4_ADDR( & (state->remote_addr), MQTT_SERVER_IP0, MQTT_SERVER_IP1, MQTT_SERVER_IP2, MQTT_SERVER_IP3);

    if(LWIPstatus.AP1_ok)
    {
        convert_ipaddr(LWIPconf.MQTT_SERVER_ADDR1);
        
        if(SERVER_ADDR.addr == 0) run_dns_lookup(state);
        else state->remote_addr = SERVER_ADDR;
        
        MQTT_SERVER_PORT = atoi(LWIPconf.MQTT_SERVER_PORT1.c_str());
        
        strncpy(BROKER_USERID,LWIPconf.MQTT_BROKER_USERID1.c_str(),sizeof(BROKER_USERID));
        strncpy(BROKER_PASSWORD,LWIPconf.MQTT_BROKER_PASSWORD1.c_str(),sizeof(BROKER_PASSWORD));         
    }
    else if(LWIPstatus.AP2_ok)
    {
        convert_ipaddr(LWIPconf.MQTT_SERVER_ADDR2);
        
        if(SERVER_ADDR.addr == 0) run_dns_lookup(state);
        else state->remote_addr = SERVER_ADDR;

        MQTT_SERVER_PORT = atoi(LWIPconf.MQTT_SERVER_PORT2.c_str());
        
        strncpy(BROKER_USERID,LWIPconf.MQTT_BROKER_USERID2.c_str(),sizeof(BROKER_USERID));
        strncpy(BROKER_PASSWORD,LWIPconf.MQTT_BROKER_PASSWORD2.c_str(),sizeof(BROKER_PASSWORD));

    }
    else goto respawn;
    


    if(mqtt_run_test(state))
    {
        error_handler(4);
        goto respawn;
    }
    
    core1_wd_flag = 0xFF;

    cyw43_arch_deinit();
    goto respawn;

}