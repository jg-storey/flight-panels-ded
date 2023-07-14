/*   DED v1.22 Add lock/shoot leds for Hornet. Changed buttonPin variable name to modePin1. Added second mode switch (modePin2). Changed mode logic to allow 4 aircraft modes.
 *   DED v1.21 Testing to try and get inverted text working. Changed ded addresses to match latest dcs-bios version
 *   DED v1.20 Compatible with dcs-bios-0.3.7 from https://github.com/DCSFlightpanels.
 *   DED v1.15 Implemented interrupt method to switch mode.
 *   DED v1.1 Added antenna symbols that replace the radio channels during radio operation in F-18 mode.
 *   Changed LIST and MISC detection method as just checking the first row wasn't robust enough.
 *  DED v1.0 Created by James Storey in May 2022, github.com/jg-storey/ded
 *  Arduino code to display F-16 Data Entry Display and F-18 Upfront Controls information from DCS-BIOS
 *  on a ER-OLEDM028-1 2.8" 256x64 OLED display from BuyDisplay.com
 *  https://www.buydisplay.com/yellow-2-8-inch-arduino-raspberry-pi-oled-display-module-256x64-spi
 *  Tested with DCS-BIOS v0.10.0
 *  https://github.com/dcs-bios/dcs-bios/releases
 *  
 *  A toggle switch (i.e. not a pushbutton) wired between pin D2 and GND allows the user to select between the F-16 and F-18 modes.
 *  If a switch is not connected, the mode defaults to the F-16.
 *  The selected mode activates after disconnecting and connecting to the Arduino in the DCS-BIOS Hub, or by resetting the Arduino.
 *  There is probably a better way to change modes using an interrupt.
 *  
 *  The F-18 display also includes a fuel reading and a master arm indicator. 
 *  There is still some space to add other items in both modes.
 *  
 *  Known issues. The DEST page in the LIST menu comes up blank. This appears to be an issue with DCS-BIOS.
 *  When entering data between the highlighted asterisks the field does not highlight. I haven't figured out an efficient way to do this yet.
 */

/*
  Tell DCS-BIOS to use a serial connection and use interrupt-driven
  communication. The main program will be interrupted to prioritize
  processing incoming data.
  
  This should work on any Arduino that has an ATMega328 controller
  (Uno, Pro Mini, many others).
 */
#define DCSBIOS_IRQ_SERIAL


#include "DcsBios.h"

/***************************************************
//Web: http://www.buydisplay.com
EastRising Technology Co.,LTD
Examples for ER-OLEDM28-1
Display is Hardward SPI 4-Wire SPI Interface 
Tested and worked with:
Works with Arduino 1.6.0 IDE  
Test OK : Arduino DUE,Arduino mega2560,Arduino UNO Board 
****************************************************/

/*
 Note:The module needs to be jumpered to an SPI interface. R19,R23 Short and R18,R20 Open  
 Unused signal pin Recommended to connect to GND
  == Hardware connection ==
    OLED   =>    Arduino
  *1. GND    ->    GND
  *2. VCC    ->    3.3
  *4. SCL    ->    SCK (D13)
  *5. SDI    ->    MOSI (D11)
  *14. DC     ->    D9
  *15. RES    ->    D8  
  *16. CS     ->    D10
*/



#include "er_oled.h"


volatile boolean f16 = true; // flag to display F-16 DED
volatile boolean f18 = false; // flag to display F-18 UFC
//boolean isList = false; //flag to indicate if we are on the LIST or MISC DED pages

volatile int mode = 16;

const int modePin1 = 2; //Toggle switch to change modes
const int modePin2 = 3; //Second toggle switch to change modes
const int lockLEDPin = 4; //Lock LED indicator
const int shootLEDPin = 5; //Shoot LED indicator
const int strobeLEDPin = 6; //Shoot strobe LED indicator
//variables to keep track of the timing of recent interrupts
unsigned long button_time = 0;
unsigned long last_button_time = 0;

//F-16 DCS BIOS Functions

