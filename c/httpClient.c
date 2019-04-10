//=============================================
// File.......: httpClient.c
// Date.......: 2019-04-10
// Author.....: Benny Saxen
// Description: http client
int sw_version = 2;
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

  lib_loop(&co,&da);
  
  if (da.counter%10 == 0)
  {
    lib_publishPayload(&co,&da,g_payload);
  }

  if (da.counter%50 == 0)
  {
    lib_publishDynamic(&co,&da);
  }
  
  if (da.counter%100 == 0)
  {
    lib_publishLog(&co,&da,g_message);
  }

  if (da.counter%200 == 0)
  {
    lib_publishStatic(&co,&da);
  }
}
//=============================================
// End of File
//=============================================
