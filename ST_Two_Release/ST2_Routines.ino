//*******************************************************************************************************************
// 								                        Helper - State incrementor
//*******************************************************************************************************************
// Increments the Main state to the next state upto MAXSTATE
// If you add more States increase MAXSTATE to match.

void NextState()
{
  STATE = STATE + 1;
  SUBSTATE = 0;
  NextStateRequest = false;
  if(STATE > MAXSTATE)
  {
    STATE = 0;
    SUBSTATE = 0;
  }
}


//*******************************************************************************************************************
// 								                                    Beep the piezo
//*******************************************************************************************************************
// NOTE: the piezo uses the same pin as the SET Button input
// This routine switches the pin to an output during the beeping process
// (There is a current limited resistor on the board to prevent over current if the Set Button is pressed at
//  the same time as the pin is driving the piezo.)

void beepsound(int freq, int freqlenght)
{
  // freq was 4000
  // freqlenght was 100
  pinMode(SETBUTTON, OUTPUT);
  tone(SETBUTTON,freq,freqlenght);
  delay(freqlenght);
  noTone(SETBUTTON);
  digitalWrite(SETBUTTON, HIGH);
  
  #if ARDUINO >= 101 
  pinMode(SETBUTTON, INPUT_PULLUP);
//  digitalWrite(SETBUTTON, HIGH);
  #else
//  digitalWrite(SETBUTTON, HIGH);
  pinMode(SETBUTTON, INPUT);
  #endif
  
}

//*******************************************************************************************************************
// 								                              Display Time Routine
//*******************************************************************************************************************
void DisplayTimeSub()
{

  if(NextStateRequest)
  {
    SUBSTATE =99;
  }

  if(NextSUBStateRequest)
  {
    SUBSTATE = SUBSTATE +1;
    if(SUBSTATE>4)
    {
      SUBSTATE =1;
    }
    NextSUBStateRequest = false;
  }

  UpdateTime = UpdateTime + 1;
  if(UpdateTime > 2000)
  {
    checktime();
    checkDate();
    UpdateTime = 0;
  }


  switch (SUBSTATE) 
  {
  case 0:                // Start Display Time
    SUBSTATE = 1;
    blinkON = true;
    blinkFlag = false;
    blinkMin = false;
    blinkHour = false;
    
    checktime();
    checkDate();
    
    if(!JustWokeUpFlag2)
    {
    displayString("Time");
    }
    else
    {
      JustWokeUpFlag2 = false;
    }
    delay(250);
    break;

  case 1:              // Time

//    checktime();
    writeTime(HourTens, HourOnes, MinTens, MinOnes);
    break;

  case 2:              // Day
 //   checkDate();
    displayStringDay(Days -1);
    break;

  case 3:              // Month
 //   checkDate();
    displayMonth(MonthCode-1);
    break;      

  case 4:              // Date
 //   checkDate();
    displayDate();
    delay(100);
    break;  

  case 5:              // Year

    break;  

  case 99:              // Exit Display Time

    NextState();
    clearmatrix();

    break;  
  }  
}

