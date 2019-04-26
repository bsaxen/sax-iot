//=============================================
// File.......: lib.c
// Date.......: 2019-04-26
// Author.....: Benny Saxen
// Description:
int lib_version = 1;
//=============================================
#include <SPI.h>
#include <Ethernet.h>

struct Configuration
{
  String conf_id         = "x";
  unsigned long conf_period        = 10;
  unsigned long conf_wrap          = 999999;
  int conf_feedback      = 1;
  String conf_title      = "title";
  String conf_tags       = "tag1";
  String conf_desc       = "your_description";
  String conf_platform   = "arduino";
  
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
  unsigned long counter;
  int fail;
  int latency;
  String fail_msg;
  String ip;
};

struct Configuration co;
struct Data da;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
 
// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 177);
IPAddress myDns(192, 168, 1, 1);

// initialize the library instance:
EthernetClient client;
//=============================================

String g_payload = "{\"no_data\":\"0\"}";
String g_message = "nothing";

//=============================================
String lib_loop(struct Configuration *co,struct Data *da)
//=============================================
{
  String msg;
 
  ++da->counter;
 
  if (da->counter > co->conf_wrap) da->counter = 1;
  
  msg = lib_publishDynamic(co,da);
  Serial.println(msg); 
  
  if (da->counter%50 == 0)
  {
      lib_publishStatic(co,da);
  }

  unsigned long x = co->conf_period*1000;
  delay(x);
  byte res = Ethernet.maintain();
  if (res != 0)
  {
    Serial.print("Maintain result = ");
    Serial.println(res);
  }
  
  return msg;
}
//=============================================
String IpAddress2String(const IPAddress& ipAddress)
//=============================================
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ; 
}
//=============================================
void lib_setup(struct Configuration *co,struct Data *da)
//=============================================
{
  
  Serial.begin(9600);

  char ssid[100];
  char password[100];
  
  Serial.println("Initialize Ethernet with DHCP...");
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  

  da->counter = 0;
  co->conf_mac = "not_used";
  da->ip = IpAddress2String(Ethernet.localIP());
  //co->conf_id = co->conf_mac;
  String stat_url = lib_buildUrlStatic(co);
  String dont_care = lib_ethSend(co,da, stat_url);
}
//=============================================
String lib_publishStatic(struct Configuration *c2,struct Data *d2)
//=============================================
{
  String url,msg;
  url = lib_buildUrlStatic(&co);
  msg = lib_ethSend(&co,&da, url);
  return msg;
}
//=============================================
String lib_publishDynamic(struct Configuration *c2,struct Data *d2)
//=============================================
{
  String url,msg;
  url = lib_buildUrlDynamic(&co,&da);
  msg = lib_ethSend(&co,&da, url);
  return msg;
}
//=============================================
String lib_publishPayload(struct Configuration *c2,struct Data *d2,String payload)
//=============================================
{
  String url,msg;
  url = lib_buildUrlPayload(&co,&da, payload);
  msg = lib_ethSend(&co,&da, url);
  return msg;
}
//=============================================
String lib_publishLog(struct Configuration *c2,struct Data *d2,String message)
//=============================================
{
  String url,msg;
  url = lib_buildUrlLog(&co, message);
  msg = lib_ethSend(&co,&da, url);
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
  
  url += "\"ip";
  url += "\":\"";
  url += d2->ip;
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
 
  String url = '/'+ c2->conf_server;
 
  url += "?do=log";

  url += "&id=";
  url += c2->conf_id;
  
  url += "&log=";
  url += message;

  return url;
}  

//=============================================
String lib_ethSend(struct Configuration *co,struct Data *da, String cur_url)
//=============================================
{
  char server[80];
  
  co->conf_domain.toCharArray(server,80); 
  //Serial.println(server);
  unsigned long t1,t2;
  
  String sub = "-";
  Serial.println(cur_url);
  t1 = millis();
 if (client.connect(server, 80)) {
    //Serial.println("connecting...");
    // send the HTTP GET request:
    String temp1 = "GET " + cur_url + " HTTP/1.1\r\n" +
             "Host: " + co->conf_domain + "\r\n" +
             "Connection: close\r\n\r\n";

    //Serial.println(temp1);
    client.println(temp1);
  } 
  else 
  {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
  t2 = millis();
  da->latency = t2 - t1;
  // Read all the lines of the reply from server and print them to Serial
  delay(100);
  while (client.available()) {
    String action = client.readStringUntil('\r');
    //Serial.print(action);
    if (action.indexOf('[') == 1)
    {
      int b = action.indexOf(':')+1;
      int x = action.lastIndexOf(':');
      sub = action.substring(b,x);
    }
    // Do something based upon the action string
  }

  //Serial.println("closing connection");

  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();
  return sub;
}
