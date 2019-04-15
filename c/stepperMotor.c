//=============================================
// File.......: stepperMotor.c
// Date.......: 2019-04-13
int sw_version = 1;
// Author.....: Benny Saxen
// Description:
// Message Api:
// move,dir,step-size,steps,delay  example: 1,1,40,5
// reset                      reset position to zero
// reboot
// period,x                   set period to x seconds
// 90 degrees <=> 1152/4 steps = 288
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

int DIR    = 4;   // D2
int STEP   = 5;   // D1
int SLEEP  = 12;  // D6
int MS1    = 13;  // D7
int MS2    = 14;  // D5
int LIMIT  = 15;  // D8
int MINMAX = 10;  // S3

int dir = 0;
int step_size = FULL_STEP;
int number_of_steps = 0;
int delay_between_steps = 10;
int limit = 0;
int g_calibrated = 0;
//================================================
int stepCW(int steps,int dd, int force)
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
      if (digitalRead(LIMIT) == HIGH && force == 0) 
      {
        digitalWrite(MINMAX, HIGH);
        digitalWrite(DIR, LOW);
        digitalWrite(SLEEP, LOW);
        return 2;
      }
      if (digitalRead(LIMIT) == LOW && force == 1)
      {
        digitalWrite(MINMAX, LOW);
        digitalWrite(DIR, LOW);
        digitalWrite(SLEEP, LOW);
        return 12;
      }
    }
  digitalWrite(DIR, LOW);
  digitalWrite(SLEEP, LOW); // Set the Sleep mode to SLEEP.Serial.println
  return 1;
}

//================================================
int stepCCW(int steps,int dd, int force)
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
      if (digitalRead(LIMIT) == HIGH && force == 0)
      {
        digitalWrite(MINMAX, HIGH);
        digitalWrite(DIR, LOW);
        digitalWrite(SLEEP, LOW);
        return 3;
      }
      if (digitalRead(LIMIT) == LOW && force == 1)
      {
        digitalWrite(MINMAX, LOW);
        digitalWrite(DIR, LOW);
        digitalWrite(SLEEP, LOW);
        return 13;
      }
    }
  digitalWrite(DIR, LOW);
  digitalWrite(SLEEP, LOW); // Set the Sleep mode to SLEEP.
  return 1;
}

