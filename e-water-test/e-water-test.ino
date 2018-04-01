/*  Arduino Tutorial - How to use a Soil Hygrometer Module
  Dev: Michalis Vasilakis // Date: 18/05/2016 // www.ardumotive.com */
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Set these to run example.

#define FIREBASE_HOST "e-water-c7960.firebaseio.com"
#define FIREBASE_AUTH "1AI8CBnuuNaEaRkK2I4WoBqs4rmdJWbMm6IF3Upe"
#define WIFI_SSID "ewater"
#define WIFI_PASSWORD "ewater18"

#define LED LED_BUILTIN

int stime=0;

void setup(){
  pinMode(LED,OUTPUT);
  digitalWrite(LED,1);

  Serial.begin(9600);
  
  
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
Serial.print("connecting");
while (WiFi.status() != WL_CONNECTED) {
  Serial.print(".");
  delay(500);
}
digitalWrite(LED,0);
Serial.println();
Serial.print("connected: ");
Serial.println(WiFi.localIP());
Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
delay(1000);

}

void loop(){
  digitalWrite(LED,0);
  Firebase.setInt("test", stime++);
  Serial.println("Save new Time value to firebase");
  // handle error
  if (Firebase.failed()) {
    digitalWrite(LED,1);
    Serial.print("setting /number failed:");
    Serial.println(Firebase.error());
    delay(400);
    return;
  }



  delay(2000); //Read every 2 sec.
}


