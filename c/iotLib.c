//=============================================
// File.......: iotLib.c
// Date.......: 2019-03-08
// Author.....: Benny Saxen
// Description:
//=============================================
#include <ESP8266WiFi.h>


struct Configuration
{
  String conf_topic = "benny/saxen/esp";
  int conf_period  = 10;
  int conf_wrap    = 999999;
  int conf_action  = 1;
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
  String stat_url = "/gowServer.php";
  //===================================
  stat_url += "?do=stat";

  stat_url += "&topic=";
  stat_url += c2.conf_topic;

  stat_url += "&mac=";
  stat_url += c2.conf_mac;

  stat_url += "&ssid=";
  stat_url += c2.conf_ssid;

  stat_url += "&wrap=";
  stat_url += c2.conf_wrap;

  stat_url += "&period=";
  stat_url += c2.conf_period;

  stat_url += "&domain=";
  stat_url += c2.conf_host;

  stat_url += "&platform=";
  stat_url += c2.conf_platform;

  stat_url += "&tags=";
  stat_url += c2.conf_tags;

  stat_url += "&desc=";
  stat_url += c2.conf_desc;

  stat_url += "&action=";
  stat_url += c2.conf_action;

  return stat_url;
}
//=============================================
String lib_buildUrlDynamic(struct Configuration c2,struct Data d2)
//=============================================
{
  //===================================
  String dyn_url = "/gowServer.php";
  //===================================
  dyn_url += "?do=dyn";

  dyn_url += "&topic=";
  dyn_url += c2.conf_topic;

  dyn_url += "&counter=";
  dyn_url += d2.counter;

  dyn_url += "&rssi=";
  dyn_url += d2.rssi;

  dyn_url += "&fail=";
  dyn_url += d2.fail;
  
  /*dyn_url += "&payload=";
  dyn_url += "{";
  dyn_url += "\"temp";
  dyn_url += "\":\"";
  dyn_url += 123;
  dyn_url += "\"";
  dyn_url += "}";*/
  return dyn_url;
}
//=============================================
String lib_buildUrlLog(struct Configuration c2, char* message)
//=============================================
{
  //===================================
  String url = "/gowServer.php";
  //===================================
  url += "?do=log";

  url += "&topic=";
  url += c2.conf_topic;
  
  url += "&dev_ts=";
  url += 'void';
  
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