//*******************************************************************************************************************
// 								                              SET Time Routine
//*******************************************************************************************************************
void setTimeSub()
{


  switch (SUBSTATE) 
  {
  case 0:                // Start SET Time
    clearmatrix();
    displayString("Set?");
    SUBSTATE = 1;
    NextSUBStateRequest = false;


    break;

  case 1:                                                          // Ask user if they want to set time
    if(NextSUBStateRequest)
    {
      SUBSTATE = 2;
      NextSUBStateRequest = false;
      blinkFlag = true;
    }

    if(NextStateRequest)
    {
      SUBSTATE =99;
      NextStateRequest = false;
    }

    break;

  case 2:                                                          // Minute + one
    //    displayString("----");
    blinkMin = true;
    writeTime(HourTens, HourOnes, MinTens, MinOnes);
    if(NextSUBStateRequest)
    {
      settimeNEW(1);                                      
      NextSUBStateRequest = false;
    }

    if(NextStateRequest)
    {
      blinkMin = false;
      SUBSTATE =3;
      NextStateRequest = false;
    }
    break;

  case 3:                                                          // Hours + one
    blinkHour = true;
    writeTime(HourTens, HourOnes, MinTens, MinOnes);
    if(NextSUBStateRequest)
    {
      settimeNEW(2);                                     
      NextSUBStateRequest = false;
    }

    if(NextStateRequest)
    {
      blinkHour = false;
      SUBSTATE =4;
      NextStateRequest = false;
    }

    break;      

  case 4:                                                          // Day + one
    displayStringDay(Days -1);
    if(NextSUBStateRequest)
    {
      settimeNEW(3);                                     
      NextSUBStateRequest = false;
    }

    if(NextStateRequest)
    {
      SUBSTATE =5;
      NextStateRequest = false;
    }

    break;  

  case 5:                                                          //  Month + one
    displayMonth(MonthCode-1);
    if(NextSUBStateRequest)
    {
      settimeNEW(4);                                     
      NextSUBStateRequest = false;
    }

    if(NextStateRequest)
    {
      SUBSTATE =6;
      NextStateRequest = false;
    }
    break;  

  case 6:                                                          //  Date + one
    displayDate();
    if(NextSUBStateRequest)
    {
      settimeNEW(5);                                     
      NextSUBStateRequest = false;
    }
    if(NextStateRequest)
    {
      SUBSTATE =8;
      NextStateRequest = false;
    }
    break;     

    /*
  case 7:                                                          //  Year + one ** NOTE: not used
     displayString("Year"); 
     
     if(NextSUBStateRequest)
     {
     settimeNEW(6);                                     
     NextSUBStateRequest = false;
     }
     
     if(NextStateRequest)
     {
     SUBSTATE =8;
     NextStateRequest = false;
     }
     break;
     */

  case 8:
    NewTimeFormate = TH_Not24_flag;                                // Pre-set before toggle
    SUBSTATE =9;
    break;


  case 9:                                                          // Select 12 or 24 hour clock

    if(NewTimeFormate)
    {   
      displayString("12 h"); 
    }
    else   
    {
      displayString("24 h");
    }

    if(NextSUBStateRequest)
    {
      NewTimeFormate = !NewTimeFormate;                                     
      NextSUBStateRequest = false;
      TwelveTwentyFourConvert();
      A_TH_Not24_flag = NewTimeFormate;
    }


    if(NextStateRequest)
    {
      SUBSTATE =99;
      NextStateRequest = false;
    }

    break; 

  case 99:              // Exit Set Time
    blinkFlag = false;
    NextState();
    clearmatrix();

    break;    
  }
}

//*******************************************************************************************************************
// 								                              SET Alarm Routine
//*******************************************************************************************************************
void setAlarmSub()
{


  switch (SUBSTATE) 
  {
  case 0:                // Start SET Alarm
    clearmatrix();
    displayString("ALM?");
    SUBSTATE = 1;
    NextSUBStateRequest = false;


    break;

  case 1:                                                          // Ask user if they want to set Alarm
    if(NextSUBStateRequest)
    {
      //      displayString("A ON");
      //      delay(250);
      //      ALARMON = true;
      SUBSTATE = 2;
      NextSUBStateRequest = false;
      blinkFlag = true;
    }

    if(NextStateRequest)
    {
      //      displayString("AOFF");
      //      delay(500);
      //      EnableAlarm1(false);
      //      ALARMON = false;
      SUBSTATE =99;
      NextStateRequest = false;
    }

    break;

  case 2:
    blinkMin = true;
    writeTime(AHourTens, AHourOnes, AMinTens, AMinOnes);
    if(NextSUBStateRequest)
    {
      setAlarm(1);                                      
      NextSUBStateRequest = false;
    }

    if(NextStateRequest)
    {
      blinkMin = false;
      SUBSTATE =3;
      NextStateRequest = false;
    }
    break;   

  case 3:
    blinkHour = true;
    writeTime(AHourTens, AHourOnes, AMinTens, AMinOnes);
    if(NextSUBStateRequest)
    {
      setAlarm(2);                                      
      NextSUBStateRequest = false;
    }

    if(NextStateRequest)
    {
      blinkMin = false;
      SUBSTATE =4;
      NextStateRequest = false;
      //      EnableAlarm1(true);
    }  
    break;

  case 4:
    if(ALARMON)
    {   
//      displayString("A ON"); 
      displayGraphic(1,0,5);
      displayGraphic(2,5,5);
      displayGraphic(0,10,4);
      displayGraphic(3,14,5);
    }
    else   
    {
//      displayString("AOFF");
      displayGraphic(1,0,5);
      displayGraphic(2,5,5);
      displayGraphic(0,10,4);
      displayGraphic(4,14,5);
    }

    if(NextSUBStateRequest)
    {
      ALARMON = !ALARMON;                                     
      NextSUBStateRequest = false;
    }


    if(NextStateRequest)
    {
      blinkMin = false;
      SUBSTATE =99;
      NextStateRequest = false;  
      if(ALARMON)
      {
        EnableAlarm1(true);
      }
      else
      {
        EnableAlarm1(false);
      }
    }

    break;  

  case 99:              // Exit Set Alarm
    blinkFlag = false;
    NextState();
    clearmatrix();

    break;      
  }
}  

