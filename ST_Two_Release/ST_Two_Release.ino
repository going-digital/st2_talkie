

//  *****************************************************************************************************************
//  *                                                                                                               *
//  *                                         SpikenzieLabs.com                                                     *
//  *                                                                                                               *
//  *                                                ST2                                                            *
//  *                                                                                                               *
//  *****************************************************************************************************************
//
// BY: MARK DEMERS 
// May 2012
// VERSION 1.1
//
// Brief:
// Sketch used in the Solder: Time II Watch Kit, more info and build instructions at http://www.spikenzielabs.com/st2
// 
//
// LEGAL:
// This code is provided as is. No guaranties or warranties are given in any form. It is your responsibilty to 
// determine this codes suitability for your application.
// 
// Changes:
// July 23 2012
// Reduced fliker in some lighting conditions.
// 1. Moved Time and Date request during time and date display to top of Display Time routine.
// 2. Reduced frequency that the Time and Date is requested from the RTC (once every 2000 loops).
// 
// Improved brightness consistancy on all columns.
// 1. Added delay to "on time" of all the pixels in the first 16 columns that are driven by the Demultiplexer.
// 2. Adjusted delays to for Scrolling Text and Worm animation to balance slower overall refresh from added brightness delay.
//
// Fixed bug introduced by Arduino 1.0.1
// 1. Added conditional compile in Setup for the button pins. Arduino > 1.0 uses INPUT_PULLUP vs Digitalwrite HIGH.
// 
//

#include <Wire.h>
#include <EEPROM.h>

#include <TimerOne.h>

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

// Worm animation
int c =0;
int y = 3;
int target = 3;
int targdist =0;
boolean targdir = true;
int wormlenght = 15;
boolean soundeffect = false;

// int  i  =0;
// int  i2  =0;
// int  vite = 2;
// uint8_t incro = 0;
// uint8_t column = 0;
uint8_t TEXT = 65;
uint8_t i2cData = 0;
// int nextcounter = 0;

int STATE = 0;
int SUBSTATE = 0;
int MAXSTATE = 6;
boolean NextStateRequest = false;
boolean NextSUBStateRequest = false;
boolean JustWokeUpFlag = false;
boolean JustWokeUpFlag2= false;
boolean OptionModeFlag = false;

int ROWBITINDEX = 0;
int scrollCounter =0;
int ScrollLoops = 3;
int scrollSpeed = 100;                                    // was 1200
int blinkCounter = 0;
boolean blinkFlag = false;
boolean blinkON = true;
boolean blinkHour = false;
boolean blinkMin = false;
#define blinkTime  500                                   // was 1000

boolean displayFLAG = true;

unsigned long SleepTimer;
unsigned long currentMillis;
unsigned long SleepLimit = 6000;
boolean SleepEnable = true;
int UpdateTime = 0;

#define BUTTON1 2
#define MODEBUTTON 2
#define BUTTON2 3
#define SETBUTTON 3
boolean  bval = false;

//char Str1[] = "Hi";
char IncomingMessage[24];
char MessageRead;
//uint8_t INBYTE;
uint8_t Message[275];
int IncomingIndex = 0;
int IncomingMessIndex =0;
int IncomingMax = 0;
int MessagePointer = 0;
int StartWindow = 0;
int IncomingLoaded =0;


char days[7][4] = {
  "Sun","Mon","Tue","Wed","Thr","Fri","Sat"};
