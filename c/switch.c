//=============================================
// File.......: switch.c
// Date.......: 2019-02-26
// Author.....: Benny Saxen
// Description:
int sw_version = 1;
//=============================================
// Configuration
//=============================================
//#include "iotLib.c"
//=============================================

#define SWITCH_PIN 5  // D1 pin on NodeMCU 1.0
int g_status = 1;

//=============================================
void setup() {
//=============================================
  co.conf_sw         = sw_version;
  co.conf_id         = "set_to_mac";
  co.conf_period     = 60;
  co.conf_wrap       = 999999;
  co.conf_feedback   = 1;

  co.conf_title      = "test1";
  co.conf_tags       = "test1";
  co.conf_desc       = "test1";
  co.conf_platform   = "esp8266";
  co.conf_ssid       = "bridge";
  co.conf_password   = "dfgdfg";
  co.conf_domain     = "iot.simuino.com";
  co.conf_server     = "gateway.php";
  co.conf_streamId   = "....................";
  co.conf_privateKey = "....................";

  lib_setup(&co, &da);
  
  pinMode(SWITCH_PIN, OUTPUT);
  digitalWrite(SWITCH_PIN,HIGH);
  delay(2000);
  digitalWrite(SWITCH_PIN,LOW);
  delay(2000);
  digitalWrite(SWITCH_PIN,HIGH);
  g_status = 1;
}

//=============================================
void loop()
//=============================================
{
  
  String msg,message;
  delay(co.conf_period*1000);
  
  ++da.counter;
  da.rssi = WiFi.RSSI();
  if (da.counter > co.conf_wrap) da.counter = 1;
  
  dyn_url    = lib_buildUrlDynamic(&co, &da);  
  msg = lib_wifiConnectandSend(&co,&da, dyn_url);
  Serial.println(msg);
  int res = lib_decode_ON_OFF(msg);
  
  g_status = res;
  int event = 0;
  if (res == 1) 
  {
    digitalWrite(SWITCH_PIN,HIGH);
    event = 1;
    message = "Switch is set to HIGH";
  }
  if (res == 2) 
  {
    digitalWrite(SWITCH_PIN,LOW);
    event = 2;
    message = "Switch is set to LOW";
  }
  
  if (event != 0)
  { 
    payload = "{";
    payload += "\"status\":\"";
    payload += g_status;
    payload += "\"";
    payload += "}";
    pay_url    = lib_buildUrlPayload(&co,&da,payload);  
    msg = lib_wifiConnectandSend(&co,&da, pay_url);
    Serial.println(msg);
    
    log_url = lib_buildUrlLog(&co,message);
    msg = lib_wifiConnectandSend(&co,&da, log_url);
    Serial.println(msg);
  }

}
//=============================================
// End of File
//=============================================