//*******************************************************************************************************************
// 								                              Stop Watch
//*******************************************************************************************************************
void StopWatch()
{

  switch (SUBSTATE) 
  {  
  case 0:                                                          // Stop Watch Set-up



    OldTime = 0;
    CurrentTime = 0;
    TotalTime = 0;
    SWDigit4 = 0;
    SWDigit3 = 0;
    SWDigit2 = 0;
    SWDigit1 = 0;

    blinkON = true;
    blinkFlag = false;
    blinkMin = false;
    blinkHour = false;

    SUBSTATE = 1;
    NextSUBStateRequest = false;
    displayString("Stop");
    delay(500);

    clearmatrix();
    writeTime(SWDigit4, SWDigit3, SWDigit2, SWDigit1);

    break; 

  case 1:                                                          // Waiting for "Start" button to be pressed
    writeTime(SWDigit4, SWDigit3, SWDigit2, SWDigit1);

    if(NextSUBStateRequest)
    {
      SUBSTATE = 2;    
      NextSUBStateRequest = false;
      SleepEnable = false;
      currentMillis = millis();
      SleepTimer = currentMillis;                                  // Using Long SleepTimer variable for timing not sleep
    }

    if(NextStateRequest)
    {
      SUBSTATE =99;
      NextStateRequest = false;
    }  

    break;

  case 2:                                                          // Stop Watch Running

 //   I2C_RX(RTCDS1337,RTC_SEC);
 //   CurrentTime =i2cData & B00001111;
 currentMillis = millis();
    if((currentMillis - SleepTimer) >= 1000)
    {
 //     OldTime = CurrentTime;
       SleepTimer = currentMillis;
      TotalTime = TotalTime + 1;
      if(TotalTime > 5999)                                     // Over 99 minutes can "not" be displayed (60seconds x 99 = 5940) 
      {
        TotalTime = 0;
      }
    }

    // Convert Total Time to digits
    SWMINUTES = TotalTime / 60;
    SWSECONDS = TotalTime % 60;

    SWDigit4 = SWMINUTES / 10;
    SWDigit3 = SWMINUTES % 10;
    SWDigit2 = SWSECONDS / 10;
    SWDigit1 = SWSECONDS % 10;    

    writeTime(SWDigit4, SWDigit3, SWDigit2, SWDigit1);

    if(NextSUBStateRequest)
    {
      SUBSTATE = 1;    
      NextSUBStateRequest = false;
      SleepEnable = true;
    }

    if(NextStateRequest)
    {
      SUBSTATE =99;
      NextStateRequest = false;
    }  
    break; 
    
/*    
// Not using this RTC version since it seems too power hungery due to constant I2C polling
  case 2:                                                          // Stop Watch Running

    I2C_RX(RTCDS1337,RTC_SEC);
    CurrentTime =i2cData & B00001111;
    if(CurrentTime != OldTime)
    {
      OldTime = CurrentTime;
      TotalTime = TotalTime + 1;
      if(TotalTime > 5940)                                     // Over 99 minutes can "not" be displayed (60seconds x 99 = 5940) 
      {
        TotalTime = 0;
      }
    }

    // Convert Total Time to digits
    SWMINUTES = TotalTime / 60;
    SWSECONDS = TotalTime % 60;

    SWDigit4 = SWMINUTES / 10;
    SWDigit3 = SWMINUTES % 10;
    SWDigit2 = SWSECONDS / 10;
    SWDigit1 = SWSECONDS % 10;    

    writeTime(SWDigit4, SWDigit3, SWDigit2, SWDigit1);

    if(NextSUBStateRequest)
    {
      SUBSTATE = 1;    
      NextSUBStateRequest = false;
      SleepEnable = true;
    }

    if(NextStateRequest)
    {
      SUBSTATE =99;
      NextStateRequest = false;
    }  
    break; 
*/
  case 99:              // Exit Stop Watch Function

    NextState();
    clearmatrix();

    SleepEnable = true;

    break;   
  } 
}

