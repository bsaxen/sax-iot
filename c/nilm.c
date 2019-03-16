//=============================================
// File.......: nilm.c
// Date.......: 2019-03-16
// Author.....: Benny Saxen
// Description: nilm - electricity
// Sensor pin.: D1 NodeMcu
int sw_version = 1;
//=============================================
// Configuration
//=============================================
//#include "gowLib.c"

struct Configuration c1;
struct Data d1;
tring stat_url = " ";
String dyn_url = " ";
String pay_url = " ";
String log_url = " ";
String payload = "{\"no_data\":\"0\"}";
String message = "nothing";
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
    elpow = 3600.*1000.*1000./(c1.conf_em_pulses*dt);
    interrupt_counter++;
    //digitalWrite(led_pin,LOW);
}
//===============================================================
void setup(){
//===============================================================
    c1.conf_sw         = sw_version;
    c1.conf_id         = "set_to_mac";
    c1.conf_period     = 10;
    c1.conf_wrap       = 999999;
    c1.conf_feedback   = 1;

    c1.conf_title      = "title";
    c1.conf_tags       = "tag1";
    c1.conf_desc       = "your";
    c1.conf_platform   = "esp8266";
    c1.conf_ssid       = "bridge";
    c1.conf_password   = "sdfsdf";
    c1.conf_domain     = "iot.simuino.com";
    c1.conf_server     = "gateway.php";
    c1.conf_streamId   = "....................";
    c1.conf_privateKey = "....................";
    c1.conf_em_pulses  = 1000; //1000 pulses/kWh
    
    bounce_value = 36000./c1.conf_em_pulses; // based on max power = 100 000 Watt

    pinMode(interrupt_pin, INPUT_PULLUP);
    pinMode(led_pin, OUTPUT);
    digitalWrite(led_pin,LOW);

    lib_wifiBegin(&c1);
  
    d1.counter = 0;
    c1.conf_id = c1.conf_mac;
    stat_url = lib_buildUrlStatic(c1);
    Serial.println(stat_url);
    String dont_care = lib_wifiConnectandSend(c1,d1, stat_url);

    attachInterrupt(interrupt_pin, measure, FALLING);
}
//=============================================
void loop()
//=============================================
{
    delay(c1.conf_period*1000);
    ++d1.counter;
    if (d1.counter > c1.conf_wrap) d1.counter = 1;
    d1.rssi = WiFi.RSSI();
 
    dyn_url = lib_buildUrlDynamic(c1, d1);
    String msg = lib_wifiConnectandSend(c1, d1, dyn_url);
    
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
 
    pay_url = lib_buildUrlPayload(c1, d1, payload);
    
    msg = lib_wifiConnectandSend(c1,d1, pay_url);
    
    interrupt_counter = 0;

}
//=============================================
// End of File
//=============================================
