//=============================================
// File.......: stepperMotor.c
// Date.......: 2019-12-17
int sw_version = 1;
// Author.....: Benny Saxen
//=============================================
// Description:
// Note: CW, CCW orientation is from stepper motor facing axis side
// Message Api:
// move,dir,step-size,steps,delay  example: 1,1,40,5
// reset                      reset position to zero
// reboot
// period,x                   set period to x seconds
// 90 degrees <=> 1152/4 steps = 288
//=============================================
// Debug Codes
//
// Normal 
// 801 CW limit reached
// 802 CW limit released
// 803 CCW limit reached
// 804 CCW limit released
// 805 Unknow limit detected
// 901 CW ok
// 902 CCW ok
// Error
// 9003 Failure during calibration, cw limit not found
// 9004 Failure during calibration, ccw limit not found
// 9005 Failure during calibration, cw and ccw limit not found
//=============================================
//#include "iotLib.c"
//================================================
// Globals
//================================================
int current_pos       = 0;
int FULL_STEP         = 1;
int HALF_STEP         = 2;
int QUARTER_STEP      = 3;
int CLOCKWISE         = 1; // Increase
int COUNTER_CLOCKWISE = 2; // Decrease

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
int limit = 901;
int g_calibrated = 0;
int g_steps = 0;
int g_simulated_position = 77;
int g_cw_ccw = 0; // Normal shunt rotation
//================================================
int stepCW(int steps,int dd, int force)
//================================================
{
  int i;
  int buf = 10;
  digitalWrite(DIR, LOW);
  digitalWrite(SLEEP, HIGH); // Set the Sleep mode to AWAKE.
  for(i=0;i<=steps;i++)
    {
      g_simulated_position++;
      delayMicroseconds(200);
      digitalWrite(STEP, HIGH);
      delay(dd);
      digitalWrite(STEP, LOW);
      delay(dd);
      //if (g_simulated_position == 222 && force == 0) 
      if (digitalRead(LIMIT) == HIGH && force == 0) 
      {
        digitalWrite(MINMAX, HIGH);
        digitalWrite(DIR, LOW);
        digitalWrite(SLEEP, LOW);
        g_steps = i;
        Serial.println("stepCW - limit reached");
        return 801;
      }
      //if (g_simulated_position > 10 && force == 1 && buf > 0)
      if (digitalRead(LIMIT) == LOW && force == 1 && buf > 0)
      {
        buf--;
        Serial.println(buf);
        if( buf == 0)
        {
          digitalWrite(MINMAX, LOW);
          digitalWrite(DIR, LOW);
          digitalWrite(SLEEP, LOW);
          g_steps = i;
          Serial.println("stepCW - limit released");
          return 804;
        }
      }
    }
  digitalWrite(DIR, LOW);
  digitalWrite(SLEEP, LOW); // Set the Sleep mode to SLEEP
  g_steps = i;
  Serial.println(steps);
  return steps;
}

//================================================
int stepCCW(int steps,int dd, int force)
//================================================

