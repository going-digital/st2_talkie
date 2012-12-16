//*******************************************************************************************************************
// 								                                 Check Time
//*******************************************************************************************************************
void checktime()
{
  uint8_t temp =0;

  I2C_RX(RTCDS1337,RTC_SEC);
  SecOnes = i2cData & B00001111;                   // 

  SecTens = i2cData & B01110000;                   // 
  SecTens = SecTens >> 4;


  I2C_RX(RTCDS1337,RTC_MIN);
  MinOnes = i2cData & B00001111;                   // 

  MinTens = i2cData & B01110000;                   // 
  MinTens = MinTens >> 4;

  I2C_RX(RTCDS1337,RTC_HOUR);
  HourOnes = i2cData & B00001111;                   // 

  TH_Not24_flag = bitRead(i2cData, 6);                   // False on RTC when 24 mode selected
  PM_NotAM_flag = bitRead(i2cData, 5);

  if(TH_Not24_flag == true)
  {
    HourTens = i2cData & B00010000;                   // 
    HourTens = HourTens >> 4;

  }
  else
  {
    HourTens = i2cData & B00110000;                   // 
    HourTens = HourTens >> 4;
  }
}

//*******************************************************************************************************************
// 								                                 Check Date
//*******************************************************************************************************************
void checkDate()
{

  int temp = 0;
  I2C_RX(RTCDS1337,RTC_DAY);
  Days = i2cData & B00000111;                   // 

  I2C_RX(RTCDS1337,RTC_MONTH);
  MonthCode = i2cData & B00001111;                   // 

  temp = (i2cData & B00010000) >> 4;
  if(temp)
  {
    MonthCode = MonthCode +10;                           // Convert BCD month into interger month
  }

  I2C_RX(RTCDS1337,RTC_DATE);
  DateOnes = i2cData & B00001111;                   // 
  DateTens = (i2cData & B00110000) >> 4;

}


//*******************************************************************************************************************
// 								                                 SET Time - NEW
//*******************************************************************************************************************

void settimeNEW(uint8_t setselect)                     // both min digits or both hour digits (advance one at a time)
{
  uint8_t temp =0;
  switch(setselect)
  {

  case 1:
    MinOnes = MinOnes +1;
    if(MinOnes >9)
    {
      MinOnes = 0;

      MinTens = MinTens +1;
      if(MinTens >5)
      {
        MinTens = 0;
      }

      //   temp = (MinTens << 4) + MinOnes;
      //   I2C_TX(RTCDS1337,RTC_MIN,temp);
    }

    temp = (MinTens << 4) + MinOnes;
    I2C_TX(RTCDS1337,RTC_MIN,temp);
    break;


    // -----------------------------------------------

  case 2:
    HourOnes = HourOnes +1;
    
    if(TH_Not24_flag)
//                                                                    12 hours mode increment    
    {

    if(HourOnes >9 )
    {
      HourOnes = 0;
      HourTens = 1;
    }
    
   if((HourOnes ==2) &&  (HourTens == 1))
    {
      PM_NotAM_flag = !PM_NotAM_flag;
    }
    
    if((HourOnes >2) &&  (HourTens == 1))
    {
//      PM_NotAM_flag = !PM_NotAM_flag;
      HourTens = 0;
      HourOnes = 1;      
    }
    
    }
    else
//                                                                    24 hours mode increment - S    
    {

    if((HourOnes >9) && (HourTens < 2))
    {
      HourOnes = 0;
      HourTens = HourTens +1;
    }
    
     if((HourTens ==2) && (HourOnes == 4))
    {
      HourOnes = 0;
      HourTens = 0;    
    }
    }
//                                                                    24 hours mode increment - E
    
    temp = (HourTens << 4) + HourOnes;
    if(TH_Not24_flag)
    {
      bitWrite(temp, 5, PM_NotAM_flag);
    }

    bitWrite(temp, 6, TH_Not24_flag);
    I2C_TX(RTCDS1337,RTC_HOUR,temp);
    break;    

  case 3:
    Days = Days +1 ;
    if(Days>7)
    {
      Days = 1;
    }
    temp = Days & B00000111;                   // 
    I2C_TX(RTCDS1337,RTC_DAY,temp);
    break;

  case 4:
    temp = 0;
    MonthCode = MonthCode +1 ;  
    if(MonthCode >12)
    {
      MonthCode = 1;
    } 
    if(MonthCode>9)
    {
      temp = MonthCode - 10;
      // MonthCode = MonthCode & B00001111;
      bitSet(temp, 4);                                      // Convert int to BCD
    }
    else
    {
      temp = MonthCode & B00001111;
    }

    I2C_TX(RTCDS1337,RTC_MONTH,temp);
    break;

  case 5:    // Date

    //   I2C_RX(RTCDS1337,RTC_DATE);
    //   DateOnes = i2cData & B00001111;                   
    //   DateTens = (i2cData & B00110000) >> 4;    
    DateOnes = DateOnes + 1;
    if((DateTens == 3) && (DateOnes > 1))
    {
      DateOnes = 1;
      DateTens =0;
    }
    else
    {
      if(DateOnes>9)
      {
        DateOnes = 0;
        DateTens = DateTens +1;
      }
    }
    temp = (DateOnes & B00001111) | ((DateTens << 4) & B00110000);
    I2C_TX(RTCDS1337,RTC_DATE,temp);
    break;  

  case 6:     // year
    break;
  }
}