//*******************************************************************************************************************
// 								                           Display Serial Data
//*******************************************************************************************************************

void DisplaySerialData()
{
  int temp =0;
  switch (SUBSTATE) 
  {  

  case 0:                                                          // Display Set-up

    ResetScrollMessage();


    NextSUBStateRequest = false;
    NextStateRequest = false;
    OptionModeFlag = false;
    displayString("Text");
    delay(250);

    GETFROMEEPROM();
    if(IncomingMessIndex == 0 | IncomingMessIndex > 27)
    {
      MessagePointer = 0;
      SUBSTATE =3;
      char Str2[] = "SpikenzieLabs";
      for(int i =0; i <= sizeof(Str2); i ++)                            // Show default Scrolling message
      {
        IncomingMessage[i] = Str2[i];
        IncomingMessIndex = i;
      }

    }
    else
    {
      MessagePointer= 0;
      SUBSTATE = 3; 
      SleepEnable = true;
    }

    break;

  case 1:
    if(NextSUBStateRequest)
    {
      ResetScrollMessage();

      SUBSTATE = 2;
      NextSUBStateRequest = false;
      power_usart0_enable();
      Serial.begin(57600);
      SleepEnable = false;
    }

    if(NextStateRequest)
    {
      SUBSTATE =99;
      NextStateRequest = false;
    }  
    break; 

    //    break;  

  case 2:                                                          // Receive Serial

    clearmatrix();
    // LED MATRIX
    //
    // Port C: C0 to C3 set to high. Columns 17 to 20 of LED matrix - Cathode connection
    PORTC = (PORTC & B11110000) | B00001111;      
    // Port B: Unselect the MUX chip
    PORTB = (1<<PORTB7);
    // Port B: Set all the ROWs to high: High on both cathode and annode = no current ?
    PORTB = PORTB | B01111111;     // Could be PORTB =B11111111;
    Timer1.detachInterrupt();

shortloop:

    if(Serial.available()>0)
    {
      MessageRead = Serial.read();

      if(IncomingMessIndex<24)
      {
        IncomingMessage[IncomingMessIndex] = MessageRead;
        //  IncomingMessIndex = IncomingMessIndex + 1;
        IncomingMessIndex++;
        //   IncomingMessage[IncomingMessIndex] = '\0';
        Serial.print(MessageRead);
      } 
    }

    bval = !digitalRead(SETBUTTON);
    if(!bval)
    {
      goto shortloop;
    }
    else

    {
      Timer1.attachInterrupt(LEDupdateTWO);

      while(bval)
      {
        bval = !digitalRead(SETBUTTON);
      }
      delay(100);
    }

    if(IncomingMessIndex == 0)
    {
      SUBSTATE = 99;
    }
    else
    {
      SUBSTATE = 3; 
      FILLEEPROM();
      OptionModeFlag = false;
      

    }
    SleepTimer = millis();
    SleepEnable = true;
    Serial.end();
    power_usart0_disable();

    break;

  case 3:

    for(int i = 0;i<=IncomingMessIndex-1;i ++)
    {
      TEXT = IncomingMessage[i]-32;
      for(int y =0;y<5;y++)
      {    
        Message[MessagePointer] = LETTERS[TEXT][y];
        MessagePointer = MessagePointer +1;
      }

      Message[MessagePointer] = 0;                                          // One space between words
      MessagePointer = MessagePointer +1;
      IncomingMax = MessagePointer;
    }


    for(int i = 0;i<20;i ++)                                             // 20 spaces between phrases
    {
      Message[MessagePointer] = 0;
      MessagePointer = MessagePointer +1;
      IncomingMax = MessagePointer;
    }

    SUBSTATE = 4;
    ScrollLoops = 3;
    SleepEnable = true;


    break;    

    // ==================================================================================================

  case 4:     

    scrollCounter = scrollCounter +1;
    if(scrollCounter>scrollSpeed)
    {
      
      if(ScrollLoops > 0)
      {
        SleepTimer = millis();
      }
      
      IncomingIndex = StartWindow;
      for(int i=0;i<20;i++)
      {
        LEDMAT[i] = Message[IncomingIndex];
        IncomingIndex = IncomingIndex + 1;
        if(IncomingIndex>IncomingMax)
        {
          IncomingIndex = 0;                            // Rolled over end of message
    //      ScrollLoops = ScrollLoops - 1;                  // Used to extend number of messages to scroll before sleep mode reqctivated
        }
      }
      StartWindow = StartWindow + 1;
      if(StartWindow>IncomingMax)
      {
        StartWindow = 0;
        ScrollLoops = ScrollLoops - 1;                  // Used to extend number of messages to scroll before sleep mode reqctivated
      }

      scrollCounter= 0;
    }

    if(NextSUBStateRequest)
    {
      scrollSpeed = scrollSpeed + 50;
      if(scrollSpeed>400)
      {
        scrollSpeed = 100;
      }
      scrollCounter= 0;
      NextSUBStateRequest = false;
    }

    if(NextStateRequest)
    {
      SUBSTATE =99;
      NextStateRequest = false;
    }  

    if(OptionModeFlag)
    {
      SUBSTATE =1;
      displayString("NEW?");
      delay(250);
    }
    break;  

  case 99:              // Exit Stop Watch Function

    Serial.end();
    power_usart0_disable();

    NextState();
    clearmatrix();

    SleepEnable = true;

    break;  
  }
}

