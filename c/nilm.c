//=============================================
// File.......: nilm.c
// Date.......: 2019-03-29
// Author.....: Benny Saxen
// Description: nilm - electricity
// Sensor pin.: D1 NodeMcu
int sw_version = 1;
//=============================================
// Configuration
//=============================================
//#include "gowLib.c"
//=============================================

const byte interrupt_pin = 5; // D1 NodeMcu
const byte led_pin       = 4; // D2 NodeMcu
int timeToCheckStatus    = 0;
unsigned long t1,t2,dt,ttemp;
float elpow               = 0.0;
int interrupt_counter     = 0;
int bounce_value          = 50; // minimum time between interrupts
//===============================================================
// Interrupt function for measuring the time between pulses and number of pulses
// Always stored in RAM
void ICACHE_RAM_ATTR measure(){
//===============================================================
    //digitalWrite(led_pin,HIGH);
    ttemp = t1;
    t2 = t1;
    t1 = millis();
    dt = t1 - t2;
    if (dt < bounce_value)
    {
        t1 = ttemp;
        digitalWrite(led_pin,LOW);
        return;
    }
    elpow = 3600.*1000.*1000./(co.conf_kwh_pulses*dt);
    interrupt_counter++;
    //digitalWrite(led_pin,LOW);
}
//===============================================================
void setup(){
//===============================================================
    co.conf_sw         = sw_version;
    co.conf_id         = "-";
    co.conf_period     = 5;
    co.conf_wrap       = 999999;
    co.conf_feedback   = 1;

    co.conf_title      = "kvv32_nilm";
    co.conf_tags       = "kvv32_nilm";
    co.conf_desc       = "kvv32_nilm";
    co.conf_platform   = "esp8266";
    
    co.conf_ssid_1     = "NABTON";
    co.conf_password_1 = "123";
    
    co.conf_ssid_2     = "bridge";
    co.conf_password_2 = "1234";
    
    co.conf_ssid_3     = "ASUS";
    co.conf_password_3 = "123";
    
    co.conf_domain     = "iot.simuino.com";
    co.conf_server     = "gateway.php";

    lib_setup(&co, &da);

    co.conf_kwh_pulses  = 1000; //1000 pulses/kWh
    
    bounce_value = 36000./co.conf_kwh_pulses; // based on max power = 100 000 Watt

    pinMode(interrupt_pin, INPUT_PULLUP);
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin,LOW);

    attachInterrupt(interrupt_pin, measure, FALLING);
}
//=============================================
void loop()
//=============================================
{
    String msg;
    msg = lib_loop(&co,&da);
    Serial.println(msg);
    
    payload = "{";
    
    payload += "\"elpow";
    payload += "\":\"";
    payload += elpow;
    payload += "\",";
    
    payload += "\"pulses";
    payload += "\":\"";
    payload += interrupt_counter;
    payload += "\"";
  
    payload += "}";
 
    pay_url    = lib_buildUrlPayload(&co,&da,payload);  
    msg = lib_wifiConnectandSend(&co,&da, pay_url);
    
    interrupt_counter = 0;

}
//=============================================
// End of File
//=============================================
