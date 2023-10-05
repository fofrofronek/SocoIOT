# SocoIOT
This project have been created during few rainy days in the summer of 2023 debugged into working state during few further days in following five weeks still containing some imperfections.

There are some known bugs like speed and power isn't parsed correctly which are under investigation.<br />
Also temperatures below zero can bring us surprise readings :-)<br />
Bluetooth isn't implemented yet.<br />
Get working uart dma with lwip.<br />
MQTT encryption for using public cloud services isn't tested yet.<br />
An absolute minimum of configurable parameters can be configured.<br />

Working means it reads TC Pro and hope also TC Max internal bus data, parsing, connecting to WiFi and sending unencrypted data over local network to local MQTT broker (Mosquitto) plugin for Home Assistant.

It can send following MQTT topic:<br />
<br />
Battery voltage "tc/voltage"<br />
Battery current "tc/current"<br />
Battery charging state "tc/charging_state"<br />
BMS temperature "tc/bms_temp"<br />
Battery # chargings "tc/cycles"<br />
Battery full cycles "tc/fullcycles"<br />
A "heartbeat" growing # of received 485 chunks "tc/485rcvd_pkts"<br />
<br />
<img src="https://github.com/fofrofronek/SocoIOT/blob/main/hardware/hassio.jpg" width="640px" height="auto">
<br />
## Necessary hardware:
An open source gamepad Picopad Wifi based on Raspberry Pico W<br />
Sold as assembly kit<br />
<br />
https://pajenicko.cz/picopad/picopad-wifi-open-source-herni-konzole<br />
<br />
https://github.com/Pajenicko/Picopad<br />
<br />
<img src="https://github.com/fofrofronek/SocoIOT/blob/main/hardware/Screen.jpg" width="640px" height="auto">


### RS485 to uart interface module:
https://www.laskakit.cz/prevodnik-uart-na-rs-485--max485/<br />
This module is tricky having pinout marked by MAX485,<br />
thus correct wiring is RX<->RX and GND, no need to connect TX as we only listen on the 485 bus<br />
On the bus side RS485 A<->A and B<->B shall be wired.<br />

<img src="https://github.com/fofrofronek/SocoIOT/blob/main/hardware/RS485.jpg" width="640px" height="auto">

Suggesting to use 3 or 4 pin connector of same style placed inside battery compartment for easy connect & disconnect Picopad<br />

Uart wiring made by commonly used Molex connectors.<br />
There are more options:<br />
1. Use the Picopad supplied 12 pin female extension connector, then cabling is sticking out quite far from the side of picopad.<br />
2. Do not populate 12 pin female but solder commonly available 100 mil male pins instead, just RX and GND making connection less prone to damage.<br />
3. a handy individuals can combine two above as I did.<br />

For more mechanical stability use melting glue to secure connectors to cables.<br />
Secure module to the picopad with MacGyver tape or better kapton tape :-)<br />

<img src="https://github.com/fofrofronek/SocoIOT/blob/main/hardware/485Picopad.jpg" width="640px" height="auto">

<br />
<br />

### Connectors JST SM male female 4 pin:
https://www.laskakit.cz/laskakit-sada-jst-sm-konektoru-40ks/<br />
compatible with those used on TC Max<br />

<img src="https://github.com/fofrofronek/SocoIOT/blob/main/hardware/jstsm.jpg" width="640px" height="auto">

A non-invasive tapping of RS485 under seat:<br />

<img src="https://github.com/fofrofronek/SocoIOT/blob/main/hardware/485Underseat.jpg" width="640px" height="auto">

TC Max underseat connector wiring:<br />

<img src="https://github.com/fofrofronek/SocoIOT/blob/main/hardware/Underseat485Con.png" width="640px" height="auto">

## Quickstart guide:
Prepare an micro SD card with config file SocoIOT.cfg in root folder with following content modified by your needs.<br />

\# use hashtag for comments if needed

WIFI_SSID1 = SSID1<br />
WIFI_PASSWORD1 = pass1<br />
MQTT_SERVER_ADDR1 = 192.168.1.1<br />
MQTT_SERVER_PORT1 = 1883<br />
MQTT_BROKER_USERID1 = mqttuser1<br />
MQTT_BROKER_PASSWORD1 = passwd1<br />

WIFI_SSID2 = SSID2<br />
WIFI_PASSWORD2 = PASS2<br />
MQTT_SERVER_ADDR2 = 192.168.1.2<br />
MQTT_SERVER_PORT2 = 1883<br />
MQTT_BROKER_USERID2 = mqttuser2<br />
MQTT_BROKER_PASSWORD2 = passwd2<br />


### Uploading the firmware via usb - same way as USB Pico:<br />
1. Press and keep pressed "Bootsel" button located at the bottom side of Picopad<br />
2. Connect Picopad to the USB (or if already connected press "Reset" button)<br />
3. Wait till Pico gets connected as mass storage device, new drive should appear.<br />
4. copy SocoIOT.uf2 binary file to the new drive.<br />
5. after copying it will reboot.<br />
6. insert SD card with config file into Picopad and reboot. 

### Basic operation:<br />
Buttons "Up" and "Down" are switching between different screens - Battery, ECU, Wireless, Debug.<br />
Button "a" turns-off screen and lock keyboard.<br />
Unlocking is done by pressing and holding "Right" and pressing "a" during holding.<br />
Button "X" starts writing RS485 raw data to the SD card, second press stops the operation.<br />
Recording the RS485 data is indicated by USR led. Also SD led is blinking at SD access activity.<br />
Recording function is in initial state and is not fully tested yet.<br />
Full and chg leds are related to battery state and are not influenced by firmware.  <br />
<br />
<br />
<br />
This project is composed from many publicly available sources, so "All rights belong to their respective owners" etc.<br />
<br />
Let's mention some of them:<br />
<br />
Andreas Kurzmann Soco RS 485 protocol description and parser <br />
https://github.com/stprograms/SuperSoco485Monitor<br />
Rewritten from C# to C++ and state machine fixed<br />
<br />
Arm developer Ecosystem's display driver<br />
st7789-library-for-pico<br />
https://github.com/ArmDeveloperEcosystem<br />
used with necessary mods ny hardware<br />
<br />
Craig Niles MQTT client for Pico W<br />
https://github.com/cniles/picow-iot<br />
modified for my purposesa<br />
<br />
SD card FatFS library from Carl J Kugler III and ELM ChaN<br />
https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico<br />
used as is<br />
<br />
Hardware - Picopad open source hardware<br />
https://github.com/Pajenicko/Picopad<br />
<br />
