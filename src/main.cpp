#include <Arduino.h>
#include <max6675.h>
#include <timer.h>
#include <PID_v1.h>

//DEFINES********************************************************************************
#define THERMO_DO 4   //
#define THERMO_CS 5   // THERMOCOUPLE
#define THERMO_CLK 6  //

#define SSR 3         // Solid State Relay

#define KP 5.16
#define KI .8125984252  //PID arguments
#define KD 8.1915

//FUNCTION DECLERATION*******************************************************************
void update(); //Execute timer function.
void pidExecute();

//GLOBALS********************************************************************************
int GLOBAL_OLD_TEMP = 0;
double GLOBAL_TEMP = 0;
double GLOBAL_TEMP_SETPOINT = 650;
double GLOBAL_TEMP_OUTPUT;


// Timers********************************************************************************
Timer timer;

// Thermocouple**************************************************************************
MAX6675 thermocouple(THERMO_CLK, THERMO_CS, THERMO_DO); //Thermistor object

//PID************************************************************************************
PID eNail(&GLOBAL_TEMP, &GLOBAL_TEMP_OUTPUT, &GLOBAL_TEMP_SETPOINT, KP, KI, KD, DIRECT);

//SETUP***********************************************************************************
void setup() {
  //Serial
  Serial.begin(9600);
  Serial.println("Serial Started at 9600bps");

  //Temp Sensor
  GLOBAL_TEMP = thermocouple.readFarenheit(); //Temp in F.

  //Solid State Relay
  pinMode(SSR,OUTPUT);

  //PID
  eNail.SetMode(AUTOMATIC);


  //Timers
  timer.every(220,pidExecute);
  timer.every(100,update);

  


}
//LOOP***********************************************************************************
void loop() {
  timer.update();
}

//FUNCTIONS******************************************************************************
void update(){ //Execute Timer Function.
  int output = floor(GLOBAL_TEMP_SETPOINT);
  int temp = floor(GLOBAL_TEMP);
  Serial.print("Real_Time_TMP.txt=");
  Serial.print("\"");
  Serial.print(temp);
  Serial.print(" F");
  Serial.print("\"");
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);

  Serial.print("Set_Temp.txt=");
  Serial.print("\"");
  Serial.print(output);
  Serial.print("\"");
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);

}

void pidExecute(){
  GLOBAL_TEMP = thermocouple.readFarenheit(); //Temp in F
  eNail.Compute();
  analogWrite(SSR,GLOBAL_TEMP_OUTPUT);
}