//*******************************************************************************************************************
// 12:00								                                 Start Time
//*******************************************************************************************************************

void SetStartTime()
{
  uint8_t temp =0;

  HourTens = 1;
  HourOnes = 2;
  temp = (HourTens << 4) + HourOnes;
  bitWrite(temp, 5, PM_NotAM_flag);
  bitWrite(temp, 6, TH_Not24_flag);

  I2C_TX(RTCDS1337,RTC_HOUR,temp);  

  MinTens = 0;
  MinOnes = 0;
  temp = (MinTens << 4) + MinOnes;
  I2C_TX(RTCDS1337,RTC_MIN,temp);
}

//*******************************************************************************************************************
// 								                                 SET Alarm
//*******************************************************************************************************************

void SetAlarmTime()                              // Just for testing set to 12:01 PM
{
  uint8_t temp =0;

  HourTens = 1;
  HourOnes = 2;
  temp = (HourTens << 4) + HourOnes;
  bitWrite(temp, 5, A_PM_NotAM_flag);
  bitWrite(temp, 6, A_TH_Not24_flag);

  I2C_TX(RTCDS1337,RTC_ALARM1HOUR,temp);   

  MinTens = 0;
  MinOnes = 1;
  temp = (MinTens << 4) + MinOnes;
  I2C_TX(RTCDS1337,RTC_ALARM1MIN,temp);


}

//*******************************************************************************************************************
// 								                                 Check Alarm
//*******************************************************************************************************************

void CheckAlarm()
{
  uint8_t temp =0;
  I2C_RX(RTCDS1337,RTCSTATUS);
  ALARM1FLAG = bitRead(i2cData, 0);

  if(ALARM1FLAG)
  {
    temp =i2cData;
    bitClear(temp, 0);
    I2C_TX(RTCDS1337,RTCSTATUS,temp);   
  }

}

//*******************************************************************************************************************
// 								                                 Enable Alarm
//*******************************************************************************************************************

void EnableAlarm1(boolean onoff)                                            // Trigger on Hours & Minutes Match
{
  uint8_t temp =0;

  // Adjust for Hours - Minutes Trigger -S
  I2C_RX(RTCDS1337,RTC_ALARM1SEC);
  temp =i2cData;
  bitClear(temp, 7);
  I2C_TX(RTCDS1337,RTC_ALARM1SEC,temp);

  I2C_RX(RTCDS1337,RTC_ALARM1MIN);
  temp =i2cData;
  bitClear(temp, 7);
  I2C_TX(RTCDS1337,RTC_ALARM1MIN,temp);

  I2C_RX(RTCDS1337,RTC_ALARM1HOUR);
  temp =i2cData;
  bitClear(temp, 7);
  I2C_TX(RTCDS1337,RTC_ALARM1HOUR,temp); 

  I2C_RX(RTCDS1337,RTC_ALARM1DATE);
  temp =i2cData;
  bitSet(temp, 7);
  I2C_TX(RTCDS1337,RTC_ALARM1DATE,temp);  
  // Adjust for Hours - Minutes Trigger -E

  I2C_RX(RTCDS1337,RTCCONT);                                  // Enable Alarm Pin on RTC
  temp =i2cData;

  if(onoff)
  {
    bitSet(temp, 0);    
  }
  else
  {
    bitClear(temp, 0);
  }
  I2C_TX(RTCDS1337,RTCCONT,temp);

  I2C_RX(RTCDS1337,RTCSTATUS);                                // Clear Alarm RTC internal Alarm Flag
  temp =i2cData;
  bitClear(temp, 0);
  I2C_TX(RTCDS1337,RTCSTATUS,temp);
}
//*******************************************************************************************************************
// 								                                 SET ALARM TIME
//*******************************************************************************************************************