void ResetScrollMessage()
{

  IncomingIndex = 0;
  IncomingMax = 0;
  MessagePointer = 0;
  IncomingMessIndex =0;
  StartWindow = 0;
  IncomingLoaded = 0;
  scrollCounter = 0;

  for(int i =0;i<275;i++)
  {    
    Message[i] = 0;
  }

  for(int i =0;i<24;i++)
  {    
    IncomingMessage[i] = 0;
  }
}

//*******************************************************************************************************************
// 								                           Graphic Animation
//*******************************************************************************************************************
void graphican()
{

  int temp =0;
  //  int rand = 0;
  switch (SUBSTATE) 
  {  

  case 0:
    SUBSTATE =1;
    scrollCounter = 0;
    scrollSpeed = 200;
    //  soundeffect = false;
    y = 3;
    target =  1;
    targdist = 0;
    displayString("Worm");
    delay(250);
    break;  

  case 1:      

    if(scrollCounter>scrollSpeed)
    {

      c= c +1;
      if(c>19)
      {
        c = 0;
      }
      // --     


      if(NextSUBStateRequest)
      {
        target = target +1;
        if(target> 3)
        {
          target = 0;
          // targdir = false;
          targdist = 0;

        }
        NextSUBStateRequest = false;
      }


      if(targdir)
      {                                                        // Going up

        if(targdist == target)
        {
          targdir = !targdir;
          targdist = - target;
          if(soundeffect)
          {
            beepsound(4000,10);
          }

        }
        else
        {
          targdist = targdist +1;
          y = 3 - targdist;
        }
      }
      else
      {                                                       // Going Down

        if(targdist == target)
        {
          targdir = !targdir;
          targdist = - target;
          if(soundeffect)
          {
            beepsound(5000,10);
          }
        }
        else
        {
          targdist = targdist +1;
          y = 3 + targdist;
        }
      }


      bitSet(temp, y);


      // --         
      LEDMAT[c] = temp;
      if((c-wormlenght)<0)
      {
        LEDMAT[19-((wormlenght-1)-c)] = 0;
      }
      else
      {
        LEDMAT[c-wormlenght] = 0;
      }
      scrollCounter = 0;
    }



    if(NextStateRequest)
    {
      SUBSTATE =99;
      NextStateRequest = false;
    }  

    if(OptionModeFlag)
    {
      soundeffect = !soundeffect;
    }   

    scrollCounter = scrollCounter +1;
    break;



  case 99:              // Exit Graphic Function

    NextState();
    clearmatrix();

    break;  

  }
}

