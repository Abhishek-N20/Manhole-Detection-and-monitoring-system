#include<Wire.h>
#define BLYNK_PRINT Serial
#include <Blynk.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

WiFiClient  client;
 
char auth[] = "q0lFtEDUzQB9b4M_Ye2TTR-Q5tLwJRwl";
char ssid[] = "JioFiber-ptfPt";
char pass[] = "9739732141";
 
const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

unsigned long starttimer = 0 , counter=0;
 
int minVal=265;
int maxVal=402;
 
double x;
double y;
double z;
 
void setup(){
  WiFi.mode(WIFI_STA);
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
}

void loop(){

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }
  
  Blynk.run();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();
  int xAng = map(AcX,minVal,maxVal,-90,90);
  int yAng = map(AcY,minVal,maxVal,-90,90);
  int zAng = map(AcZ,minVal,maxVal,-90,90);
   
  x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
  y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
  z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);
   
  Serial.print("AngleX= ");
  Serial.println(x);
   
  Serial.print("AngleY= ");
  Serial.println(y);
   
  Serial.print("AngleZ= ");
  Serial.println(z);
  Serial.println("-----------------------------------------");
   
  Blynk.virtualWrite(V2, x);
  Blynk.virtualWrite(V3, y);
  Blynk.virtualWrite(V4, z);
  
  if(x>400 || x<300 || y>400 || y<300 || z>250 || z<100)
  {
    if(counter==0 || millis() - starttimer >=60000)
    {
      Blynk.email("t9035755@gmail.com", "ESP8266 Alert","TILT TILT TILT");
      Blynk.notify("TILT TILT TILT!"); 
      counter++;
      starttimer = millis();
    }
  }
   
  delay(1000);
}