void setAlarm(uint8_t setselect)                                // both min digits or both hour digits (advance one at a time)
{
  uint8_t temp =0;
  switch(setselect)
  {

  case 1:
    AMinOnes = AMinOnes +1;
    if(AMinOnes >9)
    {
      AMinOnes = 0;

      AMinTens = AMinTens +1;
      if(AMinTens >5)
      {
        AMinTens = 0;
      }
    }

    temp = (AMinTens << 4) + AMinOnes;
    I2C_TX(RTCDS1337,RTC_ALARM1MIN,temp);
    break;


  case 2:
    AHourOnes = AHourOnes +1;
    
// -----------*
    if(A_TH_Not24_flag)
//                                                                    12 hours mode increment    
    {

    if(AHourOnes >9 )
    {
      AHourOnes = 0;
      AHourTens = 1;
    }
    
   if((AHourOnes ==2) &&  (AHourTens == 1))
    {
      A_PM_NotAM_flag = !A_PM_NotAM_flag;
    }
    
    if((AHourOnes >2) &&  (AHourTens == 1))
    {
//      PM_NotAM_flag = !PM_NotAM_flag;
      AHourTens = 0;
      AHourOnes = 1;      
    }
    
    }
    else
//                                                                    24 hours mode increment - S    
    {

    if((AHourOnes >9) && (AHourTens < 2))
    {
      AHourOnes = 0;
      AHourTens = AHourTens +1;
    }
    
     if((AHourTens ==2) && (AHourOnes == 4))
    {
      AHourOnes = 0;
      AHourTens = 0;    
    }
    }
//                                                                    24 hours mode increment - E
// -----------*
    
/*    
    if(AHourOnes >9)
    {
      AHourOnes = 0;
      AHourTens = AHourTens +1;
      if((AHourTens >1) && (A_TH_Not24_flag))
      {
        AHourTens = 0;
      }
      else
      {
        if(AHourTens >2)
        {
          AHourTens = 0;
        }
      }
    }
*/
    temp = (AHourTens << 4) + AHourOnes;
    if(A_TH_Not24_flag)
    {
      bitWrite(temp, 5, A_PM_NotAM_flag);
    }

    bitWrite(temp, 6, A_TH_Not24_flag);
    I2C_TX(RTCDS1337,RTC_ALARM1HOUR,temp);
    break;
  }

}

//*******************************************************************************************************************
// 								           Toggle Twelve and  Twenty Four hour time
//*******************************************************************************************************************

void TwelveTwentyFourConvert()                                    
{

  int temphours = 0;
  int temp = 0;

  I2C_RX(RTCDS1337,RTC_HOUR);
  HourOnes = i2cData & B00001111;                   // 

  //  TH_Not24_flag = bitRead(i2cData, 6);                   // False on RTC when 24 mode selected
  //  PM_NotAM_flag = bitRead(i2cData, 5);

  if(TH_Not24_flag)
  {
    HourTens = i2cData & B00010000;                   // 
    HourTens = HourTens >> 4;

  }
  else
  {
    HourTens = i2cData & B00110000;                   // 
    HourTens = HourTens >> 4;
  }

  temphours = HourOnes + (HourTens*10);             // 12 .... 1.2.3...12 or 0 ..1.2.3. ...23

  if(TH_Not24_flag != NewTimeFormate)
  {
    if(NewTimeFormate)                                    // NewTimeFormate is same formate as TH_Not24_flag where H is 12 and LOW is 24
    { 
      // ---------------- 24 -> 12    
      // Convert into 12 hour clock
      if(temphours >= 12)
      {
        PM_NotAM_flag = true;                             // it is in the PM  
        if(temphours> 12)
        {
          temphours = temphours - 12;                     // Convert from 13:00 .... 23:00 to 1:00 ... 11:00 [Go from 23:59 / 13:00 to 12:00 to 1:00] ?
        }
        else
        {
          temphours = temphours;                          // do nothing it is 12:00
        }
      }
      else
      {
        PM_NotAM_flag = false;                            // it is in the AM - No other conversion needed      
      }
      if(temphours == 0)
     {
       temphours = 12;
     } 
    }
    else
      // ---------------- 12 -> 24                        // Convert into 24 hour clock
    { 
       if((PM_NotAM_flag == false) && (temphours == 12))  // AM only check for 00 hours
        {
          temphours = 0;
        }
       if(PM_NotAM_flag == true)
        {                                                  // PM conversion
                 if(temphours != 12)                       // Leave 12 as 12 in 24h time
                 {
                   temphours = temphours + 12;
                 }
        }

    }
    
    // Common finish conversion section
    TH_Not24_flag = NewTimeFormate;
    HourTens = temphours / 10;
    HourOnes = temphours % 10;
    
    // ---
    temp = (HourTens << 4) + HourOnes;
    if(TH_Not24_flag)
    {
      bitWrite(temp, 5, PM_NotAM_flag);
    }

    bitWrite(temp, 6, TH_Not24_flag);
    I2C_TX(RTCDS1337,RTC_HOUR,temp);
    
    // ---
    
  }


}


