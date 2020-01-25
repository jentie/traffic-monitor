//
//   traffic-monitor - WLAN man-in-the-middle traffic monitor
//
//   jens, 20200125, intial proof of concept
//
//   MIT License
//   Copyright (c) 2020 Jens Tiemann
//
//   based on: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/RangeExtender-NAPT/RangeExtender-NAPT.ino
//   see also work of https://github.com/martin-ger
//
//   netdump from https://github.com/d-a-v/EspGoodies
//
//
//   tested with Lolin/Wemos D1 mini
//
//   usage: e.g. on win10 ubuntu shell (disabale flow controls)
//   bash> stty -F /dev/ttyS5  -crtscts -ixon
//   bash> cat /dev/ttyS5 | tee sample.txt
//   bash> cat /dev/ttyS5 | ./gethostbyaddr.py
//


// #if LWIP_FEATURES && !LWIP_IPV6        needed configuration


// include your local secrets ...
#include "credentials.h"

// ... or here
// wifi credentials, upstream (existing WLAN, role: STA) and downstream (new access point, role: AP)
//const char* up_wlan_ssid = "WLAN-NAME";
//const char* up_wlan_pass = "WLAN-PASSWORD";
//const char* down_wlan_ssid = "WLAN-NAME";
//const char* down_wlan_pass = "WLAN-PASSWORD";
//


#include <ESP8266WiFi.h>
#include <lwip/napt.h>
#include <lwip/dns.h>
#include <dhcpserver.h>

#include <NetDump.h>

/* size of the tables used for NAPT */
#define NAPT 1000
#define NAPT_PORT 10


//
//           LED layout
//
//               led0
//        led4
//   led3                led1
//            led2
//
// LED pins - wemos D5, D1, D2, D0, D6

const byte ledpin[5] = { 14, 5, 4, 16, 12};       // #LED --> #pin

int ledstate[5] = { 0, 0, 0, 0, 0};               // counter for each LED


byte decode[256];


void pre_decode() {                 // initial assignment for proof of concept - TODO
  for (int n = 0; n < 256; n++)
    decode[n] = 2;

  decode[172] = 1;   // google      // based on some samples...
  decode[74] = 1;
  decode[104] = 3;   // akamai
  decode[72] = 3;
  decode[34] = 4;    // amazon
  decode[35] = 4;
  decode[185] = 0;   // facebook
}


bool flag_d = 0;                    // decode packet headers IP / TCP
bool flag_a = 1;                    // decode address only
bool flag_h = 0;                    // dump IP packet header, decimal


void ledtest() {
  for (int count = 0; count < 25; count++) {      // 5 rounds LED flashing
    digitalWrite(ledpin[count % 5], LOW);
    delay(50);
    digitalWrite(ledpin[count % 5], HIGH);
  }
}


void dump(int netif_idx, const char* data, size_t len, int out, int success) {
  (void)success;

  //  Serial.print(out ? F("out ") : F(" in "));
  //  Serial.printf("%d ", netif_idx);
  //  if (netDump_is_IPv4(data)) {
  //    netDump(Serial, data, len);
  //    //netDumpHex(Serial, data, len);
  //  }
  //  else
  //    Serial.println();

  if ((netif_idx == 0) && netDump_is_IPv4(data) && out ) {       // if 0 - upstream, if 1 - downstream

    if (flag_d)
      netDump(Serial, data, len);

    if (flag_h) {
      for (int n = 14; n < 34; n++) {             // ip packet header
        Serial.print((byte)data[n]);
        Serial.print(" ");
      }
      Serial.println();
    }

    if (flag_a) {
      Serial.print(data[30], DEC);                // print IP destination address
      Serial.print(".");
      Serial.print(data[31], DEC);
      Serial.print(".");
      Serial.print(data[32], DEC);
      Serial.print(".");
      Serial.println(data[33], DEC);
    }

    int adr = data[30];
    byte led = decode[adr];

    ledstate[led] = 5;

  }
}


