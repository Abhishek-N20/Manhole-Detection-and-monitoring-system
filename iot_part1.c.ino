#define LED 2 //Define blinking LED pin

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include "MQ135.h"
#include "ThingSpeak.h"

//#include<MQ2.h>

unsigned long starttimer = 0 , counter=0;
unsigned long myChannelNumber = 1422072;
const char * myWriteAPIKey = "LAFPC6XYW9R7GCL8";

String  str="";

WiFiClient  client;


#define DHTPIN 2          
#define DHTTYPE DHT11     
int pin=A0;
int data = 0;



char auth[] = "q0lFtEDUzQB9b4M_Ye2TTR-Q5tLwJRwl";
char ssid[] = "JioFiber-ptfPt";
char pass[] = "9739732141";

WidgetLCD lcd(V3);

MQ135 gasSensor = MQ135(pin);

DHT dht(DHTPIN, DHTTYPE);


int trigPin = 4; //D2
int echoPin = 0; //D3

BlynkTimer timer;

void setup() {

   WiFi.mode(WIFI_STA);
  Serial.begin(9600); 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LED, OUTPUT); // Initialize the LED pin as an output

   lcd.clear();

   Blynk.begin(auth, ssid, pass);
   dht.begin();
  timer.setInterval(1000L, sendSensor);

 ThingSpeak.begin(client);

}

void loop() {
  
  pinMode(LED, HIGH);
  delay(100);
  pinMode(LED, LOW);
   delay(100);

   Serial.println("hello");

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
  else
  {
       lcd.clear();
    lcd.print(0, 0, "Distance in cm");
    
     

       Serial.println("conncted");
       float t = dht.readTemperature();
       float h = dht.readHumidity();
       data = analogRead(pin); // smoke
       float ppm = gasSensor.getPPM(); // air_q
       
       Serial.println(t);
       Serial.println(h);

       long duration, distance;
      digitalWrite(trigPin,HIGH);
      delayMicroseconds(1000);
      digitalWrite(trigPin, LOW);
      duration=pulseIn(echoPin, HIGH);
      distance =(duration/2)/29.1;


       //delay(10000);
       ThingSpeak.setField(1,String(t));
       ThingSpeak.setField(2,String(h));  
       ThingSpeak.setField(3,String(ppm));
       ThingSpeak.setField(4,String(data));
       ThingSpeak.setField(6,String(distance));
       
       ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey) ;

       Blynk.run();
    timer.run();
        

  }

}

void email_send()
{
  Blynk.email("t9035755@gmail.com", "ESP8266 Alert",str);
  return ;
}

void sendSensor()
{
  //Temperature Sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  Blynk.virtualWrite(V0, h);
  Blynk.virtualWrite(V1, t);

  data = analogRead(pin); 
  Blynk.virtualWrite(V2, data);

  
  float ppm = gasSensor.getPPM();
  Blynk.virtualWrite(V6, ppm);
  
  int tflag=0, smflag=0,ppmflag=0;
  
  long duration, distance;
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin, LOW);
  duration=pulseIn(echoPin, HIGH);
  distance =(duration/2)/29.1;
  Serial.print(distance);
  Serial.println("CM");
  delay(10);
  lcd.clear();
  lcd.print(0, 0, "Distance in cm");
  lcd.print(7, 1, distance);


  
  if(t<=50 || ppm >50 || data>400)
  {
    if(counter==0 || millis() - starttimer >=60000)
    {
     
      Serial.println(millis());
      Serial.println(starttimer);
      
      if(t<=50)
      {
        tflag=1;
        str+="temp over 50 \n";
      }
  
      if(ppm>50)
      {
        str+="air quality degraded\n";
      }
  
      if(data>400)
      {
         str+="smoke detected \n";
      }
  
      if(distance<10)
      {
        str+="water about to overflow \n";
      }
  
      //timer.setInterval(32500L, email_send);
      
      Blynk.email("t9035755@gmail.com", "ESP8266 Alert",str);
      
      counter++;
      starttimer = millis();
    }
  }  
   
  
}
