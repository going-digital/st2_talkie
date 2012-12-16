//*******************************************************************************************************************
// 								                                         Main Loop 
//*******************************************************************************************************************
void loop()
{
  // Test for Sleep ------------------------------------------------*

  currentMillis = millis();
  OptionModeFlag = false;

  if(((currentMillis - SleepTimer) > SleepLimit) && SleepEnable)
  {
    STATE= 99; 
  }

  // Test for Mode Button Press ------------------------------------* 

  bval = !digitalRead(MODEBUTTON);
  if(bval)
  {
    if(ALARMON)
    {
    CheckAlarm();
    }

    if(ALARM1FLAG)
    {
      ALARM1FLAG = false;
      ALARMON = false;
      EnableAlarm1(false);
      STATE = 90;
      JustWokeUpFlag = false;
    }
    else
    {
      if(JustWokeUpFlag)
      {
        JustWokeUpFlag = false;
        JustWokeUpFlag2 = true;                                    // Used to supress "Time" text from showing when waking up.
      }
      else
      {
      NextStateRequest = true;
      }
   //   SUBSTATE = 99;

      while(bval)
      {
          bval = !digitalRead(SETBUTTON);
          if(bval)
           {
            OptionModeFlag = true;
            NextStateRequest = false;
            NextSUBStateRequest = false;
            displayString("SPEC");
            delay(300);
           }      
        bval = !digitalRead(MODEBUTTON);
      }

      delay(100);
      SleepTimer = millis();
    }
  }

  // Test for SET Button Press ------------------------------------*
  
  bval = !digitalRead(SETBUTTON);
  if(bval && !OptionModeFlag)
  {
    NextSUBStateRequest = true;

    while(bval)
    {
      
          bval = !digitalRead(MODEBUTTON);
          if(bval)
           {
            OptionModeFlag = true;
            NextStateRequest = false;
            NextSUBStateRequest = false;
            displayString("SPEC");
            delay(300);
           }   
      
      
      bval = !digitalRead(SETBUTTON);
    }
    delay(100);
    SleepTimer = millis();
  }
 
   // Running Blink counter ------------------------------------* 
  if(blinkFlag)
  {
    blinkCounter = blinkCounter +1;
    if(blinkCounter >blinkTime)                                        // was 150
    {
      blinkON = !blinkON;
      blinkCounter = 0;
    }
  }
  else
  {
    blinkON = true;                                                    // Not blinking, just leave the LEDs lit
  }  
  
//*******************************************************************************************************************
// 								                        Main Loop - State Machine 
//*******************************************************************************************************************

  switch (STATE) 
  {
  case 0:                                                                  // Set-Up
    STATE = 1;
    break;

  case 1:                                                                  // Display Time
    DisplayTimeSub(); 
    break;

  case 2:                                                                  // Set Time
    setTimeSub();
    break; 

  case 3:                                                                  // Config Alarm
   setAlarmSub();
    break;
 
   case 4:                                                                 // Stop Watch
    StopWatch();
    break;
 
    
  case 5:                                                                 // Serial Display                                 
  DisplaySerialData();
  break;

  case 6:                                                                 // Graphic Demo                                 
  graphican();
  break;

   // ---------------------------------------------------------------   
  
  case 90:                                                                  // Alarm Triggered
  
    blinkFlag = true;
    displayString("Beep");

  if(blinkON)
  {
    pinMode(SETBUTTON, OUTPUT);
    tone(SETBUTTON,4000) ;
    delay(100);
    noTone(SETBUTTON);
    digitalWrite(SETBUTTON, HIGH);
  }

    #if ARDUINO >= 101 
    pinMode(SETBUTTON, INPUT_PULLUP);
//    digitalWrite(SETBUTTON, HIGH);
     #else
//    digitalWrite(SETBUTTON, HIGH);
    pinMode(SETBUTTON, INPUT);
     #endif
    delay(250);

//    bval = !digitalRead(SETBUTTON);
    if(NextSUBStateRequest | NextStateRequest)
    {
      STATE = 0;
      SUBSTATE = 0;
 //     NextStateFlag = true;
      NextStateRequest = false;
      NextSUBStateRequest = false;      
      blinkFlag = false;
    }    
    break;

    // --------------------------------------------------------------- 

  case 99:                                                                    // Sleep
    displayString("Nite");
    delay(500);
    clearmatrix();
    sayTime();
    GoToSleep();
    SleepTimer = millis();
    STATE = 0;
    SUBSTATE = 0;
    break;

    // --------------------------------------------------------------- 

  }
}


//*******************************************************************************************************************
// 								                                 End of Main Loop
//*******************************************************************************************************************