void setup() {
  Serial.begin(115200);
  Serial.printf("\n\nWeak WLAN Router\n");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  for (int n = 0; n < 5; n++)
    pinMode(ledpin[n], OUTPUT);
  for (int n = 0; n < 5; n++)                     // DEBUG: LEDs on
    digitalWrite(ledpin[n], LOW);

  Serial.printf("Heap on start: %d\n", ESP.getFreeHeap());

  phy_capture = dump;

  // first, connect to STA so we can get a proper local DNS server
  WiFi.mode(WIFI_STA);
  WiFi.begin(up_wlan_ssid, up_wlan_pass);         // TODO? public info display, increment password?
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.printf("\nSTA: %s (dns: %s / %s)\n",
                WiFi.localIP().toString().c_str(),
                WiFi.dnsIP(0).toString().c_str(),
                WiFi.dnsIP(1).toString().c_str());

  // give DNS servers to AP side
  dhcps_set_dns(0, WiFi.dnsIP(0));
  dhcps_set_dns(1, WiFi.dnsIP(1));

  WiFi.softAPConfig(                              // AP config
    IPAddress(192, 168, 192, 1),                  // private address range 192.168.x.y
    IPAddress(192, 168, 192, 1),                  // gateway
    IPAddress(255, 255, 255, 0));                 // netmask
  WiFi.softAP(down_wlan_ssid, down_wlan_pass);    // start AP

  Serial.printf("AP: %s\n", WiFi.softAPIP().toString().c_str());

  Serial.printf("Heap before: %d\n", ESP.getFreeHeap());
  err_t ret = ip_napt_init(NAPT, NAPT_PORT);
  Serial.printf("ip_napt_init: ret=%d (OK=%d), table (%d,%d)\n", (int)ERR_OK, NAPT, NAPT_PORT, (int)ret );
  if (ret == ERR_OK) {
    ret = ip_napt_enable_no(SOFTAP_IF, 1);
    Serial.printf("ip_napt_enable_no(SOFTAP_IF): ret=%d (OK=%d)\n", (int)ret, (int)ERR_OK);
    if (ret == ERR_OK) {
      Serial.printf("WiFi network '%s' with is now available via '%s'\n", down_wlan_ssid, up_wlan_ssid);
    }
  }
  Serial.printf("Heap after napt init: %d\n", ESP.getFreeHeap());
  if (ret != ERR_OK) {
    Serial.printf("NAPT initialization failed\n");
  }

  pre_decode();                           // TODO: helper, later external table construction

  for (int n = 0; n < 5; n++)             // DEBUG: LEDs off
    digitalWrite(ledpin[n], HIGH);

  ledtest();                              // DEBUG: show LED pattern
}


void loop() {
  byte actsta = WiFi.softAPgetStationNum();

  //  Serial.print("Stations connected: ");   // DEBUG
  //  Serial.println(actsta);

  if ( actsta == 1 )
    digitalWrite(LED_BUILTIN, LOW);       // only ONE traffic source, no traffic mix
  else
    digitalWrite(LED_BUILTIN, HIGH);

  for (byte led = 0; led < 6; led++) {    // show led pattern
    if (ledstate[led]) {                  // show led...
      ledstate[led]--;                    // ... for a number of periods
      digitalWrite(ledpin[led], LOW);
    }
    else
      digitalWrite(ledpin[led], HIGH);
  }

  if (Serial.available() > 0) {
    char flag = Serial.read();
    if (flag == 'a')                      // small letter: disable output
      flag_a = false;
    if (flag == 'd')
      flag_d = false;
    if (flag == 'h')
      flag_h = false;
    if (flag == 'A')                      // enable output
      flag_a = true;
    if (flag == 'D')
      flag_d = true;
    if (flag == 'H')
      flag_h = true;
  }

  delay(100);
}
