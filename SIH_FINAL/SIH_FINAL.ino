#include <SoftwareSerial.h>
#include <TinyGPS.h>
TinyGPS gps;
SoftwareSerial ss(0, 1);
SoftwareSerial sgsm(7, 8);

#define RX 7
#define TX 8
#define RESET 2
#define BAUD 9600
#include "Arduino.h"
#include "MPU6050.h"
#include "Wire.h"
#include "I2Cdev.h"
#include<TimerOne.h>
int interruptPin = 2;
int Thx = 32700;
int flag=1;
int Thy = 10;
int Thz = 10;
int buzzer = 9;
int pushbutton = 5;
int16_t mpu6050Ax, mpu6050Ay, mpu6050Az;
int16_t mpu6050Gx, mpu6050Gy, mpu6050Gz;

MPU6050 mpu6050;

float gpslat=10, gpslon=10;
float flat, flon;
unsigned long age;

String link = "https://maps.google.com/maps/place/";
String linkLAT;
String linkLON; 
String linkf; 
String LINK();
String Glink;

unsigned long previousMillis = 0; 
const long interval = 5000; 
void setup()
{
  sgsm.begin(9600);
  Serial.begin(9600);
  ss.begin(9600);
  mpu6050.initialize();
  Wire.begin();
 pinMode(10,OUTPUT);
 pinMode(5,OUTPUT);
 digitalWrite(10,LOW);

    pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), forStop, CHANGE);
 
 
}

void loop()
{
Glink =  LINK();
mpu6050.getMotion6(&mpu6050Ax, &mpu6050Ay, &mpu6050Az, &mpu6050Gx, &mpu6050Gy, &mpu6050Gz); 
  double mpu6050Temp = ((double)mpu6050.getTemperature() + 12412.0) / 340.0;

  Serial.print("Ax ");
  Serial.print(mpu6050Ax); Serial.print("\n");
  //Serial.print("Ay ");
  //Serial.print(mpu6050Ay); Serial.print("\n");
  //Serial.print("Ax ");
  //Serial.print(mpu6050Az); Serial.print("\n");

  if (mpu6050Ax > Thx )
  {
    tone(buzzer, 450);
    
    delay(10000);
    
    noTone(buzzer);
    if(flag==1)
    {
    Glink = LINK();
    Serial.println( Glink );
    SendMessage();
    }
  }
 flag=1;

} // VOID LOOP END

String LINK()
 {
    
 
    while (ss.available())
    {
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
          {}
    }

  
    
    gps.f_get_position(&flat, &flon, &age);
    linkLAT = String(flat);
    linkLON = String(flon);
    linkf = link + linkLAT + "," + linkLON;
   // Serial.print(linkf);
    
  return linkf;
  
  
  } 



  void SendMessage()
{ 
   
      sgsm.print("\r");
      delay(1000);
      sgsm.print("AT+CMGF=1\r");
      delay(1000);
      /*Replace XXXXXXXXXX to 10 digit mobile number &
        ZZ to 2 digit country code*/
      sgsm.print("AT+CMGS=\"+number\"\r");
      delay(1000); 
      //The text of the message to be sent.
      sgsm.print(" I am in DANGER !! GPS link --  ");
      sgsm.print("https://maps.google.com/maps/place/");
      sgsm.print(flat, 6);
      sgsm.print(",");
      sgsm.println(flon, 6);
      delay(1000);
      sgsm.write(0x1A);
      delay(1000);
      Serial.print("Sent");
      digitalWrite(10,HIGH);
      digitalWrite(5,HIGH);
      delay(50);
      digitalWrite(10,LOW);
      digitalWrite(5,LOW);
      
}

void forStop()
{
  if(digitalRead(pushbutton) == LOW)
  {
    noTone(buzzer);
    flag=0;
    Serial.print("interrupt");
  }
}
