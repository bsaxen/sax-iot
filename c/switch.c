//=============================================
// File.......: switch.c
// Date.......: 2019-05-12
// Author.....: Benny Saxen
// Description:
int sw_version = 2;
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
  
  String msg;
  
  msg = lib_loop(&co,&da);
  Serial.println(msg);

  int res = lib_decode_ON_OFF(msg);
  
  g_status = res;
  int event = 0;
  if (res == 1) 
  {
    digitalWrite(SWITCH_PIN,HIGH);
    event = 1;
    g_message = "Switch is set to HIGH";
  }
  if (res == 2) 
  {
    digitalWrite(SWITCH_PIN,LOW);
    event = 2;
    g_message = "Switch is set to LOW";
  }
  
  if (event != 0)
  { 
    g_payload = "{";
    g_payload += "\"status\":\"";
    g_payload += g_status;
    g_payload += "\"";
    g_payload += "}";
    
    lib_publishPayload(&co,&da,g_payload);
   
    lib_publishLog(&co,&da,g_message);
  }

}
//=============================================
// End of File
//=============================================