//*******************************************************************************************************************
// 								                                        LED tester
//*******************************************************************************************************************

void lamptest()
{

  int lamptestspeed = 250;
  clearmatrix();
  bval = !digitalRead(SETBUTTON);
  if(bval)
  {
    do
    {
      //    clearmatrix();
      for(int i = 0; i<20;i++)
      {
        for(int y = 0; y<8;y++)
        {
          bitSet(LEDMAT[i],y);
          delay(lamptestspeed / 10);
          bval = !digitalRead(SETBUTTON);
          if(bval)
          {
            lamptestspeed = lamptestspeed -1;
            if(lamptestspeed== 0)
            {
              lamptestspeed = 250;
            }
          }
        }

        bval = !digitalRead(MODEBUTTON);
        if(bval)
        {
          break;
        }

        delay(lamptestspeed);
        LEDMAT[i] = 0;
        delay(lamptestspeed / 5);
      }
      bval = !digitalRead(MODEBUTTON);
    }
    while(!bval);
    }
  }

  //*******************************************************************************************************************
  //                                                                                         Read Message from EEPROM			
  //*******************************************************************************************************************
  void GETFROMEEPROM() 
  {
    int EEPadd;
    IncomingMessIndex = EEPROM.read(0);
    for(int EEPadd=1; EEPadd < 26; EEPadd++)
    {
      IncomingMessage[EEPadd-1] = EEPROM.read(EEPadd);
    }
  }

//*******************************************************************************************************************
//                                                                                   Save changed Message to EEPROM			
//*******************************************************************************************************************
void FILLEEPROM()                                                      // Normally only run once if EEPROM is clear
{
  int EEPadd;
  EEPROM.write(0, IncomingMessIndex);                                  // Holds the message lenght

  for(int EEPadd=1; EEPadd < IncomingMessIndex+1; EEPadd++)
  {

    EEPROM.write(EEPadd, IncomingMessage[EEPadd-1]);
  }
  // EEPROM.write(25, 1);                                      // 1 is just a number we selected to show EEPROM was writen
}


//*******************************************************************************************************************
// 								                      Output 4 Characters to Display
//*******************************************************************************************************************
void displayString(char outText[])
{
  int  cindex = 0;
//  for (int i = 0; i < sizeof(Str1) - 1; i++){
    for (int i = 0; i < 4; i++){
    for(int y =0;y<5;y++)
    { 
      TEXT = outText[i]-32;   
      LEDMAT[cindex] = LETTERS[TEXT][y]; 
      cindex = cindex +1;     
    }
  }  
}

//*******************************************************************************************************************
// 								                  Output Custom Graphic to Display
//*******************************************************************************************************************
void displayGraphic(int index, int pos, int len)
{
    for(int y =0;y<len;y++)
    {    
      LEDMAT[pos] = GRAPHIC[index][y]; 
      pos = pos +1;    
    }  
}

//*******************************************************************************************************************
// 								                                   Display Day Text
//*******************************************************************************************************************
void displayStringDay(int day)
{
  int  cindex = 0;
  if(blinkON)
  {
    for (int i = 0; i < 3; i++){
      LEDMAT[cindex] = 0;
      cindex = cindex +1;
      for(int y =0;y<5;y++)
      { 
        TEXT = days[day][i]-32;   
        LEDMAT[cindex] = LETTERS[TEXT][y]; 
        cindex = cindex +1;     
      }
    } 
    LEDMAT[cindex] = 0;
    LEDMAT[cindex+1] = 0;
  }
  else
  {
    clearmatrix();
  }
}

//*******************************************************************************************************************
// 								                                 Display Month Text
//*******************************************************************************************************************
void displayMonth(int code)
{
  int  cindex = 0;
  if(blinkON)
  {
    for (int i = 0; i < 3; i++){
      LEDMAT[cindex] = 0;
      cindex = cindex +1;
      for(int y =0;y<5;y++)
      { 
        TEXT = months[code][i]-32;   
        LEDMAT[cindex] = LETTERS[TEXT][y]; 
        cindex = cindex +1;     
      }
    } 
    LEDMAT[cindex] = 0;
    LEDMAT[cindex+1] = 0;
  }
  else
  {
    clearmatrix();
  }
}