void onDedLine1Change(char* newValue) {
    /* your code here */
    if(mode==16){
      //Serial.print(newValue); 
      er_oled_dedstring(0,0,newValue,0); 
    }
}
DcsBios::StringBuffer<29> dedLine1Buffer(0x450a, onDedLine1Change);

void onDedLine2Change(char* newValue) {
    /* your code here */
    if(mode==16){
      er_oled_dedstring(0,13,newValue,0); 
      //size_t s = strlen(newValue);
      //String st = String(s);
      //er_oled_dedstring(25*8,13,st.c_str(),0);
      /*
      //Highlight numbers if we are on the LIST or MISC pages
      if(newValue[0]=='1' && (newValue[1]=='D' || newValue[1]=='C')){
        //Highlight the menu numbers
        er_oled_string(0,13,"1",1);
        er_oled_string(6*8,13,"2",1);
        er_oled_string(12*8,13,"3",1);
        er_oled_string(18*8,13,"R",1);
      }
      */
      
    }  
}
DcsBios::StringBuffer<29> dedLine2Buffer(0x4528, onDedLine2Change);


void onDedLine3Change(char* newValue) {
    /* your code here */
    if(mode==16){
      er_oled_dedstring(0,26,newValue,0); 

      //size_t s = strlen(newValue);
      //String st = String(s);
      //er_oled_dedstring(25*8,26,st.c_str(),0);
      /*
      if(newValue[0]=='4' && (newValue[1]=='N' || newValue[1]=='I')){
        er_oled_string(0,26,"4",1);
        er_oled_string(6*8,26,"5",1);
        er_oled_string(12*8,26,"6",1);
        er_oled_string(18*8,26,"E",1);
      }
      */
    }  
}
DcsBios::StringBuffer<29> dedLine3Buffer(0x4546, onDedLine3Change);

void onDedLine4Change(char* newValue) {
    /* your code here */
    if(mode==16){
      er_oled_dedstring(0,39,newValue,0);  

      //size_t s = strlen(newValue);
      //String st = String(s);
      //er_oled_dedstring(25*8,39,st.c_str(),0);
      /*
      if(newValue[0]=='7' && (newValue[1]=='C' || newValue[1]=='D')){
        er_oled_string(0,39,"7",1);
        er_oled_string(6*8,39,"8",1);
        er_oled_string(12*8,39,"9",1);
        er_oled_string(18*8,39,"0",1);
      }
      */
    }
}
DcsBios::StringBuffer<29> dedLine4Buffer(0x4564, onDedLine4Change);

void onDedLine5Change(char* newValue) {
    /* your code here */
    if(mode==16){
      er_oled_dedstring(0,52,newValue,0); 
      //size_t s = strlen(newValue);
      //String st = String(s);
      //er_oled_dedstring(25*8,52,st.c_str(),0);
    }  
}
DcsBios::StringBuffer<29> dedLine5Buffer(0x4582, onDedLine5Change);




// F-18 DCS BIOS Functions
void onUfcComm1DisplayChange(char* newValue) {
    /* your code here */
    if(mode==18){
      er_oled_string(0*8,52,newValue,0);
    }
    //else if(mode==8){
    //  er_oled_string(16*8,26,newValue,0);
    //}
}
DcsBios::StringBuffer<2> ufcComm1DisplayBuffer(0x7424, onUfcComm1DisplayChange);


void onUfcComm2DisplayChange(char* newValue) {
    /* your code here */
    if(mode==18){
      er_oled_string(5*8,52,newValue,0);
    }
    //else if(mode==8){
    //  er_oled_string(26*8,26,newValue,0);
    //}
}
DcsBios::StringBuffer<2> ufcComm2DisplayBuffer(0x7426, onUfcComm2DisplayChange);


void onUfcOptionCueing1Change(char* newValue) {
    /* your code here */
    if(mode==18){
      er_oled_string(12*8,0,newValue,0);
    }  
}
DcsBios::StringBuffer<1> ufcOptionCueing1Buffer(0x7428, onUfcOptionCueing1Change);


