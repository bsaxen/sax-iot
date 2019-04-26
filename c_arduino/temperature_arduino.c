//=============================================
// File.......: temperature_arduino.c
// Date.......: 2019-04-26
// Author.....: Benny Saxen
int sw_version = 1;
// Description: Signal from D5 pin, Arduino
// 4.7kOhm between signal and Vcc
//=============================================
// Configuration
//=============================================
//#include "lib.c"
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 5 // Pin for connecting one wire sensor
#define TEMPERATURE_PRECISION 12

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);
DeviceAddress device[10];
int nsensors = 0;
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
  co.conf_sw         = sw_version;
  co.conf_id         = "arduino_mega";
  co.conf_period     = 120;
  co.conf_wrap       = 999999;
  co.conf_feedback   = 1;

  co.conf_title      = "kvv32_indoor_outdoor";
  co.conf_tags       = "kvv32_indoor_outdoor";
  co.conf_desc       = "kvv32_indoor_outdoor";
  co.conf_platform   = "esp8266";

  co.conf_domain     = "iot.simuino.com";
  co.conf_server     = "gateway.php";

  lib_setup(&co, &da);
    
  SetUpTemperatureSensors();
  co.conf_sensors = nsensors;
}
//=============================================
void loop()
//=============================================
{
  String msg;
  float temps[10];
  int i;
  char order[10];
  char buf[100];

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
        //Serial.println(temperature);
      }
  }

  g_payload = "{";
  if (nsensors > 0)
  {
    for (int i=1;i<=nsensors;i++)
    {
        g_payload += "\"temp";
        g_payload += i;
        g_payload += "\":\"";
        g_payload += temps[i-1];
        g_payload += "\"";
        if(i < nsensors)g_payload += ",";
    }
  }
  else
  {
    g_payload +=  "\"no_temperatures\":\"0\""; 
  }
  g_payload += "}";

  lib_publishPayload(&co,&da,g_payload);
   msg = lib_loop(&co,&da);
  //Serial.println(msg);
}
//=============================================
// End of File
//=============================================