//*******************************************************************************************************************
// 								                                       Display Date
//*******************************************************************************************************************
void displayDate()
{
  int y =0;
  int i =0;
  // clearmatrix();
  if(blinkON)
  {
    for( i = 0; i <5; i++)
    {
      LEDMAT[i] = 0;
    }

    for( i = 5; i <10; i++)
    {
      LEDMAT[i] = LETTERS[DateTens+digitoffset][y];
      y=y+1;
    }

    y=0;

    for( i = 10; i <15; i++)
    {
      LEDMAT[i] = LETTERS[DateOnes+digitoffset][y];
      y=y+1;
    }

    for( i = 15; i <20; i++)
    {
      LEDMAT[i] = 0;
    }
  }
  else
  {
    clearmatrix();
  }
}

//*******************************************************************************************************************
// 								                                       Display Year
//*******************************************************************************************************************
void displayYear()
{
  int y =0;
  int i =0;
  // clearmatrix();
  for( i = 0; i <5; i++)
  {
    LEDMAT[i] = LETTERS[DateTens+digitoffset][y];
    y=y+1;
  }

  for( i = 5; i <10; i++)
  {
    LEDMAT[i] = LETTERS[DateTens+digitoffset][y];
    y=y+1;
  }

  y=0;

  for( i = 10; i <15; i++)
  {
    LEDMAT[i] = LETTERS[DateOnes+digitoffset][y];
    y=y+1;
  }

  for( i = 15; i <20; i++)
  {
    LEDMAT[i] = LETTERS[DateTens+digitoffset][y];
    y=y+1;
  }
}

//void fillmatrix()                                                    // Fill Matrix with some text
//{
// for(int i =0;i<20;i=i+5)
//  {  
//    for(int y =0;y<5;y++)
//    {      
//      LEDMAT[i+y] = LETTERS[TEXT][y];      
//    }
//    TEXT = TEXT +1;
//  }
//}

// ----------

/*
void scrollleft()                                                    // Shift all dots one space left
{
  int i= 0;
  uint8_t temp =0;
  temp = LEDMAT[0];
  for(int i =0;i<20;i++)
  {
    LEDMAT[i] = LEDMAT[i+1];
  }
  LEDMAT[19] = temp;
}
*/

/*
void scrollRight()                                                    // Shift all dots one space left
{
  int i= 0;
  uint8_t temp =0;
  temp = LEDMAT[19];
  for(int i =19;i>0;i--)
  {
    LEDMAT[i] = LEDMAT[i-1];
  }
  LEDMAT[0] = temp;
}
*/

//*******************************************************************************************************************
// 								                                Clear LED Matrix
//*******************************************************************************************************************

void clearmatrix()
{
  for(int i =0;i<20;i++)
  {
    LEDMAT[i] = 0;
  }
}


//*******************************************************************************************************************
// 								               Output single character to display
//*******************************************************************************************************************

void filldigit(int dig, int index)        // Where dig is 1 to 4 and index is position of character 
{
  for(int y =0;y<5;y++)
  {      
    LEDMAT[((dig-1)*5)+y] = LETTERS[index][y];      
  }    
}

//*******************************************************************************************************************
// 								            Fill LED Matrix with formatted time
//*******************************************************************************************************************
// Fills diplay with formated time
// Depending on postion of "1"s spacing is adjusted beween it and next digit
// Blinks if it settng mode
// displays AM/PM dot and Alarm on dot