void onUfcOptionCueing2Change(char* newValue) {
    /* your code here */
    if(mode==18){
      er_oled_string(12*8,13,newValue,0);
    }
}
DcsBios::StringBuffer<1> ufcOptionCueing2Buffer(0x742a, onUfcOptionCueing2Change);


void onUfcOptionCueing3Change(char* newValue) {
    /* your code here */
    if(mode==18){
      er_oled_string(12*8,26,newValue,0);
    }  
}
DcsBios::StringBuffer<1> ufcOptionCueing3Buffer(0x742c, onUfcOptionCueing3Change);


void onUfcOptionCueing4Change(char* newValue) {
    /* your code here */
    if(mode==18){
      er_oled_string(12*8,39,newValue,0);
    }
}
DcsBios::StringBuffer<1> ufcOptionCueing4Buffer(0x742e, onUfcOptionCueing4Change);

void onUfcOptionCueing5Change(char* newValue) {
    /* your code here */
    if(mode==18){
      er_oled_string(12*8,52,newValue,0);
    }
}
DcsBios::StringBuffer<1> ufcOptionCueing5Buffer(0x7430, onUfcOptionCueing5Change);


void onUfcOptionDisplay1Change(char* newValue) {
    /* your code here */
    if(mode==18){
      er_oled_string(13*8,0,newValue,0);
    }
}
DcsBios::StringBuffer<4> ufcOptionDisplay1Buffer(0x7432, onUfcOptionDisplay1Change);


void onUfcOptionDisplay2Change(char* newValue) {
    /* your code here */
    if(mode==18){
      er_oled_string(13*8,13,newValue,0);
    }
}
DcsBios::StringBuffer<4> ufcOptionDisplay2Buffer(0x7436, onUfcOptionDisplay2Change);

void onUfcOptionDisplay3Change(char* newValue) {
    /* your code here */
    if(mode==18){
      er_oled_string(13*8,26,newValue,0);
    }
}
DcsBios::StringBuffer<4> ufcOptionDisplay3Buffer(0x743a, onUfcOptionDisplay3Change);

void onUfcOptionDisplay4Change(char* newValue) {
    /* your code here */
    if(mode==18){
      er_oled_string(13*8,39,newValue,0);
    }
}
DcsBios::StringBuffer<4> ufcOptionDisplay4Buffer(0x743e, onUfcOptionDisplay4Change);

void onUfcOptionDisplay5Change(char* newValue) {
    /* your code here */
    if(mode==18){
      er_oled_string(13*8,52,newValue,0);
    }
}
DcsBios::StringBuffer<4> ufcOptionDisplay5Buffer(0x7442, onUfcOptionDisplay5Change);


void onUfcScratchpadNumberDisplayChange(char* newValue) {
    /* your code here */
    if(mode==18){
      er_oled_string(3*8,0,newValue,0);
    }
}
DcsBios::StringBuffer<8> ufcScratchpadNumberDisplayBuffer(0x7446, onUfcScratchpadNumberDisplayChange);



void onUfcScratchpadString1DisplayChange(char* newValue) {
    /* your code here */
    if(mode==18){
      er_oled_string(0*8,0,newValue,0);
    }
}
DcsBios::StringBuffer<2> ufcScratchpadString1DisplayBuffer(0x744e, onUfcScratchpadString1DisplayChange);

void onUfcScratchpadString2DisplayChange(char* newValue) {
    /* your code here */
    if(mode==18){
      er_oled_string(2*8,0,newValue,0);
    }
}
DcsBios::StringBuffer<2> ufcScratchpadString2DisplayBuffer(0x7450, onUfcScratchpadString2DisplayChange);


void onIfeiFuelUpChange(char* newValue) {
    /* your code here */
    if(mode==18){
      er_oled_string(20*8,26,newValue,0);
    }
}
DcsBios::StringBuffer<6> ifeiFuelUpBuffer(0x748a, onIfeiFuelUpChange);

//void onMasterModeAaLtChange(unsigned int newValue) {
    /* your code here */
//}
//DcsBios::IntegerBuffer masterModeAaLtBuffer(0x740c, 0x0200, 9, onMasterModeAaLtChange);

