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
String pay_url = " ";
String log_url = " ";
String payload = "{\"no_data\":\"0\"}";
String message = "nothing";
//=============================================
void setup() {
//=============================================  
  c1.conf_id         = "set_to_mac";
  c1.conf_period     = 10;
  c1.conf_wrap       = 999999;
  c1.conf_feedback   = 1;

  c1.conf_title      = "title";
  c1.conf_tags       = "tag1";
  c1.conf_desc       = "your";
  c1.conf_platform   = "esp8266";
  c1.conf_ssid       = "bridge";
  c1.conf_password   = "sdfsdf";
  c1.conf_domain     = "iot.simuino.com";
  c1.conf_server     = "gateway.php";
  c1.conf_streamId   = "....................";
  c1.conf_privateKey = "....................";
  
  Serial.begin(9600);
  delay(100);
  
  lib_wifiBegin(&c1);
  
  d1.counter = 0;
  c1.conf_id = c1.conf_mac;
  stat_url = lib_buildUrlStatic(c1);
  Serial.println(stat_url);
  String dont_care = lib_wifiConnectandSend(c1,d1, stat_url);
}


//=============================================
void loop() {
//=============================================
  delay(c1.conf_period*1000);
  
  ++d1.counter;
  d1.rssi = WiFi.RSSI();
  if (d1.counter > c1.conf_wrap) d1.counter = 1;
  
  dyn_url    = lib_buildUrlDynamic(c1, d1);  
  Serial.println(dyn_url);
  String msg = lib_wifiConnectandSend(c1,d1, dyn_url);
  Serial.println(msg);
  delay(500);
  pay_url    = lib_buildUrlPayload(c1, d1,payload);  
  msg = lib_wifiConnectandSend(c1,d1, pay_url);
  Serial.println(msg);


  log_url = lib_buildUrlLog(c1,message);
  msg = lib_wifiConnectandSend(c1,d1, log_url);
  Serial.println(msg);
}
//=============================================
// End of File
//=============================================