void writeTime(uint8_t dig1, uint8_t dig2, uint8_t dig3, uint8_t dig4)         
{
  int currentdig = 4;
  int y = 0;
  int i = 18;
  LEDMAT[19] = 0;
//  LEDMAT[0] = 0;
  
  for( y =5;y>1;y--)
  {      
    if(blinkON && (blinkMin))
    {
      LEDMAT[i] = LETTERS[0][y-2];                            // blank space
    }
    else
    {
      LEDMAT[i] = LETTERS[dig4+digitoffset][y-2]; 
    }
    i = i -1;    
  }    
  //  }

  if(dig3 == 1)                                              // Special case of #1, smaller kerning
  {
    LEDMAT[i] = 0;
    if(blinkON && (blinkMin))
    {
      LEDMAT[i-1] = 0;                                        // blank space
    }
    else
    {
      LEDMAT[i-1] = 62; //LETTERS[dig3+digitoffset][3];
    }
    LEDMAT[i-2] = 0;
    i = i -3;
    currentdig = 2;
  }
  else
  {
    for( y =5;y>1;y--)
    {      
      if(blinkON && (blinkMin))
      {
        LEDMAT[i] = LETTERS[0][y-2];                          // blank space
      }
      else
      {
        LEDMAT[i] = LETTERS[dig3+digitoffset][y-2]; 
      }
      i = i -1;    
    }    
    currentdig = 2;
  }
  // ADD Colon Here - S ---------------------------------
  if((dig2 == 1) || (dig1 == 3)) 
  {
    LEDMAT[i] = 0;

    LEDMAT[i-1] = 20; //LETTERS[26][3];                        // the " : "

    LEDMAT[i-2] = 0;
    i=i-3;     
  }
  else
  {
    LEDMAT[i] = 20; //LETTERS[26][3];                          // the " :"
    LEDMAT[i-1] = 0;
    i=i-2;    
  }
  // ADD Colon Here - E ---------------------------------    

  if(dig2 == 1)                                                // Special case of #1, smaller kerning
  {
    LEDMAT[i] = 0;
    if(blinkON && (blinkHour))
    {
      LEDMAT[i-1] = 0;                                         // blank space
    }
    else
    {
      LEDMAT[i-1] = 62; //LETTERS[dig2+digitoffset][3];
    }
    LEDMAT[i-2] = 0;
    i = i -3;
    currentdig = 1;
  }
  else
  {
    for( y =5;y>1;y--)
    {      
      if(blinkON && (blinkHour))
      {
        LEDMAT[i] = LETTERS[0][y-2];                          // blank space
      }
      else
      {
        LEDMAT[i] = LETTERS[dig2+digitoffset][y-2]; 
      }
      i = i -1;    
    }    
    currentdig = 1;
  }

  if(dig1 == 1)                                              // Special case of #1, smaller kerning
  {
    LEDMAT[i] = 0;
    if(blinkON && (blinkHour))
    {
      LEDMAT[i-1] = 0;                                       // blank space
    }
    else
    {
      LEDMAT[i-1] = 62; // LETTERS[dig1+digitoffset][3];
    }
    LEDMAT[i-2] = 0;
    i = i -2;
  }
  else
  {
    for( y =5;y>1;y--)
    {                  
      if(blinkON && (blinkHour))
      {
        LEDMAT[i] = LETTERS[0][y-2];                          // blank space
      }
      else
      {
        LEDMAT[i] = LETTERS[dig1+digitoffset][y-2]; 
      }
      i = i -1;    
    }    
   // i = 0;
  }
  
//  for(y= i+1; y>0;y--)                                        // Clear any remaining columns
    for(y= i+1; y>1;y--)                                        // Clear any remaining columns, but not leftmost
  {
    LEDMAT[y-1] = 0;
  }

//  LEDMAT[0] = 0;
  AMPMALARMDOTS = 0;

  if(ALARMON && (STATE == 1))                                 // Alarm dot (top left) Do not display while setting alarm
  {
//    bitSet(LEDMAT[0],6);
   bitSet(AMPMALARMDOTS,6);
  }
  
   if(PM_NotAM_flag && (STATE == 1 | STATE == 2) && TH_Not24_flag)     // AM / PM dot (bottom left) (Display or Set Time)
  {
//    bitSet(LEDMAT[0],0);
   bitSet(AMPMALARMDOTS,0);
  } 
  
     if(A_PM_NotAM_flag && (STATE == 3) && TH_Not24_flag)              // AM / PM dot (bottom left) (Set Alarm Time)
  {
//    bitSet(LEDMAT[0],0);
   bitSet(AMPMALARMDOTS,0);
  } 
  
  LEDMAT[0] = AMPMALARMDOTS;
  
}
