//=============================================
// File.......: switch.c
// Date.......: 2019-02-26
// Author.....: Benny Saxen
// Description:
//=============================================
// Configuration
//=============================================
//#include "iotLib.c"

struct Configuration c1;
struct Data d1;
String stat_url;
String dyn_url;
//=============================================

#define FAN_PIN 5  // D1 pin on NodeMCU 1.0
int g_status = 1;

//=============================================
void setup() {
//=============================================
  c1.conf_topic = "nytomta/gow/qwe/0";
  c1.conf_period  = 60;
  c1.conf_wrap    = 999999;
  c1.conf_action  = 2;

  c1.conf_tags = "tag1,tag2,tag3";
  c1.conf_desc = "floor_fan";
  c1.conf_platform = "esp8266";
  c1.conf_ssid       = "ASUS-hus";
  c1.conf_password   = "qweqwe";
  c1.conf_host       = "gow.qwe.com";
  c1.conf_streamId   = "....................";
  c1.conf_privateKey = "....................";

  d1.fail = 0;
  Serial.begin(9600);
  delay(100);

  lib_wifiBegin(c1);

  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN,HIGH);
  delay(2000);
  digitalWrite(FAN_PIN,LOW);
  delay(2000);
  digitalWrite(FAN_PIN,HIGH);
  g_status = 1;

  stat_url = lib_buildUrlStatic(c1);
  String dont_care = lib_wifiConnectandSend(c1,d1, stat_url);
}

//=============================================
void loop()
//=============================================
{
  ++d1.counter;
  if (d1.counter > c1.conf_wrap) d1.counter = 1;
  d1.rssi = WiFi.RSSI();
  dyn_url = lib_buildUrlDynamic(c1, d1);
  dyn_url += "&payload=";
  dyn_url += "{";
  dyn_url += "\"status\":\"";
  dyn_url += g_status;
  dyn_url += "\"}";
  String msg = lib_wifiConnectandSend(c1,d1, dyn_url);
  int res = lib_decode_ON_OFF(msg);
  if (res == 1) 
  {
    digitalWrite(FAN_PIN,HIGH);
    g_status = res;
  }
  if (res == 2) 
  {
    digitalWrite(FAN_PIN,LOW);
    g_status = res;
  }
  delay(c1.conf_period*1000);
}
//=============================================
// End of File
//=============================================
