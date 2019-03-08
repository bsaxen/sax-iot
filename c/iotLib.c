//=============================================
// File.......: iotLib.c
// Date.......: 2019-03-08
// Author.....: Benny Saxen
// Description:
//=============================================
#include <ESP8266WiFi.h>


struct Configuration
{
  String conf_id = "x";
  int conf_period  = 10;
  int conf_wrap    = 999999;
  int conf_feedback  = 1;
  String conf_tags = "tag1,tag2,tag3";
  String conf_desc = "your_description";
  String conf_platform = "esp8266";
  String conf_ssid       = "bridge";
  String conf_password   = "qweqwe";
  String conf_host       = "gow.qwe.com";
  String conf_streamId   = "....................";
  String conf_privateKey = "....................";
  int conf_em_pulses = 1000;
  String conf_mac = "void";
};

struct Data
{
  int counter;
  int rssi;
  int fail;
};

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
  //Serial.println(buf);
  result = msg.toInt();

  return result;
}
//=============================================
String lib_buildUrlStatic(struct Configuration c2)
//=============================================
{
  //===================================
  String url = c2_conf_host;
  //===================================
  url += "?do=static";

  url += "&id=";
  url += c2.conf_id;
  
  url += "&json=";
  url += "{";
  url += "\"desc" + "\":\"" + c2.conf_desc + "\",";
  url += "\"tags" + "\":\"" + c2.conf_tags + "\",";
  url += "\"ssid" + "\":\"" + c2.conf_ssid + "\",";
  url += "\"wrap" + "\":\"" + c2.conf_wrap + "\",";
  url += "\"feedback" + "\":\"" + c2.conf_feedback + "\",";
  url += "\"period" + "\":\""   + c2.conf_period + "\"";
  url += "}";

  return url;
}
//=============================================
String lib_buildUrlDynamic(struct Configuration c2,struct Data d2)
//=============================================
{
  //===================================
  String url = c2_conf_host;
  //===================================
  url += "?do=dynamic";

  url += "&id=";
  url += c2.conf_id;
  
  url += "&json=";
  url += "{";
  url += "\"counter" + "\":\"" + d2.counter + "\",";
  url += "\"fail" + "\":\"" + d2.fail + "\",";
  url += "\"rssi" + "\":\"" + d2.rssi + "\"";
  url += "}";
  
  return url;
}
//=============================================
String lib_buildUrlPayload(struct Configuration c2,struct Data d2, String payload)
//=============================================
{
  //===================================
  String url = c2_conf_host;
  //===================================
  url += "?do=payload";

  url += "&id=";
  url += c2.conf_id;
  
  url += "&json=" + payload;
  
  return url;
}
//=============================================
String lib_buildUrlLog(struct Configuration c2, char* message)
//=============================================
{
  //===================================
  String url = c2_conf_host;
  //===================================
  url += "?do=log";

  url += "&id=";
  url += c2.conf_id;
  
  url += "&log=";
  url += message;

  return url;
}  
//=============================================
void lib_wifiBegin(struct Configuration *c2)
//=============================================
{
  char ssid[100];
  char password[100];
  
  Serial.print("Connecting to ");
  Serial.println(c2->conf_ssid);

   c2->conf_ssid.toCharArray(ssid,100);
   c2->conf_password.toCharArray(password,100);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
   would try to act as both a client and an access-point and could cause
   network-issues with your other WiFi-devices on your WiFi-network. */
   WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, password);

   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
   }

   Serial.println("WiFi connected");
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());
   c2->conf_mac = WiFi.macAddress();
   c2->conf_id = c2->conf_mac;
   Serial.println(WiFi.macAddress());
}
//=============================================
String lib_wifiConnectandSend(struct Configuration c2,struct Data d2, String cur_url)
//=============================================
{
  String sub = "-";
  Serial.print("Requesting URL: ");
  Serial.println(cur_url);
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(c2.conf_host,httpPort)) {
    Serial.println("connection failed");
    d2.fail += 1;
    return sub;
  }

  // This will send the request to the server
  client.print(String("GET ") + cur_url + " HTTP/1.1\r\n" +
             "Host: " + c2.conf_host + "\r\n" +
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
