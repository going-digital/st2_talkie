//*******************************************************************************************************************
// 								                                 Setup
//*******************************************************************************************************************
void setup() 
{

  // Rows Digital pin 0 to 7 
  //  pinMode(0, OUTPUT);                                  // Not used
  //  pinMode(1, OUTPUT);                                  // Not used

  // User interface Button Pins 
  /*
  digitalWrite(2, HIGH);                                   // Write a high to pin, acts as weak pull-up
  digitalWrite(3, HIGH);
  pinMode(2, INPUT);
  pinMode(3, INPUT);  
  */
  
  #if ARDUINO >= 101
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  digitalWrite(2, HIGH);                                   // Write a high to pin, acts as weak pull-up
  digitalWrite(3, HIGH);  
  #else
  digitalWrite(2, HIGH);                                   // Write a high to pin, acts as weak pull-up
  digitalWrite(3, HIGH);
  pinMode(2, INPUT);
  pinMode(3, INPUT);  
  #endif

  // Column address bits 4 to 16 decode    
  pinMode(4, OUTPUT);                                      // DeMux A
  pinMode(5, OUTPUT);                                      // DeMux B
  pinMode(6, OUTPUT);                                      // DeMux C
  pinMode(7, OUTPUT);                                      // DeMux D 

  /*
  // 
   pinMode(8, OUTPUT);                                      // ROW 1
   pinMode(9, OUTPUT);                                      // ROW 2
   pinMode(10, OUTPUT);                                     // ROW 3
   pinMode(11, OUTPUT);                                     // ROW 4
   pinMode(12, OUTPUT);                                     // ROW 5
   pinMode(13, OUTPUT);                                     // ROW 6
   
   // Make these pins outputs (Was the crystal Pins)   B6 = Row 7, B7 = demux select
   DDRB = (1<<DDB7)|(1<<DDB6);  
   */

  //test with
  DDRB = (1<<DDB7)|(1<<DDB6)|(1<<DDB5)|(1<<DDB4)|(1<<DDB3)|(1<<DDB2)|(1<<DDB1)|(1<<DDB0); 


  // Make these pins outputs (analog 0 to 3)
  DDRC = DDRC | 1 << PORTC0;                                      // Column 17
  DDRC = DDRC | 1 << PORTC1;                                      // Column 18
  DDRC = DDRC | 1 << PORTC2;                                      // Column 19
  DDRC = DDRC | 1 << PORTC3;                                      // Column 20




  // attachInterrupt(0, MinuteUP, FALLING);
  // attachInterrupt(1, MinuteDOWN, FALLING);

  // Turn one Interupts, used to update the displayed LED matrix
  Timer1.initialize(100);                                          // was 200
  //   Timer1.attachInterrupt(LEDupdate);
  Timer1.attachInterrupt(LEDupdateTWO); 


  // I2C Inits
  Wire.begin();
  
  
  // Power Reduction - S
  power_adc_disable();
  power_spi_disable();
  power_usart0_disable();
//  
//  power_timer0_disable();                                       // Seems required (for delay ?)
//  power_timer2_disable();                                       // Seems required for tone (crashes without)

  
  wdt_disable();
 
 //Special 
 //power_all_disable();
 //power_timer1_disable(); 
 
  // Power Reduction - E

  // Program specific inits
  //  fillmatrix();
  delay(300);
  bval = !digitalRead(SETBUTTON);
  if(bval)
  {
  lamptest();
  }
  
  displayString("STII");
  delay(1500);
  clearmatrix();
  
   SetStartTime();                                                  // Basic start time of 12:00 PM
   SetAlarmTime(); // for testing
   EnableAlarm1(false); // for testing
   
   SleepTimer = millis();

}