//================================================
int move_stepper(int dir, int step_size, int number_of_step, int delay_between_steps){
//================================================
        int sw = 0;

        Serial.print( " dir=");Serial.println( dir);
        Serial.print( " step_size=");Serial.println( step_size);
        Serial.print( " steps=");Serial.println( number_of_steps);
        Serial.print( " delay=");Serial.println( delay_between_steps);

        if(step_size == FULL_STEP)
        {
            Serial.println( "FULL STEP");
            digitalWrite(MS1,LOW);
            digitalWrite(MS2,LOW);
        }
        else if (step_size == HALF_STEP)
        {
            Serial.println( "HALF STEP");
            digitalWrite(MS1,HIGH);
            digitalWrite(MS2,LOW);
        }
        else if (step_size == QUARTER_STEP)
        {
            Serial.println( "QUARTER STEP");
            digitalWrite(MS1,LOW);
            digitalWrite(MS2,HIGH);
        }
        else // default fullstep
        {
            Serial.println( "DEFAULT FULL STEP");
            digitalWrite(MS1,LOW);
            digitalWrite(MS2,LOW);
        }

        if(dir == CLOCKWISE)
        {
            Serial.println( "Stepper motor CW -->");
            sw = stepCW(number_of_steps, delay_between_steps,0);
            if (sw == 1) current_pos -=  number_of_steps;
        }
        else if(dir == COUNTER_CLOCKWISE)
        {
            Serial.println( "Stepper motor CCW  <--");
            sw = stepCCW(number_of_steps, delay_between_steps,0);
            if (sw == 1) current_pos +=  number_of_steps;
        }
        else
            Serial.println( "ERROR: Unknown direction for stepper motor");

        if(sw == 2)
        {
          sw = stepCCW(99, delay_between_steps,1);
          current_pos = 0;
          Serial.println("MIN_LIMIT");
        }
        if(sw == 3)
        {
          sw = stepCW(99, delay_between_steps,1);
          current_pos = 288;
          Serial.println("MAX_LIMIT");
        }

        digitalWrite(MS1,LOW);
        digitalWrite(MS2,LOW);
        Serial.println( "Stepper sleeping");
        
        Serial.print( "current position: ");
        Serial.println(current_pos);
        return sw;
}
//================================================
int calibrate(){
//================================================
        int touch1 = 0;
        int touch2 = 0;
        int ok = 0;

        Serial.println( "Calibrate max level...");
        touch1 = stepCW(400, delay_between_steps,0);
        if (touch1 == 2) 
        {
          Serial.println( "Release max level...");
          touch2 = stepCCW(99, delay_between_steps,1);
          if (touch2 == 13)
          {
             current_pos = 288;
             Serial.println("MAX_LIMIT reached");
             ok += 1;
          }
        }
        else
        {
          Serial.println("ERROR: Calibrate max failed");
        }

        delay(10);
        
        Serial.println( "Calibrate zero level...");
        touch1 = stepCCW(400, delay_between_steps,0);
        if (touch1 == 3)
        {
          Serial.println( "Release min level...");
          touch2 = stepCW(99, delay_between_steps,1);
          if (touch2 == 12)
          {
             current_pos = 0;
             Serial.println("MIN_LIMIT reached");
             ok += 1;
          }
        }
        else
        {
          Serial.println("ERROR: Calibrate min failed");
        }

        Serial.print( "Calibrate result");
        Serial.println(ok);
        return ok;    
}
//================================================
void setup(void){
//================================================
  co.conf_sw         = sw_version;
  co.conf_id         = "set_to_mac";
  co.conf_period     = 20;
  co.conf_wrap       = 999999;
  co.conf_feedback   = 1;

  co.conf_title      = "kvv32_stepper";
  co.conf_tags       = "kvv32_stepper";
  co.conf_desc       = "kvv32_stepper";
  co.conf_platform   = "esp8266";

  co.conf_domain     = "iot.simuino.com";
  co.conf_server     = "gateway.php";
  
  co.conf_ssid_1     = "bridge";
  co.conf_password_1 = "123";
  
  co.conf_ssid_2     = "NABTON";
  co.conf_password_2 = "a1b2c3d4e5f6g7";
  
  co.conf_ssid_3     = "ASUS";
  co.conf_password_3 = "123";

  lib_setup(&co, &da);
  
    //Initialize
    pinMode(DIR,OUTPUT);
    pinMode(STEP,OUTPUT);
    pinMode(SLEEP,OUTPUT);
    pinMode(MS1,OUTPUT);
    pinMode(MS2,OUTPUT);
    pinMode(LIMIT, INPUT);
    pinMode(MINMAX,OUTPUT);
    
    digitalWrite(MS1,LOW);
    digitalWrite(MS2,LOW);
    digitalWrite(SLEEP,LOW);
    digitalWrite(DIR,LOW);
    digitalWrite(STEP,LOW);
    digitalWrite(MINMAX,LOW);
  
    //Possible settings are (MS1/MS2) : full step (0,0), half step (1,0), 1/4 step (0,1), and 1/8 step (1,1)

    delay(10);
    int ok = calibrate();
    if (ok != 2)
    {
      Serial.println("Calibration failed");
      g_calibrated = 0;
      //exit(0);
    }
    else
    {
      Serial.println("Calibration success!");
      g_calibrated = 1;
    }
}
//================================================
void loop(void){
//================================================

  int move = 0;
  
  String msg;
  
  msg = lib_loop(&co,&da);
  Serial.println(msg);
  
  int res = lib_decode_STEPPER(msg);

  move = 0;
  if (res < 200 && res > 100)
  {
    dir = CLOCKWISE;
    number_of_steps = res - 100;
    move = 1;
  }
  else if (res > 200 && res < 300)
  {
    dir = COUNTER_CLOCKWISE;
    number_of_steps = res - 200;
    move = 1;
  }
  else
  {
    move = 0;
  }
  if (move == 1)
  {
    limit = move_stepper(dir, step_size, number_of_steps, delay_between_steps);

    g_payload = "{";

    g_payload += "\"counter";
    g_payload += "\":\"";
    g_payload += da.counter;
    g_payload += "\",";

    g_payload += "\"calibrated";
    g_payload += "\":\"";
    g_payload += limit;
    g_payload += "\",";

    g_payload += "\"limit";
    g_payload += "\":\"";
    g_payload += limit;
    g_payload += "\",";
    
    g_payload += "\"position";
    g_payload += "\":\"";
    g_payload += current_pos;
    g_payload += "\",";
    
    g_payload += "\"steps";
    g_payload += "\":\"";
    g_payload += number_of_steps;
    g_payload += "\",";
    
    g_payload += "\"direction";
    g_payload += "\":\"";
    g_payload += dir;
    g_payload += "\"";
    
    g_payload += "}";
    
    lib_publishPayload(&co,&da,g_payload);
    //Serial.println(msg);

    g_message = "stepper_";
    g_message += dir;
    g_message += "_";
    g_message += number_of_steps;
    g_message += "_";
    g_message += limit;
    lib_publishLog(&co,&da,g_message);
  }
  
}
//=============================================
// End of File
//=============================================
