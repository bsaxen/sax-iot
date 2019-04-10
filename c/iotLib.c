//=============================================
// File.......: iotLib.c
// Date.......: 2019-04-10
// Author.....: Benny Saxen
// Description:
int lib_version = 3;
//=============================================

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

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
  int latency;
  String fail_msg;
  String ssid;
};

struct Configuration co;
struct Data da;
//=============================================

String g_payload = "{\"no_data\":\"0\"}";
String g_message = "nothing";

//=============================================
String lib_loop(struct Configuration *co,struct Data *da)
//=============================================
{
  String msg;
  delay(co->conf_period*1000);
  
  ++da->counter;
  da->rssi = WiFi.RSSI();
  if (da->counter > co->conf_wrap) da->counter = 1;
  
  String url = lib_buildUrlDynamic(co, da);  
  msg = lib_wifiConnectandSend(co,da, url);
  //Serial.println(msg); 
  
  if (da->counter%50 == 0)
  {
    url = lib_buildUrlStatic(co);
    String dont_care = lib_wifiConnectandSend(co,da,url);
  }
  
  return msg;
}
//=============================================
void lib_setup(struct Configuration *co,struct Data *da)
//=============================================
{
  Serial.begin(9600);

  for (uint8_t t = 3; t > 0; t--) {
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
String lib_publishStatic(struct Configuration *c2,struct Data *d2)
//=============================================
{
  String url,msg;
  url = lib_buildUrlStatic(&co);
  msg = lib_wifiConnectandSend(&co,&da, url);
  return msg;
}
//=============================================
String lib_publishDynamic(struct Configuration *c2,struct Data *d2)
//=============================================
{
  String url,msg;
  url = lib_buildUrlDynamic(&co,&da);
  msg = lib_wifiConnectandSend(&co,&da, url);
  return msg;
}
//=============================================
String lib_publishPayload(struct Configuration *c2,struct Data *d2,String payload)
//=============================================
{
  String url,msg;
  url = lib_buildUrlPayload(&co,&da, payload);
  msg = lib_wifiConnectandSend(&co,&da, url);
  return msg;
}
//=============================================
String lib_publishLog(struct Configuration *c2,struct Data *d2,String message)
//=============================================
{
  String url,msg;
  url = lib_buildUrlLog(&co, message);
  msg = lib_wifiConnectandSend(&co,&da, url);
  return msg;
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
  Serial.print("Decode Stepper:");
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
  if (d2->fail > 0)
  {
    url += "\"fail_msg";
    url += "\":\"";
    url += d2->fail_msg;
    url += "\",";
  }
  url += "\"latency";
  url += "\":\"";
  url += d2->latency;
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
  unsigned long t1,t2;
  
  String sub = "-";
  Serial.print("Requesting URL: ");
  Serial.println(cur_url);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(co->conf_domain,httpPort)) {
    Serial.println("connection failed");
    da->fail += 1;
    return sub;
  }

  // This will send the request to the server
  t1 = millis();
  client.print(String("GET ") + cur_url + " HTTP/1.1\r\n" +
             "Host: " + co->conf_domain + "\r\n" +
             "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
     if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return sub;
     }
     delay(5);
  }
  t2 = millis();
  da->latency = t2 - t1;
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String action = client.readStringUntil('\r');
    Serial.print(action);
    if (action.indexOf('[') == 1)
    {
      int b = action.indexOf(':')+1;
      int x = action.lastIndexOf(':');
      sub = action.substring(b,x);
    }
    // Do something based upon the action string
  }

  Serial.println("closing connection");
  return sub;
}
