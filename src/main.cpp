#include <Arduino.h>
#include <max6675.h>
#include <timer.h>
#include <PID_v1.h>
#include <Nextion.h>

//NOTES**********************************************************************************
//  Currently on Version 2 of the nextion screen.
//  Changes:
//    Center Text Lettering
//    Drop font size on F, and add °.
//DEFINES********************************************************************************
#define THERMO_DO 4   //
#define THERMO_CS 5   // THERMOCOUPLE
#define THERMO_CLK 6  //

#define SSR 3         // Solid State Relay

#define KP 5.16
#define KI .8125984252  //PID arguments
#define KD 8.1915

#define INC_TMP_BUTTON 3
#define DEC_TMP_BUTTON 1
#define REAL_TIME_TEMP_TEXTBOX 4
#define SET_TEMP_TEXTBOX 5

#define SET_INCREMENT 25
#define MAX_TEMP_SETPOINT 1000

#define MAIN_PAGE 0
#define SETTING_PAGE 1
#define CLEAN_MODE_PAGE 2
#define BRIGHTNESS_PAGE 3


//FUNCTION DECLERATION*******************************************************************
void update(); //Execute timer function.
void pidExecute();
void inc_tmp_PushCallback(void *ptr);
void dec_tmp_PushCallBack(void *ptr);
void clearBuffer();

//GLOBALS********************************************************************************
int GLOBAL_OLD_TEMP = 0;
double GLOBAL_TEMP = 0;
double GLOBAL_TEMP_SETPOINT = 650;
double GLOBAL_TEMP_OUTPUT;


// Timers********************************************************************************
Timer timer;

// Thermocouple**************************************************************************
MAX6675 thermocouple(THERMO_CLK, THERMO_CS, THERMO_DO);

//PID************************************************************************************
PID eNail(&GLOBAL_TEMP, &GLOBAL_TEMP_OUTPUT, &GLOBAL_TEMP_SETPOINT, KP, KI, KD, DIRECT);


//Nextion Objects*************************************************************************

NexText INC_TMP = NexText(MAIN_PAGE, INC_TMP_BUTTON, "INC_TMP");
NexText DEC_TMP = NexText(MAIN_PAGE, DEC_TMP_BUTTON, "DEC_TMP");
NexText REAL_TIME_TMP_TEXTBOX = NexText(MAIN_PAGE,REAL_TIME_TEMP_TEXTBOX,"Real_Time_TMP");
NexText SET_TMP_TEXTBOX = NexText(MAIN_PAGE,SET_TEMP_TEXTBOX,"Set_Temp");

char GLOBAL_BUFFER[10] = {0};

NexTouch *next_listen_list[] = {
  &INC_TMP,
  &DEC_TMP,
  NULL
};






//SETUP***********************************************************************************
void setup() {
  //Nextion Init
  nexInit();

  //Temp Sensor
  GLOBAL_TEMP = thermocouple.readFarenheit(); //Temp in F.

  //Solid State Relay
  pinMode(SSR,OUTPUT);

  //PID
  eNail.SetMode(AUTOMATIC);


  //Timers
  timer.every(220,pidExecute);
  timer.every(500,update);

  //Nextion Button Callbacks
  INC_TMP.attachPush(inc_tmp_PushCallback);
  DEC_TMP.attachPush(dec_tmp_PushCallBack);

}
//LOOP***********************************************************************************
void loop() {
  timer.update();
  nexLoop(next_listen_list);
}

//FUNCTIONS******************************************************************************


void inc_tmp_PushCallback(void *ptr){
  if (GLOBAL_TEMP_SETPOINT < MAX_TEMP_SETPOINT) GLOBAL_TEMP_SETPOINT += SET_INCREMENT;

  clearBuffer();
  itoa(GLOBAL_TEMP_SETPOINT,GLOBAL_BUFFER,10);
  SET_TMP_TEXTBOX.setText(GLOBAL_BUFFER);

}

void dec_tmp_PushCallBack(void *ptr){
  GLOBAL_TEMP_SETPOINT -= SET_INCREMENT;
  if (GLOBAL_TEMP_SETPOINT < 0) GLOBAL_TEMP_SETPOINT = 0;

  clearBuffer();
  itoa(GLOBAL_TEMP_SETPOINT,GLOBAL_BUFFER,10);
  SET_TMP_TEXTBOX.setText(GLOBAL_BUFFER);
}

void update(){ //Execute Timer Function.
  int temp = floor(GLOBAL_TEMP);
  clearBuffer();
  itoa(temp,GLOBAL_BUFFER,10);
  const char *F = " F";
  strcat(GLOBAL_BUFFER,F);

  REAL_TIME_TMP_TEXTBOX.setText(GLOBAL_BUFFER);

  clearBuffer();
  itoa(GLOBAL_TEMP_SETPOINT,GLOBAL_BUFFER,10);
  SET_TMP_TEXTBOX.setText(GLOBAL_BUFFER);
  
}

void pidExecute(){
  GLOBAL_TEMP = thermocouple.readFarenheit(); //Temp in F
  eNail.Compute();
  analogWrite(SSR,GLOBAL_TEMP_OUTPUT);
}

void clearBuffer(){
  memset(GLOBAL_BUFFER,0,sizeof(GLOBAL_BUFFER));
}