{
  int i;
  int buf = 10;
  digitalWrite(DIR, HIGH);
  digitalWrite(SLEEP, HIGH); // Set the Sleep mode to AWAKE.
  for(i=0;i<steps;i++)
    {
      g_simulated_position--;
      delayMicroseconds(200);
      digitalWrite(STEP, HIGH);
      delay(dd);
      digitalWrite(STEP, LOW);
      delay(dd);
      //if (g_simulated_position == 0 && force == 0)
      if (digitalRead(LIMIT) == HIGH && force == 0)
      {
        digitalWrite(MINMAX, HIGH);
        digitalWrite(DIR, LOW);
        digitalWrite(SLEEP, LOW);
        g_steps = i;
        Serial.println("stepCCW - limit reached");
        return 803;
      }
      //if (g_simulated_position < 211 && force == 1 && buf > 0)
      if (digitalRead(LIMIT) == LOW && force == 1 && buf > 0)
      {
        buf--;
        Serial.println(buf);
        if( buf == 0)
        {
          digitalWrite(MINMAX, LOW);
          digitalWrite(DIR, LOW);
          digitalWrite(SLEEP, LOW);
          g_steps = i;
          Serial.println("stepCCW - limit released");
          return 802;
        }
      }
    }
  digitalWrite(DIR, LOW);
  digitalWrite(SLEEP, LOW); // Set the Sleep mode to SLEEP.
  g_steps = i;
  Serial.println(steps);
  return steps;
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
          // All steps executed
            if (sw == number_of_steps)
            {
              sw = 901;
            }
          // Max position reached, move back from CW limit
            if (sw == 801)
            {
               sw = stepCCW(99, delay_between_steps,1);
               Serial.println("CW release");             
            }
        }
        else if(dir == COUNTER_CLOCKWISE)
        {
            Serial.println( "Stepper motor CCW  <--");
            sw = stepCCW(number_of_steps, delay_between_steps,0);
          // All steps executed
            if (sw == number_of_steps)
            {
              sw = 902;
            }
          // Min position reached, move back from CCW limit
            if (sw == 803)
            {
               sw = stepCW(99, delay_between_steps,1);
               Serial.println("CCW release");             
            }
        }
        else
            Serial.println( "ERROR: Unknown direction for stepper motor");

        digitalWrite(MS1,LOW);
        digitalWrite(MS2,LOW);
        Serial.println( "Stepper sleeping");
        
        Serial.print( "current position: ");
        Serial.println(current_pos);
        return sw;
}
//================================================
int cw_limit()
// Returns 802 of ok, else 9003
//================================================
{
        int touch1 = 0;
        int touch2 = 0;
        int ok = 0;
        
        Serial.println( "Calibrate CW level...");
        touch1 = stepCW(400, delay_between_steps,0);
        if (touch1 == 801) 
        {
          //Serial.println("CW_LIMIT reached");
          touch2 = stepCCW(99, delay_between_steps,1);
          //Serial.println("CW_LIMIT release value");
          //Serial.println(touch2);
          if (touch2 == 802)
          {
             //Serial.println( "Released CW limit");
             Serial.print( "Simulated position = ");
             Serial.println(g_simulated_position);
             ok = touch2;
          }
        }
        else
        {
          Serial.println("ERROR: Calibrate CW failed");
          ok = 9003;
        }
        return(ok);
}
//================================================
int ccw_limit()
// Returns 804 of ok, else 9004
//================================================
{
        int touch1 = 0;
        int touch2 = 0;
        int ok = 0;
        
        Serial.println( "Calibrate CCW level...");
        touch1 = stepCCW(400, delay_between_steps,0);
        if (touch1 == 803)
        {
          //Serial.println("CCW_LIMIT reached");
          touch2 = stepCW(99, delay_between_steps,1);
          if (touch2 == 804)
          {
             //Serial.println( "Released CCW level");
             Serial.print( "Simulated position = ");
             Serial.println(g_simulated_position);
             ok = touch2;
          }
        }
        else
        {
          Serial.println("ERROR: Calibrate CCW failed");
          ok = 9004;
        }

        return(ok);
}
//================================================
int calibrate(int cw_ccw)
// Returns number of steps in working interval
// cw_ccw = 0: CCW hot CW cool (normal)
// cw_ccw = 1: CCW cool CW hot
//================================================
{
        int both = 0;
        int range = 0;
        int ok = 0;

        int res = 0;

        // Full Step
        Serial.println( "FULL STEP");
        digitalWrite(MS1,LOW);
        digitalWrite(MS2,LOW);
  
        res = 0;
        if (cw_ccw == 0)
        {
          res = ccw_limit();
        }
        else
        {
          res = cw_limit();
        }
        if (res > 9000) both++;
        
        delay(100);
        
        res = 0;
        if (cw_ccw == 0)
        {
          res = cw_limit();
        }
        else
        {
          res = ccw_limit();
        }
        if (res > 9000) both++;

        
        if (both == 2) ok = 9005;
        
        Serial.print( "Calibration result ");
        Serial.println(ok);

        if (ok == 0)
        {
          if (cw_ccw == 0)
          {
            res = stepCCW(30, delay_between_steps,0);
          }
          else
          {
            res = stepCW(30, delay_between_steps,0);
          } 

          Serial.print( "Last Stepper move ");
          Serial.println(res);
          Serial.println( "At rest ...");
        }
        return ok;    
}
//================================================
void setup(void){
//================================================
  co.conf_sw         = sw_version;
  co.conf_id         = "set_to_mac";
  co.conf_period     = 60;
  co.conf_wrap       = 999999;
  co.conf_feedback   = 1;

  co.conf_title      = "kvv32_stepper";
  co.conf_tags       = "kvv32_stepper";
  co.conf_desc       = "kvv32_stepper";
  co.conf_platform   = "esp8266";

  co.conf_domain     = "iot.simuino.com";
  co.conf_server     = "gateway.php";
  
  strcpy(co.conf_ssid_1,"bridge");
  strcpy(co.conf_password_1,"qwer");

  strcpy(co.conf_ssid_2,"NABTON");
  strcpy(co.conf_password_2,"a1b2c3d4e5f6g7");
  
  strcpy(co.conf_ssid_3,"ASUS");
  strcpy(co.conf_password_3,"123");

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
    g_steps = 0;
    delay(10);
    int ok = calibrate(g_cw_ccw);
    if (ok > 9000)
    {
      Serial.println("Calibration failed");
      g_calibrated = ok;
      limit = 0;
      //exit(0);
    }
    else
    {
      Serial.println("Calibration success!");
      g_calibrated = ok;
      limit =  804;
    }

    g_payload = "{";

    g_payload += "\"counter";
    g_payload += "\":\"";
    g_payload += da.counter;
    g_payload += "\",";

    g_payload += "\"calibrated";
    g_payload += "\":\"";
    g_payload += g_calibrated;
    g_payload += "\",";

    g_payload += "\"limit";
    g_payload += "\":\"";
    g_payload += limit;
    g_payload += "\",";
    
    g_payload += "\"steps";
    g_payload += "\":\"";
    g_payload += 0;
    g_payload += "\",";
    
    g_payload += "\"direction";
    g_payload += "\":\"";
    g_payload += 0;
    g_payload += "\"";
    
    g_payload += "}";
    
    lib_publishPayload(&co,&da,g_payload);
}
//================================================
void loop(void){
//================================================

  int move = 0;
  
  String msg;

  if (digitalRead(LIMIT) == HIGH)
  {
        digitalWrite(MINMAX, HIGH);
        limit = 805;
  }
  
  if (digitalRead(LIMIT) == LOW)
  {
        digitalWrite(MINMAX, LOW);
        limit = 804;
  }
  
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
  else if (res == 6614)
  {
    int ok = calibrate(g_cw_ccw);
    if (ok > 9000)
    {
      Serial.println("Calibration failed");
      g_calibrated = ok;
    }
    else
    {
      Serial.println("Calibration success!");
      g_calibrated = ok;
    }
    move = 0;
  }
  else
  {
    move = 0;
  }
  if (move == 1 || res == 6614 || limit == 805)
  {
    limit = move_stepper(dir, step_size, number_of_steps, delay_between_steps);

    g_payload = "{";

    g_payload += "\"counter";
    g_payload += "\":\"";
    g_payload += da.counter;
    g_payload += "\",";

    g_payload += "\"calibrated";
    g_payload += "\":\"";
    g_payload += g_calibrated;
    g_payload += "\",";

    g_payload += "\"limit";
    g_payload += "\":\"";
    g_payload += limit;
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
