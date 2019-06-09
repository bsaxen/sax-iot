//=============================================
// File.......: freezer.c
// Date.......: 2019-05-12
// Author.....: Benny Saxen
int sw_version = 1;
// Temperature Signal from D1 pin (GPIO 05).
// Switch control pin D2 pin (GPIO 04)
// 4.7kOhm between signal and Vcc
// Problem access port: sudo chmod 666 /dev/ttyUSB0
// Set target temperature with temp + 100, or temp - 100
//=============================================
// Configuration
//=============================================
//#include "iotLib.c"
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 5 // Pin for connecting one wire sensor
#define SWITCH_PIN 4 // Control relay switch
#define TEMPERATURE_PRECISION 12

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);
DeviceAddress device[10];
int nsensors = 0;
int g_status = 0; // Compressor OFF
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
  co.conf_id         = "set_to_mac";
  co.conf_period     = 20;
  co.conf_wrap       = 999999;
  co.conf_feedback   = 1;

  co.conf_title      = "freezer";
  co.conf_tags       = "freezer";
  co.conf_desc       = "freezer";
  co.conf_platform   = "esp8266";

  co.conf_domain     = "iot.simuino.com";
  co.conf_server     = "gateway.php";
  
  strcpy(co.conf_ssid_1,"bridge");
  strcpy(co.conf_password_1,"6301166614");
  
  //co.conf_ssid_2     = "bridge";
  //co.conf_password_2 = "6301166614";
  
  //co.conf_ssid_3     = "bridge";
  //co.conf_password_3 = "6301166614";

  co.conf_target_temp = -15;
  co.conf_deviation_temp = 5;

  lib_setup(&co, &da);
    
  SetUpTemperatureSensors();
  co.conf_sensors = nsensors;
  pinMode(SWITCH_PIN, OUTPUT);
  digitalWrite(SWITCH_PIN,LOW);
  delay(5000);
  digitalWrite(SWITCH_PIN,HIGH);
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
  int  current_temperature;
    
    
  msg = lib_loop(&co,&da);
  Serial.println(msg);

  if(!msg.equals("[0]"))
  {
    int res = lib_decode_FREEZER(msg);
    if(res < 0 )res = res + 100;  
    if(res > 0 )res = res - 100;  
    Serial.print("target temp");Serial.println(res);
    if (res != 0) 
    {
      co.conf_target_temp = res;
      lib_publishStatic(&co,&da);
      g_message = "New_target_temperature";
      lib_publishLog(&co,&da,g_message);
    }
  }
    
  sensor.requestTemperatures();
 
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
  // Act upon temperature value

current_temperature = temps[0];

switch (g_status)
{
  case 0:
    Serial.println("Compressor is OFF");
    if (current_temperature > co.conf_target_temp + co.conf_deviation_temp)
    {
      Serial.println("Freezer to warm  - turn on compressor");
      digitalWrite(SWITCH_PIN,LOW);
      g_status = 1;
    }
    break;

  case 1:
    Serial.println("Compressor is ON");
    if (current_temperature <= co.conf_target_temp)
    {
      Serial.println("Freezer temperature ok  - turn off compressor");
      digitalWrite(SWITCH_PIN,HIGH);
      g_status = 0;
    }  
    break; 

  default:
  Serial.println("Compressor is UNKNOWN");
    Serial.println("Default Switch - shall not happen!");
    digitalWrite(SWITCH_PIN,HIGH);
    g_status = 0;
    break;   
}

//========================================
// Send 

  g_payload = "{";

  if (nsensors > 0)
  {
    for (int i=1;i<=nsensors;i++)
    {
        g_payload += "\"temp";
        g_payload += i;
        g_payload += "\":\"";
        g_payload += temps[i-1];
        g_payload += "\",";
    }
  }
  g_payload += "\"target\":\"";
  g_payload += co.conf_target_temp;
  g_payload += "\",";

  g_payload += "\"status\":\"";
  g_payload += g_status;
  g_payload += "\"";
  
  g_payload += "}";
  
  lib_publishPayload(&co,&da,g_payload);
}
//=============================================
// End of File
//=============================================
