/*
BPB_Controller
Version			0.0.3 beta
Date		2014-11-08
License:	GNU General Public License, version 3 (GPL-3.0)
Author:		Omer Ikram ul haq
Description:http://hobbylogs.me.pn/?page_id=243
 
*/


/*
Definitions of Switches and Pin Numbers
*/
#define NUMBER_OF_SWITCHES     5
#define NUMBER_OF_DIMMERS      5
#define NUMBER_OF_MEASUREMENTS 4
#define MEASUREMENTS_REFRESH   1    //[Hz]

char switches[NUMBER_OF_SWITCHES+1] = "abcde";
int  switchPinNum[NUMBER_OF_SWITCHES] = {4,7,8,12,13};     //Same number is used as EEPROM address



char dimmers[NUMBER_OF_DIMMERS+1] = "ABCDE";
int  dimmerPinNum[NUMBER_OF_DIMMERS] = {5,6,9,10,11};       //Same number is used as EEPROM address


char measures[NUMBER_OF_MEASUREMENTS+1] = "ivxy";
int  measuresPinNum[NUMBER_OF_DIMMERS] = {0,1,2,3};
// ----------------END Definitions




//#include <SoftwareSerial.h>
#include <EEPROM.h>



#define interface_TX    3
#define interface_RX    2
#define input_size      16



//SoftwareSerial BT_port(interface_RX,interface_TX); //  Rx,Tx

char data[input_size];                             //  input command
char param, op;                                    //  input parameter
int  value;                                        //  input value
unsigned long t2;

void setup() {
  //BT_port.begin(9600);
  Serial.begin(115200);
  initSwitches();
  //BT_port.println("Ready...");
  t2 = millis();
}

void loop() {
  ReadCommand();
  ExecuteCommand();
}


/*-----------------------------
Helping Functions
-------------------------------*/

void ReadCommand(){
  // clearing out the chars
  for(int x=1;x<input_size;x++){
    data[x]='\0';
  }
  
  int i=0;
  data[i] = '|';
  if (Serial.available()>0){
    data[i] = Serial.read();
    i++;
    if (data[i-1] == '>')
    {
      while(1==1){
        if(Serial.available()>0){
          data[i] = Serial.read();
          i++;
          if (data[i-1] == '<'){
            //BT_port.println(data);
            break;
          }
        }
      }
    }else{
       data[i-1]='|';          // Discarding the data which was just read
    }
  }
}

void ExecuteCommand(){
  if (data[0] !='|'){
    param = data[1];                    //Storing parameter
    op = data[2];                       //Storing Operator
    
    char x[input_size-2];               //Storing value 
    for (int i=3;i<=input_size-1;i++){  //
       x[i-3] = data[i];                //
    }                                   //End Storing
    
    value = atoi(x);                    // Converting into int type
    
    if (param == '*'){
      statusCheck(op);            // Checking the status of operator
    }else{
      SetParameterValues(param,value);  // Setting the value of the parameter
    }
    
    data[0]='|';                        // Setting it back to Co command Character
  }
}


//------------------------------------------------------
// Setting Parameters
//------------------------------------------------------

void SetParameterValues(char p,int v){
  // Checking for Digital outputs 
  for (int i=0;i<=NUMBER_OF_SWITCHES;i++){
    if (p == switches[i]){
      digitalWrite(switchPinNum[i],v);
      EEPROM.write(switchPinNum[i],v);
    }
  }
  
  
  // Checking for PWM outputs 
  for (int i=0;i<=NUMBER_OF_DIMMERS;i++){
    if (p == dimmers[i]){
      //BT_port.println((int)v*2.55);
      analogWrite(dimmerPinNum[i],(int)v*2.55);
      if (v >= 100){
        EEPROM.write(dimmerPinNum[i],99);
      }else{
        EEPROM.write(dimmerPinNum[i],(int)v);
      }
    }
  }
}

//------------------------------------------------------
// initlizing the switiches
//------------------------------------------------------
void initSwitches(){
    for (int i=0;i<=NUMBER_OF_SWITCHES;i++){
       pinMode(switchPinNum[i],OUTPUT);
       digitalWrite(switchPinNum[i],EEPROM.read(switchPinNum[i]));
    }
    
    for (int i=0;i<=NUMBER_OF_DIMMERS;i++){
      pinMode(dimmerPinNum[i],OUTPUT);
      analogWrite(dimmerPinNum[i],(int)EEPROM.read(dimmerPinNum[i])*2.55);
    }
    
    // Measurements pins dont need initlizing
}


//------------------------------------------------------
// Check Status
//------------------------------------------------------
void statusCheck(char p){
  // Checking for Digital outputs 
  for (int i=0;i<=NUMBER_OF_SWITCHES;i++){
    if (p == switches[i]){
      answerCommand(p,EEPROM.read(switchPinNum[i]));
     }
  }
  
  
  // Checking for PWM outputs 
  for (int i=0;i<=NUMBER_OF_DIMMERS;i++){
    if (p == dimmers[i]){
      answerCommand(p,EEPROM.read(dimmerPinNum[i]));
    }
  }
  
  for (int i=0;i<=NUMBER_OF_MEASUREMENTS;i++){
      if (p == measures[i]){
         answerCommand(p,(int)analogRead(measuresPinNum[i])*100/1023);
      }
    }

}

//------------------------------------------------------
// Answer Command
//------------------------------------------------------
void answerCommand(char p,int v)
{
    char analogValueChar[4];
    String ValueStr;
    
    ValueStr = String(v);
    ValueStr.toCharArray(analogValueChar,3);
    
    Serial.write('>');
    Serial.write(p);
    Serial.write('=');
    Serial.write(analogValueChar);
    Serial.write('<');
    
    /*
    BT_port.write('>');
    BT_port.write(p);
    BT_port.write('=');
    BT_port.write(analogValueChar);
    BT_port.write('<');
    BT_port.print("\n");*/
}



