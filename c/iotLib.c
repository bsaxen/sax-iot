//=============================================
// File.......: iotLib.c
// Date.......: 2019-03-29
// Author.....: Benny Saxen
// Description:
int lib_version = 3;
//=============================================
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;


struct Configuration
{
  String conf_id         = "x";
  int conf_period        = 10;
  int conf_wrap          = 999999;
  int conf_feedback      = 1;
  String conf_title      = "title";
  String conf_tags       = "tag1";
  String conf_desc       = "your_description";
  String conf_platform   = "esp8266";
  
  String conf_ssid_1     = "bridge";
  String conf_password_1 = "qweqwe";
  
  String conf_ssid_2     = "bridge";
  String conf_password_2 = "qweqwe";
  
  String conf_ssid_3     = "bridge";
  String conf_password_3 = "qweqwe";
  
  String conf_domain     = "iot.simuino.com";
  String conf_server     = "gateway.php";
 
  int conf_kwh_pulses    = 0;
  int conf_sensors       = 0;
  String conf_mac        = "void";
  int conf_library       = lib_version;
  int conf_sw            = 0;
};

struct Data
{
  int counter;
  int rssi;
  int fail;
  String ssid;
};

struct Configuration co;
struct Data da;
//=============================================
String stat_url = " ";
String dyn_url = " ";
String pay_url = " ";
String log_url = " ";
String payload = "{\"no_data\":\"0\"}";
String message = "nothing";

//=============================================
int lib_setup(struct Configuration *co,struct Data *da)
//=============================================
{
  Serial.begin(9600);

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  } 

  WiFi.mode(WIFI_STA);

  char ssid[100];
  char password[100];
  
  Serial.print("Connecting to ");
  Serial.println(co->conf_ssid_1);
  Serial.println(co->conf_ssid_2);
  Serial.println(co->conf_ssid_3);
  
  co->conf_ssid_1.toCharArray(ssid,100);
  co->conf_password_1.toCharArray(password,100);
  WiFiMulti.addAP(ssid, password);

  co->conf_ssid_2.toCharArray(ssid,100);
  co->conf_password_2.toCharArray(password,100);
  WiFiMulti.addAP(ssid, password);

  co->conf_ssid_3.toCharArray(ssid,100);
  co->conf_password_3.toCharArray(password,100);
  WiFiMulti.addAP(ssid, password);
  
  da->counter = 0;
  co->conf_mac = WiFi.macAddress();
  da->ssid = WiFi.SSID();
  co->conf_id = co->conf_mac;
  String stat_url = lib_buildUrlStatic(co);
  String dont_care = lib_wifiConnectandSend(co,da, stat_url);
}
//=============================================
int lib_decode_ON_OFF(String msg)
//=============================================
{
  char buf[100];
  int result = 0;

  msg.toCharArray(buf,100);

  if( strstr(buf,"OFF") != NULL)
  {
      result = 1;
  }

  if( strstr(buf,"ONN") != NULL)
  {
      result = 2;
  }
  return result;
}
//=============================================
int lib_decode_STEPPER(String msg)
//=============================================
{
  char buf[100];
  int result = 0;

  //msg.toCharArray(buf,100);
  Serial.print(">>>>>>>>>>>");
  Serial.println(msg);
  result = msg.toInt();

  return result;
}
//=============================================
String lib_buildUrlStatic(struct Configuration *c2)
//=============================================
{
  //===================================
  String url = '/'+ c2->conf_server;
  //===================================
  url += "?do=static";

  url += "&id=";
  url += c2->conf_id;
  
  url += "&json=";
  url += "{";
  
  url += "\"title";
  url += "\":\"";
  url += c2->conf_title;
  url += "\",";
  
  url += "\"desc";
  url += "\":\"";
  url += c2->conf_desc;
  url += "\",";
  
  url += "\"tags";
  url += "\":\"";
  url += c2->conf_tags;
  url += "\",";
  
  url += "\"wrap";
  url += "\":\"";
  url += c2->conf_wrap;
  url += "\",";
  
  if (c2->conf_sensors != 0)
  {
    url += "\"sensors";
    url += "\":\"";
    url += c2->conf_sensors;
    url += "\",";
  }
  
  if (c2->conf_kwh_pulses != 0)
  {
    url += "\"kwh_pulses";
    url += "\":\"";
    url += c2->conf_kwh_pulses;
    url += "\",";
  }  
  
  url += "\"feedback";
  url += "\":\"";
  url += c2->conf_feedback;
  url += "\",";
  
  url += "\"library";
  url += "\":\"";
  url += c2->conf_library;
  url += "\",";
  
  url += "\"sw";
  url += "\":\"";
  url += c2->conf_sw;
  url += "\",";
  
  url += "\"period";
  url += "\":\"";
  url += c2->conf_period;
  url += "\"";
  
  url += "}";

  return url;
}
//=============================================
String lib_buildUrlDynamic(struct Configuration *c2,struct Data *d2)
//=============================================
{
  //===================================
  String url = '/'+ c2->conf_server;
  //===================================
  url += "?do=dynamic";

  url += "&id=";
  url += c2->conf_id;
  
  url += "&json=";
  url += "{";
  url += "\"counter";
  url += "\":\"";
  url += d2->counter;
  url += "\",";
  url += "\"fail";
  url += "\":\"";
  url += d2->fail;
  url += "\",";
  url += "\"ssid";
  url += "\":\"";
  url += d2->ssid;
  url += "\",";
  url += "\"rssi";
  url += "\":\"";
  url += d2->rssi;
  url += "\"";
  url += "}";
  
  return url;
}
//=============================================
String lib_buildUrlPayload(struct Configuration *c2,struct Data *d2, String payload)
//=============================================
{
  //===================================
  String url = '/'+ c2->conf_server;
  //===================================
  url += "?do=payload";

  url += "&id=";
  url += c2->conf_id;
  
  url += "&json=" + payload;
  
  return url;
}
//=============================================
String lib_buildUrlLog(struct Configuration *c2, String message)
//=============================================
{
  //===================================
  String url = '/'+ c2->conf_server;
  //===================================
  url += "?do=log";

  url += "&id=";
  url += c2->conf_id;
  
  url += "&log=";
  url += message;

  return url;
}  

//=============================================
String lib_wifiConnectandSend(struct Configuration *co,struct Data *da, String cur_url)
//=============================================
{
  String sub = "-";
  String url = "http://"+co->conf_domain+cur_url;
  
  Serial.println(url);
  WiFiClient client;
  HTTPClient http;

  da->rssi = WiFi.RSSI();

  if ((WiFiMulti.run() == WL_CONNECTED)) {
    //Serial.print("[HTTP] begin...\n");
    delay(100);
    if (http.begin(client, url)) {  // HTTP
      int httpCode = http.GET();
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
          sub = payload;
        }
      } else {
        da->fail += 1;
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    } else {
      da->fail += 1;
      Serial.printf("[HTTP} Unable to connect\n");
    }     
  }
  //Serial.println("closing connection");
  return sub;
}
