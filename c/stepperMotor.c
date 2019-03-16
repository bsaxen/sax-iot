//=============================================
// File.......: stepperMotor.c
// Date.......: 2019-03-16
int sw_version = 1;
// Author.....: Benny Saxen
// Description:
// Message Api:
// move,dir,step-size,steps,delay  example: 1,1,40,5
// reset                      reset position to zero
// reboot
// period,x                   set period to x seconds
//=============================================
// Configuration
//=============================================
#include "iotLib.c"

struct Configuration c1;
struct Data d1;
//=============================================
String stat_url = " ";
String dyn_url = " ";
String pay_url = " ";
String log_url = " ";
String payload = "{\"no_data\":\"0\"}";
String message = "nothing";

//================================================
// Globals
//================================================
int current_pos = 0;
int FULL_STEP = 1;
int HALF_STEP = 2;
int QUARTER_STEP = 3;
int CLOCKWISE = 1;
int COUNTER_CLOCKWISE = 2;

int DIR   = 4;   // D2
int STEP  = 5;   // D1
int SLEEP = 12;  // D6
int MS1   = 13;  // D7
int MS2   = 14;  // D5
int SW    = 15;  // D8

int dir = 0;
int step_size = FULL_STEP;
int number_of_step = 0;
int delay_between_steps = 5

//================================================
int stepCW(int steps,int dd)
//================================================
{
  int i;
  digitalWrite(DIR, LOW);
  digitalWrite(SLEEP, HIGH); // Set the Sleep mode to AWAKE.
  for(i=0;i<=steps;i++)
    {
      delayMicroseconds(200);
      digitalWrite(STEP, HIGH);
      delay(dd);
      digitalWrite(STEP, LOW);
      delay(dd);
      if (digitalRead(SW) == HIGH) return 2;
    }
  digitalWrite(DIR, LOW);
  digitalWrite(SLEEP, LOW); // Set the Sleep mode to SLEEP.Serial.println
  return 1;
}

//================================================
int stepCCW(int steps,int dd)
//================================================

{
  int i;
  digitalWrite(DIR, HIGH);
  digitalWrite(SLEEP, HIGH); // Set the Sleep mode to AWAKE.
  for(i=0;i<=steps;i++)
    {
      delayMicroseconds(200);
      digitalWrite(STEP, HIGH);
      delay(dd);
      digitalWrite(STEP, LOW);
      delay(dd);
      if (digitalRead(SW) == HIGH) return 2;

    }
  digitalWrite(DIR, LOW);
  digitalWrite(SLEEP, LOW); // Set the Sleep mode to SLEEP.
  return 1;
}
//================================================
void go_to_pos(int cur, int pos)
//================================================
{
  int delta = pos -cur;
    Serial.print( "delta = ");
      Serial.println(delta);
  if (delta > 0)
  {
    move_stepper(1, 1, delta, 100);
  }
  else
  {
    move_stepper(2, 1, abs(delta), 100);
  }
  current_pos += delta;
}
//================================================
void reset_pos()
//================================================
{
   Serial.println( "Reset position...");
   move_stepper(2,1,300,100);
   move_stepper(1,1,10, 100);
   current_pos = 10;
   Serial.println( "Reset finished!");
}

//================================================
void move_stepper(int dir, int step_size, int number_of_step, int delay_between_steps){
//================================================
        int sw = 0;

        Serial.print( " dir=");Serial.println( dir);
        Serial.print( " step_size=");Serial.println( step_size);
        Serial.print( " steps=");Serial.println( number_of_step);
        Serial.print( " delay=");Serial.println( delay_between_steps);

        if(step_size == FULL_STEP)
        {
            Serial.println( "Stepstepper FULL STEP");
            digitalWrite(MS1,LOW);
            digitalWrite(MS2,LOW);
        }
        else if (step_size == HALF_STEP)
        {
            Serial.println( "Stepper HALF STEP");
            digitalWrite(MS1,HIGH);
            digitalWrite(MS2,LOW);
        }
        else if (step_size == QUARTER_STEP)
        {
            Serial.println( "Stepper QUARTER STEP");
            digitalWrite(MS1,LOW);
            digitalWrite(MS2,HIGH);
        }
        else // default fullstep
        {
            Serial.println( "Steppernumber_of_step DEFAULT FULL STEP");
            digitalWrite(MS1,LOW);
            digitalWrite(MS2,LOW);
        }

        if(dir == CLOCKWISE)
        {
            current_pos +=  number_of_step;

            Serial.println( "Stepper motor CW -->");
            sw = stepCW(number_of_step, delay_between_steps);
        }
        else if(dir == COUNTER_CLOCKWISE)
        {
            current_pos -=  number_of_step;

            Serial.println( "Stepper motor CCW  <--");
            sw = stepCCW(number_of_step, delay_between_steps);
        }
        else
            Serial.println( "ERROR: Unknown direction for stepper motor");

        digitalWrite(MS1,LOW);
        digitalWrite(MS2,LOW);
        Serial.println( "Stepper sleeping");

        if(sw == 2)
        {
          current_pos = 0;
          Serial.println("RESET");
        }

       Serial.print( "current position: ");
       Serial.println(current_pos);
}
//================================================
void software_Reset() // Restarts program from beginning but does not reset the peripherals and registers
//================================================
{
asm volatile ("  jmp 0");
}

//================================================
void setup(void){
//================================================
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

   Serial.begin(9600);
    //Initialize
    pinMode(DIR,OUTPUT);
    pinMode(STEP,OUTPUT);
    pinMode(SLEEP,OUTPUT);
    pinMode(MS1,OUTPUT);
    pinMode(MS2,OUTPUT);
    pinMode(SW, INPUT);

    digitalWrite(MS1,LOW);
    digitalWrite(MS2,LOW);
    digitalWrite(SLEEP,LOW);
    digitalWrite(DIR,LOW);
    digitalWrite(STEP,LOW);
    //Possible settings are (MS1/MS2) : full step (0,0), half step (1,0), 1/4 step (0,1), and 1/8 step (1,1)
    //reset_pos();

    lib_wifiBegin(&c1);
    d1.counter = 0;
    c1.conf_id = c1.conf_mac;
    stat_url = lib_buildUrlStatic(c1);
    Serial.println(stat_url);
    String dont_care = lib_wifiConnectandSend(c1,d1, stat_url);
}
//================================================
void loop(void){
//================================================

  int move = 0;
  
  delay(c1.conf_period*1000);
  ++d1.counter;
  d1.rssi = WiFi.RSSI();
  
  if (d1.counter > c1.conf_wrap) d1.counter = 1;

  String url = lib_buildUrlDynamic(c1, d1);

  // Add payload
  url += "&payload=";
  url += "{";
  url += "\"position";
  url += "\":\"";
  url += current_pos;
  url += "\"";
  url += "}";

  String msg = lib_wifiConnectandSend(c1, url);

  int res = lib_decode_STEPPER(msg);

  move = 0;
  if (res < 100 && res > 0)
  {
    dir = CLOCKWISE;
    number_of_step = res;
    move = 1;
  }
  else if (res > 100 && res <200)
  {
    dir = COUNTER_CLOCKWISE;
    number_of_step = res - 100;
    move = 1;
  }
  else
  {
    move = 0;
  }
  if (move == 1)
  {
   move_stepper(dir, step_size, number_of_step, delay_between_steps);
   String msg = "Stepper moved: " + dir + " " + number_of_steps;
   url = lib_buildUrlLog(c1,msg);
   lib_wifiConnectandSend(c1, url);
  }
}
//=============================================
// End of File
//=============================================
