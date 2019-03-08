//=============================================
// File.......: httpClient.c
// Date.......: 2019-03-08
// Author.....: Benny Saxen
// Description: http client
//=============================================
// Configuration
//=============================================
//#include "iotLib.c"

struct Configuration c1;
struct Data d1;
//=============================================
String stat_url = " ";
String dyn_url = " ";
//=============================================
void setup() {
//=============================================  
  c1.conf_topic = "ret/rum/0";
  c1.conf_period  = 10;
  c1.conf_wrap    = 999999;
  c1.conf_action  = 1;

  c1.conf_tags = "tag1,tag2,tag3";
  c1.conf_desc = "your_description";
  c1.conf_platform = "esp8266";
  c1.conf_ssid       = "ertyerty";
  c1.conf_password   = "etyrt";
  c1.conf_host       = "gow.simuino.com";
  c1.conf_streamId   = "....................";
  c1.conf_privateKey = "....................";
  Serial.begin(9600);
  delay(100);
  lib_wifiBegin(&c1);
  d1.counter = 0;
  stat_url = lib_buildUrlStatic(c1);
  String dont_care = lib_wifiConnectandSend(c1,d1, stat_url);
}


//=============================================
void loop() {
//=============================================
  delay(c1.conf_period*1000);
  ++d1.counter;
  d1.rssi = WiFi.RSSI();
  if (d1.counter > c1.conf_wrap) d1.counter = 1;
  dyn_url = lib_buildUrlDynamic(c1, d1);
  String msg = lib_wifiConnectandSend(c1,d1, dyn_url);
}
//=============================================
// End of File
//=============================================