char months[12][4] = {
  "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

// Time Variables
uint8_t HourTens = 1;
uint8_t HourOnes = 2;
uint8_t MinTens = 0;
uint8_t MinOnes = 0;
uint8_t SecTens = 0;
uint8_t SecOnes = 0;

uint8_t Days =1;
uint8_t DateOnes = 1;
uint8_t DateTens =0;
uint8_t MonthOnes =1;
uint8_t MonthTens = 1;
uint8_t YearsOnes = 2;
uint8_t YearsTens = 1;

uint8_t DayCode =1;
uint8_t MonthCode =1;


boolean TH_Not24_flag = true;
boolean PM_NotAM_flag = false;
boolean NewTimeFormate = TH_Not24_flag;
uint8_t AMPMALARMDOTS = 0;

// Alarm
uint8_t AHourTens  = 1;
uint8_t AHourOnes = 2;
uint8_t AMinTens = 0;
uint8_t AMinOnes = 0;

boolean A_TH_Not24_flag = true;
boolean A_PM_NotAM_flag = false;

// StopWatch
  int OldTime = 0;
  int CurrentTime = 0;
  int TotalTime = 0;

uint8_t SWDigit4 = 0;  
uint8_t SWDigit3 = 0;   
uint8_t SWDigit2 = 0; 
uint8_t SWDigit1 = 0; 

int SWMINUTES = 0;
int SWSECONDS = 0;

int dayIndex = 0;

//uint8_t SetDigit  = 4;
//boolean MODEOVERRIDEFLAG = false;
boolean NextStateFlag = false;
boolean SetTimeFlag = false;
boolean ALARM1FLAG = false;
boolean ALARMON = false;

boolean scrollDirFlag = false;


//
volatile uint8_t Mcolumn = 0;
//volatile uint8_t Mrow = 0;
volatile uint8_t LEDMAT[20];

volatile boolean MINUP = false;
volatile boolean MINDOWN = false;
volatile boolean TFH = false;

const int digitoffset = 95;                // 95             // was 16


// Constants
// DS1337+ Address locations
#define		RTCDS1337	B01101000               // was B11010000  
#define		RTCCONT		B00001110		//; Control
#define		RTCSTATUS       B00001111		//; Status

//#define		RTC_HSEC	B00000001		//; Hundredth of a secound
#define		RTC_SEC		B00000000		//; Seconds
#define		RTC_MIN		B00000001		//; Minuites
#define		RTC_HOUR	B00000010		//; Hours

#define		RTC_DAY  	B00000011		//; Day
#define		RTC_DATE	B00000100		//; Date
#define		RTC_MONTH	B00000101		//; Month
#define		RTC_YEAR	B00000110		//; Year

#define		RTC_ALARM1SEC	B00000111		//; Seconds
#define		RTC_ALARM1MIN	B00001000		//; Minuites
#define		RTC_ALARM1HOUR	B00001001		//; Hours
#define         RTC_ALARM1DATE  B00001010               //; Date


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Bit Map Letter - data array
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// const byte LETTERS[95][5] = {
const byte LETTERS[106][5] = {
  B00000000,B00000000,B00000000,B00000000,B00000000, // Space
  B00000000,B00000000,B01011111,B00000000,B00000000, // !
  B00000000,B00000011,B00000000,B00000011,B00000000, // "
  B00010010,B00111111,B00010010,B00111111,B00010010, // #
  B00000100,B00101010,B01111111,B00101010,B00010000, // $
  B00100010,B00010101,B00101010,B01010100,B00100010, // %
  B00110110,B01001001,B01010001,B00100010,B01010000, // &
  B00000000,B00000000,B00000011,B00000000,B00000000, // '
  B00000000,B00011100,B00100010,B01000001,B00000000, // (
  B00000000,B01000001,B00100010,B00011100,B00000000, // (
  B00000100,B00010100,B00001111,B00010100,B00000100, // *
  B00001000,B00001000,B00111110,B00001000,B00001000, // +
  B00000000,B00000000,B00000010,B00000001,B00000000, // ' (not sure)
  B00001000,B00001000,B00001000,B00001000,B00001000, // -
  B00000000,B00000000,B01000000,B00000000,B00000000, // .
  B00100000,B00010000,B00001000,B00000100,B00000010, // /

  B00111110,B01000001,B01000001,B00111110,B00000000, // 0'
  B00000000,B01000010,B01111111,B01000000,B00000000, // 1'
  B01100010,B01010001,B01001001,B01000110,B00000000, // 2'
  B01001001,B01001001,B01001101,B00110011,B00000000, // 3'
  B00011000,B00010100,B00010010,B01111111,B00000000, // 4'
  B01000111,B01000101,B01000101,B00111001,B00000000, // 5'
  B00111110,B01000101,B01000101,B00111000,B00000000, // 6'
  B00000001,B00000001,B01111101,B00000011,B00000000, // 7'
  B01110111,B01001001,B01001001,B01110111,B00000000, // 8'
  B00001110,B01010001,B01010001,B00111110,B00000000, // 9'

  B00000000,B00000000,B00110110,B00000000,B00000000, // :
  B00000000,B01000000,B00110110,B00000000,B00000000, // ;
  B00001000,B00010100,B00100010,B01000001,B00000000, // <
  B00010100,B00010100,B00010100,B00010100,B00010100, // =
  B00000000,B01000001,B00100010,B00010100,B00001000, // >
  B00000110,B00000001,B01010001,B00001001,B00000110, // ?
  B00111110,B01000001,B01001001,B01010101,B00001010, // @

  B01111110,B00010001,B00010001,B01111110,B00000000, // A'
  B01111111,B01001001,B01001001,B00110110,B00000000, // B'
  B00111110,B01000001,B01000001,B00100010,B00000000, // C'
  B01111111,B01000001,B00100010,B00011100,B00000000, // D'
  B01111111,B01001001,B01001001,B01000001,B00000000, // E'
  B01111111,B00001001,B00001001,B00000001,B00000000, // F'
  B00111110,B01000001,B01001001,B00111010,B00000000, // G'
  B01111111,B00001000,B00001000,B01111111,B00000000, // H'
  B00000000,B01000001,B01111111,B01000001,B00000000, // I' 
  B00100000,B01000001,B00111111,B00000001,B00000000, // J'
  B01111111,B00001000,B00010100,B01100011,B00000000, // K'
  B01111111,B01000000,B01000000,B01000000,B00000000, // L'
  B01111111,B00000110,B00000110,B01111111,B00000000, // M'
  B01111111,B00000110,B00011000,B01111111,B00000000, // N'
  B00111110,B01000001,B01000001,B00111110,B00000000, // O'
  B01111111,B00001001,B00001001,B00000110,B00000000, // P'
  B00111110,B01010001,B00100001,B01011110,B00000000, // Q'
  B01111111,B00001001,B00001001,B01110110,B00000000, // R'
  B01000110,B01001001,B01001001,B00110001,B00000000, // S
  B00000000,B00000001,B01111111,B00000001,B00000000, // T'
  B00111111,B01000000,B01000000,B00111111,B00000000, // U'
  B00011111,B01100000,B01100000,B00011111,B00000000, // V
  B01111111,B00110000,B00110000,B01111111,B00000000, // W'
  B01110111,B00001000,B00001000,B01110111,B00000000, // X'
  B00000111,B01001000,B01001000,B00111111,B00000000, // Y'
  B01110001,B01001001,B01000101,B01000011,B00000000, // Z'

  B00000000,B01111111,B01000001,B01000001,B00000000, // [
  B00000010,B00000100,B00001000,B00010000,B00100000, // back slash
  B00000000,B01000001,B01000001,B01111111,B00000000, // ]
  B00000000,B00000010,B00000001,B00000010,B00000000, // ^
  B10000000,B10000000,B10000000,B10000000,B10000000, // _
  B00000000,B00000000,B00000001,B00000010,B00000000, // `

  B00100000,B01010100,B01010100,B01111000,B00000000, // a'
  B01111111,B01000100,B01000100,B00111000,B00000000, // b'
  B00111000,B01000100,B01000100,B00101000,B00000000, // c'
  B00111000,B01000100,B01000100,B01111111,B00000000, // d'
  B00111000,B01010100,B01010100,B00011000,B00000000, // e'
  B00010000,B01111000,B00010100,B00000100,B00000000, // f'
  B01011000,B01100100,B01100100,B00111000,B00000000, // g'
  B01111111,B00000100,B00000100,B01111000,B00000000, // h'
  B00000000,B01000100,B01111101,B01000000,B00000000, // i'
  B01000000,B01000100,B00111101,B00000000,B00000000, // j'
  B01111111,B00010000,B00101000,B01000100,B00000000, // k'
  B00000000,B00111111,B01000000,B00000000,B00000000, // l'
  B01111100,B00011100,B00011100,B01111000,B00000000, // m'
  B01111100,B00000100,B00000100,B01111000,B00000000, // n'
  B00111000,B01000100,B01000100,B00111000,B00000000, // o'
  B01111100,B00100100,B00100100,B00011000,B00000000, // p'
  B00011000,B00100100,B00100100,B01111100,B00000000, // q'
  B01111100,B00001000,B00000100,B00000100,B00000000, // r'
  B01001000,B01010100,B01010100,B00100100,B00000000, // s'
  B00000000,B00000100,B00111111,B01000100,B00000000, // t'
  B00111100,B01000000,B01000000,B00111100,B00000000, // u'
  B00011100,B01100000,B01100000,B00011100,B00000000, // v'
  B01111100,B00110000,B00110000,B01111100,B00000000, // w'
  B01101100,B00010000,B00010000,B01101100,B00000000, // x'
  B00001100,B01010000,B01010000,B00111100,B00000000, // y'
  B01100100,B01010100,B01001100,B01000100,B00000000, // z'

  B00000000,B00001000,B00110110,B01000001,B00000000, // {
  B00000000,B00000000,B01111111,B00000000,B00000000, // |
  B00000000,B01000001,B00110110,B00001000,B00000000, // }
  B00010000,B00001000,B00011000,B00010000,B00001000, // ~

  // Small Numbers (for adding colon in clock applications)
  // Tweaked for less of a seven-segment look
  B00000000,B00011100,B00100010,B00011100,B00000000,// 0
  B00000000,B00100100,B00111110,B00100000,B00000000,// 1
  B00000000,B00100110,B00110010,B00101110,B00000000,// 2
  B00000000,B00101010,B00101010,B00110110,B00000000,// 3
  B00000000,B00011000,B00010100,B00111110,B00000000,// 4
  B00000000,B00101110,B00101010,B00010010,B00000000,// 5
  B00000000,B00011100,B00101010,B00010010,B00000000,// 6
  B00000000,B00000010,B00111010,B00000110,B00000000,// 7
  B00000000,B00110110,B00101010,B00110110,B00000000,// 8
  B00000000,B00100100,B00101010,B00011100,B00000000,// 9
  B00000000,B00000000,B00010100,B00000000,B00000000,// :

};

const byte GRAPHIC[5][5] = {
  B00000000,B00000000,B00000000,B00000000,B00000000,
  B00000000,B00011100,B00111110,B01111111,B00000000, // Speaker cone
  B00100010,B00011100,B01000001,B00100010,B00011100, // Sound wave
  B00010000,B00100000,B00010000,B00001000,B00000100, // Check mark
  B00100010,B00010100,B00001000,B00010100,B00100010, // "X"
};

