void onMasterArmSwChange(unsigned int newValue) {
    /* your code here */
    if(mode==18){
      if(newValue){
      er_oled_string(20*8,0," ARM ",1);
    }else{
      er_oled_string(20*8,0," SAFE ",0);
    }
  }
}
DcsBios::IntegerBuffer masterArmSwBuffer(0x740c, 0x2000, 13, onMasterArmSwChange);



DcsBios::LED lsLock(0x7408, 0x0001, lockLEDPin);
DcsBios::LED lsShoot(0x7408, 0x0002, shootLEDPin);
DcsBios::LED lsShootStrobe(0x7408, 0x0004, strobeLEDPin);



void onPcnDispDestChange(char* newValue) {
    /* your code here */
    //01
    if(mode==2000){
      er_oled_string(18*8,26,newValue,0);
    }
}
DcsBios::StringBuffer<2> pcnDispDestBuffer(0x72e6, onPcnDispDestChange);

void onPcnDispLChange(char* newValue) {
    /* your code here */
    //42545
    if(mode==2000){
      er_oled_string(3*8,8,newValue,0);
    }
}
DcsBios::StringBuffer<8> pcnDispLBuffer(0x72e8, onPcnDispLChange);

void onPcnDispPrepChange(char* newValue) {
    /* your code here */
    //01
    if(mode==2000){
      er_oled_string(6*8,26,newValue,0);
    }
}
DcsBios::StringBuffer<2> pcnDispPrepBuffer(0x72f0, onPcnDispPrepChange);

void onPcnDispRChange(char* newValue) {
    /* your code here */
    //040594
    if(mode==2000){
      er_oled_string(15*8,8,newValue,0);
    }
}
DcsBios::StringBuffer<9> pcnDispRBuffer(0x72f2, onPcnDispRChange);

void onPcnDis2dlChange(char* newValue) {
    /* your code here */
    //NS
    if(mode==2000){
      er_oled_string(0*8,8,newValue,0);
    }
}
DcsBios::StringBuffer<2> pcnDis2dlBuffer(0x7300, onPcnDis2dlChange);

void onPcnDis2drChange(char* newValue) {
    /* your code here */
    //EW
    if(mode==2000){
      er_oled_string(13*8,8,newValue,0);
    }
}
DcsBios::StringBuffer<2> pcnDis2drBuffer(0x7302, onPcnDis2drChange);





void onUfcScratchpadChange(char* newValue) {
    /* your code here */
    if(mode==8){
      er_oled_string(16*8,8,newValue,0);
    }
}
DcsBios::StringBuffer<12> ufcScratchpadBuffer(0x7984, onUfcScratchpadChange);





void onAv8bnaOdu1SelectChange(char* newValue) {
    /* your code here */
    if(mode==8){
      er_oled_string(6*8,8,newValue,0);
    }
}
DcsBios::StringBuffer<1> av8bnaOdu1SelectBuffer(0x7966, onAv8bnaOdu1SelectChange);

void onAv8bnaOdu1TextChange(char* newValue) {
    /* your code here */
    if(mode==8){
      er_oled_string(7*8,8,newValue,0);
    }
}
DcsBios::StringBuffer<4> av8bnaOdu1TextBuffer(0x7968, onAv8bnaOdu1TextChange);

void onAv8bnaOdu2SelectChange(char* newValue) {
    /* your code here */
    if(mode==8){
      er_oled_string(6*8,26,newValue,0);
    }
}
DcsBios::StringBuffer<1> av8bnaOdu2SelectBuffer(0x796c, onAv8bnaOdu2SelectChange);

void onAv8bnaOdu2TextChange(char* newValue) {
    /* your code here */
    if(mode==8){
      er_oled_string(7*8,26,newValue,0);
    }
}
DcsBios::StringBuffer<4> av8bnaOdu2TextBuffer(0x796e, onAv8bnaOdu2TextChange);

void onAv8bnaOdu3SelectChange(char* newValue) {
    /* your code here */
    if(mode==8){
      er_oled_string(6*8,44,newValue,0);
    }
}
DcsBios::StringBuffer<1> av8bnaOdu3SelectBuffer(0x7972, onAv8bnaOdu3SelectChange);

