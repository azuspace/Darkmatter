#include <SPI.h>
#include "mpr121.h"
#include <Wire.h>


int SS1 = 3;
int level = 5;
int mute = 12;
int buzzerPin = 6;

int g = 8;
int r = 9;
int b = 10;

int audio_shutdown = 7;
boolean shutdown_state = LOW;

int syncButton = 4;

int irqpin = 2;
boolean touchStates[12];
byte sound = B00000000;
long time1 = 0;
long time2 = 0;

void setup()
{
  pinMode(g, OUTPUT);
  pinMode(r, OUTPUT);
  pinMode(b, OUTPUT);
  digitalWrite (g, LOW);
  digitalWrite(r, HIGH);
  digitalWrite(b, HIGH);
  
  pinMode(audio_shutdown, OUTPUT);
  digitalWrite(audio_shutdown, HIGH);
  
  pinMode(syncButton, OUTPUT);
  digitalWrite(syncButton, HIGH);
  
  pinMode(SS1, OUTPUT);
  pinMode(mute, OUTPUT);
  pinMode(irqpin, INPUT);
 
  digitalWrite(irqpin, HIGH);
  
  Wire.begin();
  mpr121_setup();
  SPI.begin();
}

void loop()
{
  readTouchInputs();
  
  if (time1 != 0 && (millis() - time1) > 300 && sound < B10000000)
  {
    
    sound = sound + B00001010;
    time1 = millis();
    beepBuzzer();
  }
  if (time2 != 0 && (millis() - time2) > 300 && sound > B00001010)
  {
    
    sound = sound - B00001010;
    time2 = millis();
    beepBuzzer();
  }
  setVol(B00000000,sound);
  
}

void setVol (byte reg , byte level )
{
  digitalWrite(SS1, LOW);
  SPI.transfer(reg);
  //level=map(level, 0, 12, 0, 128);
  SPI.transfer(level);
  digitalWrite(SS1, HIGH);
}

void readTouchInputs(){
  if(!checkInterrupt()){
    
    Wire.requestFrom(0x5A,2); 
    
    byte LSB = Wire.read();
    byte MSB = Wire.read();
    
    uint16_t touched = ((MSB << 8) | LSB);

    
    for (int i=0; i < 4; i++){
      if(touched & (1<<i))
      {      
        if(touchStates[i] == 0)
        {
          // First Touch Button Volume Up
          if(i == 0 && sound < B10000000)
          {
            
            sound = sound + B00001010;
            time1 = millis();
            beepBuzzer();
          }
          // Second Touch Button Volume Down
          if(i == 1 && sound > B00001010)
          {
            
            sound = sound - B00001010;
            time2 = millis();
            beepBuzzer();
          }
          // Third Touch Button Mute (shutdown/low power state)
          if(i == 2)
          {
            if (!shutdown_state)
            {
              beepBuzzer();
              digitalWrite(audio_shutdown,LOW);
              //sound = B00000000;
              //setVol(B00000000,sound);
              shutdown_state = !shutdown_state;
            }
            else
            {
              beepBuzzer();
              digitalWrite(audio_shutdown,HIGH);
              //sound = B01000000;
              //setVol(B00000000,sound);
              shutdown_state = !shutdown_state;
            }
          }
          // Fourth Touch Button Sync
          if(i == 3)
          {
            beepBuzzer();
            digitalWrite(syncButton, LOW);
            delay(800);
            digitalWrite(syncButton, HIGH);
          }
        
          
        }
        else if(touchStates[i] == 1){}  
        touchStates[i] = 1;      
      }
      else{
        
        if(touchStates[i] == 1)
        {
          if (i == 0)
          {
              time1 = 0;
          }
          if (i == 1)
          {
              time2 = 0;
          }
        touchStates[i] = 0;
      }
    }
  }
}
}




void mpr121_setup(void){

  set_register(0x5A, ELE_CFG, 0x00); 
  
  // Section A - Controls filtering when data is > baseline.
  set_register(0x5A, MHD_R, 0x01);
  set_register(0x5A, NHD_R, 0x01);
  set_register(0x5A, NCL_R, 0x00);
  set_register(0x5A, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(0x5A, MHD_F, 0x01);
  set_register(0x5A, NHD_F, 0x01);
  set_register(0x5A, NCL_F, 0xFF);
  set_register(0x5A, FDL_F, 0x02);
  
  // Section C - Sets touch and release thresholds for each electrode
  set_register(0x5A, ELE0_T, 0x08);
  set_register(0x5A, ELE0_R, REL_THRESH);
 
  set_register(0x5A, ELE1_T, 0x08);
  set_register(0x5A, ELE1_R, REL_THRESH);
  
  set_register(0x5A, ELE2_T, 0x08);// Here is where you change the threshold for pin 2 it was 0x02, the most sensitive is 0x01.
  set_register(0x5A, ELE2_R, REL_THRESH);
  
  set_register(0x5A, ELE3_T, 0x08);
  set_register(0x5A, ELE3_R, REL_THRESH);
  
  set_register(0x5A, ELE4_T, TOU_THRESH);
  set_register(0x5A, ELE4_R, REL_THRESH);
  
  set_register(0x5A, ELE5_T, TOU_THRESH);
  set_register(0x5A, ELE5_R, REL_THRESH);
  
  set_register(0x5A, ELE6_T, TOU_THRESH);
  set_register(0x5A, ELE6_R, REL_THRESH);
  
  set_register(0x5A, ELE7_T, TOU_THRESH);
  set_register(0x5A, ELE7_R, REL_THRESH);
  
  set_register(0x5A, ELE8_T, TOU_THRESH);
  set_register(0x5A, ELE8_R, REL_THRESH);
  
  set_register(0x5A, ELE9_T, TOU_THRESH);
  set_register(0x5A, ELE9_R, REL_THRESH);
  
  set_register(0x5A, ELE10_T, TOU_THRESH);
  set_register(0x5A, ELE10_R, REL_THRESH);
  
  set_register(0x5A, ELE11_T, TOU_THRESH);
  set_register(0x5A, ELE11_R, REL_THRESH);
  
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(0x5A, FIL_CFG, 0x04);
  
  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(0x5A, ELE_CFG, 0x0C);  // Enables all 12 Electrodes
  
  
  // Section F
  // Enable Auto Config and auto Reconfig
  /*set_register(0x5A, ATO_CFG0, 0x0B);
  set_register(0x5A, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(0x5A, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(0x5A, ATO_CFGT, 0xB5); */ // Target = 0.9*USL = 0xB5 @3.3V
  
  set_register(0x5A, ELE_CFG, 0x0C);
  
}


boolean checkInterrupt(void){
  return digitalRead(irqpin);
}


void set_register(int address, unsigned char r, unsigned char v){
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}

void beepBuzzer()
{
   analogWrite(buzzerPin, 25);
   delay(80);
   analogWrite(buzzerPin, 0);
}


