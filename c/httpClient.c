//=============================================
// File.......: httpClient.c
// Date.......: 2019-03-29
// Author.....: Benny Saxen
// Description: http client
int sw_version = 1;
//=============================================
// Configuration
//=============================================
//#include "iotLib.c"
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

  co.conf_domain     = "iot.simuino.com";
  co.conf_server     = "gateway.php";
  
  co.conf_ssid_1     = "bridge";
  co.conf_password_1 = "dfgdfg";
  
  co.conf_ssid_2     = "bridge";
  co.conf_password_2 = "dfgdfg";
  
  co.conf_ssid_3     = "bridge";
  co.conf_password_3 = "dfgdfg";

  lib_setup(&co, &da);
  
}
//=============================================
void loop() {
//=============================================
  String msg;
  delay(co.conf_period*1000);
  
  ++da.counter;
  da.rssi = WiFi.RSSI();
  if (da.counter > co.conf_wrap) da.counter = 1;
  
  dyn_url    = lib_buildUrlDynamic(&co, &da);  
  msg = lib_wifiConnectandSend(&co,&da, dyn_url);
  Serial.println(msg);
  
  if (da.counter%10 == 0)
  { 
    pay_url    = lib_buildUrlPayload(&co,&da,payload);  
    msg = lib_wifiConnectandSend(&co,&da, pay_url);
    Serial.println(msg);
  }

  if (da.counter%50 == 0)
  {
    log_url = lib_buildUrlLog(&co,message);
    msg = lib_wifiConnectandSend(&co,&da, log_url);
    Serial.println(msg);
  }

}
//=============================================
// End of File
//=============================================