void onAv8bnaOdu3TextChange(char* newValue) {
    /* your code here */
    if(mode==8){
      er_oled_string(7*8,44,newValue,0);
    }
}
DcsBios::StringBuffer<4> av8bnaOdu3TextBuffer(0x7974, onAv8bnaOdu3TextChange);

void onAv8bnaOdu4SelectChange(char* newValue) {
    /* your code here */
    if(mode==8){
      er_oled_string(0*8,26,newValue,0);
    }
}
DcsBios::StringBuffer<1> av8bnaOdu4SelectBuffer(0x7978, onAv8bnaOdu4SelectChange);

void onAv8bnaOdu4TextChange(char* newValue) {
    /* your code here */
    if(mode==8){
      er_oled_string(1*8,26,newValue,0);
    }
}
DcsBios::StringBuffer<4> av8bnaOdu4TextBuffer(0x797a, onAv8bnaOdu4TextChange);


void onAv8bnaOdu5SelectChange(char* newValue) {
    /* your code here */
    if(mode==8){
      er_oled_string(0*8,44,newValue,0);
    }
}
DcsBios::StringBuffer<1> av8bnaOdu5SelectBuffer(0x797e, onAv8bnaOdu5SelectChange);

void onAv8bnaOdu5TextChange(char* newValue) {
    /* your code here */
    if(mode==8){
      er_oled_string(1*8,44,newValue,0);
    }
}
DcsBios::StringBuffer<4> av8bnaOdu5TextBuffer(0x7980, onAv8bnaOdu5TextChange);







void splashScreen(){
  er_oled_clear();
  er_oled_string(12*8,13,"DED V1.22",1);
  //er_oled_string(4*8,26,"GITHUB.COM/JG-STOREY/DED",0);
  if(mode==16){
    er_oled_string(11*8,52,"F-16 MODE",0);
  }
  else if(mode==18){
    er_oled_string(11*8,52,"F/A-18 MODE",0);
  }
  else if(mode==2000){
    er_oled_string(11*8,52,"M2000C MODE",0);
  }
  else if(mode==8){
    er_oled_string(11*8,52,"AV-8B MODE",0);
  }
  
}

void button_ISR(){
  button_time = millis();
  //check to see if increment() was called in the last 250 milliseconds
  if (button_time - last_button_time > 250){
    //f16 = digitalRead(modePin1);
    //f18 = !f16;
    boolean m1 = digitalRead(modePin1);
    boolean m2 = digitalRead(modePin2);
    if(m1 && m2){
      mode = 16; //f16
    }
    else if(!m1 && m2){
      mode = 18; //f18
    }
    else if(m1 && !m2){
      mode = 2000; //M2000
    }
    else if(!m1 && !m2){
      mode = 8; //AV8B
    }
    splashScreen();
    er_oled_clear();
    last_button_time = button_time;
  }
}

void setup() {
  //Serial.begin(9600);
  //Serial.print("DED");
  //set D2 as a switch to select F-16 or F-18 displays. If unconnected it defaults to F-16.
  //now using D2 and D3 to select between F-16, F-18, M2000 and AV-8B modes.
  pinMode(modePin1, INPUT_PULLUP);
  pinMode(modePin2, INPUT_PULLUP);
  boolean m1 = digitalRead(modePin1);
  boolean m2 = digitalRead(modePin2);
  if(m1 && m2){
    mode = 16; //f16
  }
  else if(!m1 && m2){
    mode = 18; //f18
  }
  else if(m1 && !m2){
    mode = 2000; //M2000
  }
  else if(!m1 && !m2){
    mode = 8; //AV8B
  }
  //f16 = digitalRead(modePin1);
  //f18 = !f16;
  /* initialize the display */
  er_oled_begin();
  splashScreen();
  delay(2000);
  er_oled_clear();
  attachInterrupt(digitalPinToInterrupt(modePin1), button_ISR, CHANGE);  
  
  DcsBios::setup();
  
}


void loop() {

DcsBios::loop();
 
}
