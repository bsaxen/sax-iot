//=============================================
// File.......: temperature.c
// Date.......: 2019-03-15
// Author.....: Benny Saxen
int sw_version = 1;
// Description: Signal from D1 pin.
// 4.7kOhm between signal and Vcc
// Problem access port: sudo chmod 666 /dev/ttyUSB0
//=============================================
// Configuration
//=============================================
//#include "iotLib.c"

struct Configuration c1;
struct Data d1;

String stat_url = " ";
String dyn_url = " ";
String pay_url = " ";
String log_url = " ";
String payload = "{\"no_data\":\"0\"}";
String message = "nothing";
//=============================================

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 5 // Pin for connecting one wire sensor
#define TEMPERATURE_PRECISION 12

///. CUSTOM variables
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);
DeviceAddress device[10];
int nsensors = 0;
int counter = 0;
/// END OF - CUSTOM variables

//=============================================
void SetUpTemperatureSensors()
//=============================================
{

    pinMode(ONE_WIRE_BUS, INPUT);
    sensor.begin();
    nsensors = sensor.getDeviceCount();
    if(nsensors > 0)
    {
        for(int i=0;i<nsensors;i++)
        {
            sensor.getAddress(device[i], i);
            sensor.setResolution(device[i], TEMPERATURE_PRECISION);
        }
    }
}


//=============================================
void setup()
//=============================================
{
  c1.conf_sw         = sw_version;
  c1.conf_id         = "set_to_mac";
  c1.conf_period     = 10;
  c1.conf_wrap       = 999999;
  c1.conf_feedback   = 0;

  c1.conf_title      = "title";
  c1.conf_tags       = "tag";
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
  
  SetUpTemperatureSensors();
  c1.conf_sensors = nsensors;
  Serial.println(nsensors);
  
  lib_wifiBegin(&c1);
  
  d1.counter = 0;
  c1.conf_id = c1.conf_mac;
  
  stat_url = lib_buildUrlStatic(c1);
  String dont_care = lib_wifiConnectandSend(c1,d1, stat_url); 
  
  message = "Temperature_Client_Started";
  log_url = lib_buildUrlLog(c1,message);
  String msg = lib_wifiConnectandSend(c1,d1, log_url);
  Serial.println(msg);
}

//=============================================
void loop()
//=============================================
{
  float temps[10];
  int i;
  char order[10];
  char buf[100];

  
  ++d1.counter;
  d1.rssi = WiFi.RSSI();
    
  if (d1.counter > c1.conf_wrap) d1.counter = 1;

  //Retrieve one or more temperature values
  sensor.requestTemperatures();
  //Loop through results and publish
  for(int i=0;i<nsensors;i++)
  {
     //float temperature = sensor.getTempCByIndex(i);
      float temperature = sensor.getTempC(device[i]);
      if (temperature > -100) // filter out bad values , i.e. -127
      {
        temps[i] = temperature;
        Serial.println(temperature);
      }
  }

  
  dyn_url = lib_buildUrlDynamic(c1, d1);
  String msg = lib_wifiConnectandSend(c1, d1, dyn_url);

  payload = "{";
  if (nsensors > 0)
  {
    for (int i=1;i<=nsensors;i++)
    {
        payload += "\"temp";
        payload += i;
        payload += "\":\"";
        payload += temps[i-1];
        payload += "\"";
        if(i < nsensors)payload += ",";
    }
  }
  else
  {
    payload +=  "\"no_temperatures\":\"0\""; 
  }
  payload += "}";
  
  pay_url = lib_buildUrlPayload(c1, d1, payload);

  msg = lib_wifiConnectandSend(c1, d1, pay_url);
  
  delay(c1.conf_period*1000);
  

}
//=============================================
// End of File
//=============================================
