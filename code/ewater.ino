/*  Arduino Tutorial - How to use a Soil Hygrometer Module
  Dev: Michalis Vasilakis // Date: 18/05/2016 // www.ardumotive.com */
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <FirebaseArduino.h>

#define FIREBASE_HOST "e-water-c7960.firebaseio.com"
#define FIREBASE_AUTH "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#define WIFI_SSID "ewater" //ewater
#define WIFI_PASSWORD "87654321" //87654321

//char ssid[] = "ewater";  //  your network SSID (name)
//char pass[] = "87654321";       // your network password

#define LED LED_BUILTIN
#define RelayPin D1


// Arduino UNO Pins:
//#define IN1 8
//#define IN2 9
//#define IN3 10
//#define IN4 11

// ESP-12F(WeMos D1 mini)

#define IN1 14 //GPIO 14
#define IN2 12 //GPIO 12
#define IN3 13 //GPIO 13
#define IN4 15 //GPIO 15


//Constants 
const int hygrometerPin = A0;  //Hygrometer sensor analog pin output at pin A0 of Arduino
//Variables 
int value, oldValue;
boolean flash=0;
int dropsR;

int retries=0;


String dropsS;

unsigned long lastTime = 0L;
unsigned long timeS = 0L;
int timeDrop=400;
int i;


void setup(){
  pinMode(LED,OUTPUT);
  pinMode(RelayPin,OUTPUT);
  digitalWrite(LED,0);

  Serial.begin(9600);

// simple wifi connection

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
Serial.print("connecting");
while (WiFi.status() != WL_CONNECTED && retries<50) {
      retries++;
  Serial.print(".");
  delay(500);
}
  Serial.println("WiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

if (WiFi.status() != WL_CONNECTED) {
// Smart Config
  /* Set ESP8266 to WiFi Station mode */
  WiFi.mode(WIFI_STA);
  /* start SmartConfig */
  WiFi.beginSmartConfig();

  /* Wait for SmartConfig packet from mobile */
  Serial.println("Waiting for SmartConfig.");
  while (!WiFi.smartConfigDone() && retries<40) {
    retries++;
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("SmartConfig done.");

  /* Wait for WiFi to connect to AP */
  Serial.println("Waiting for WiFi");
  while (WiFi.status() != WL_CONNECTED && retries<50) {
    retries++;
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

Serial.println();
Serial.print("connected: ");
Serial.println(WiFi.localIP());
Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
delay(1000);

// check pump
  digitalWrite(RelayPin,HIGH);
  delay(1000);
  digitalWrite(RelayPin,LOW);

}

void loop(){
  // When the plant is watered well the sensor will read a value 380~400, I will keep the 400 
  // value but if you want you can change it below. 
  value = analogRead(hygrometerPin);   //Read analog value 
  value = constrain(value,100,1023);  //Keep the ranges!
  value = map(value,100,1023,100,0);  //Map value : 400 will be 100 and 1023 will be 0
  Serial.print("Soil humidity: ");
  Serial.print(value);
  Serial.println("%");

  // flashing led when system is ready for watering
  // glastra1/e_water/lowLevel is the path on firebase database
  if (value < Firebase.getInt("glastra1/e_water/lowLevel")) {  // enable watering if it's below 
      (flash == 0) ? (flash=1) : (flash=0);
      digitalWrite(LED,flash);
      checkWatering();
  } else  { 
    digitalWrite(LED,0); 
  }

// set value to firebase
if (oldValue != value ) {
  Firebase.setInt("glastra1/e_water/Hygro", value);
  Serial.println("Save new Hygro value to firebase");
  // handle error
  if (Firebase.failed()) {
    Serial.print("setting /number failed:");
    Serial.println(Firebase.error());
    return;
  }
oldValue = value;
}

  delay(1000); //Read every 2 sec.
}

void checkWatering(){
//watering command received  get value 
  dropsS = Firebase.getString("glastra1/e_water/Vol");  //get value
  Firebase.setString("glastra1/e_water/Vol","0");       //reset for next reading
  dropsR = dropsS.toInt();
  if (dropsR>0) {
  Serial.print("Drops received from firebase :");
  Serial.println(dropsR);
  drops(dropsR); }
}

void drops(int noDrops) {

Serial.println("Start watering to drop No drops");
  int timeLeft = timeDrop*noDrops;
  digitalWrite(RelayPin,HIGH);
  delay(timeLeft);
  digitalWrite(RelayPin,LOW);
}

