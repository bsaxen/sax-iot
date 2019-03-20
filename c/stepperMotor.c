//=============================================
// File.......: stepperMotor.c
// Date.......: 2019-03-20
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
//#include "iotLib.c"
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
void setup(void){
//================================================
    co.conf_sw         = sw_version;
    co.conf_id         = "set_to_mac";
    co.conf_period     = 60;
    co.conf_wrap       = 999999;
    co.conf_feedback   = 1;

    co.conf_title      = "test1";
    co.conf_tags       = "test1";
    co.conf_desc       = "test1";
    co.conf_platform   = "esp8266";
    co.conf_ssid       = "bridge";
    co.conf_password   = "dfgdfg";
    co.conf_domain     = "iot.simuino.com";
    co.conf_server     = "gateway.php";
    co.conf_streamId   = "....................";
    co.conf_privateKey = "....................";

    lib_setup(&co, &da);
  
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
}
//================================================
void loop(void){
//================================================

  int move = 0;
  
  String msg;
  delay(co.conf_period*1000);
  
  ++da.counter;
  da.rssi = WiFi.RSSI();
  if (da.counter > co.conf_wrap) da.counter = 1;
  
  dyn_url    = lib_buildUrlDynamic(&co, &da);  
  msg = lib_wifiConnectandSend(&co,&da, dyn_url);
  Serial.println(msg);
  
  int res = lib_decode_STEPPER(msg);

  move = 0;
  if (res < 100 && res > 0)
  {
    dir = CLOCKWISE;
    number_of_step = res;
    move = 1;
  }
  else if (res > 100 && res < 200)
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
    String message = "Stepper moved: " + dir + " " + number_of_steps;
    log_url = lib_buildUrlLog(&co,message);
    msg = lib_wifiConnectandSend(&co,&da, log_url);
    Serial.println(msg);
  }
  
}
//=============================================
// End of File
//=============================================
