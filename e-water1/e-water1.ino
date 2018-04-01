/*  Arduino Tutorial - How to use a Soil Hygrometer Module
  Dev: Michalis Vasilakis // Date: 18/05/2016 // www.ardumotive.com */
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Set these to run example.

#define FIREBASE_HOST "e-water-c7960.firebaseio.com"
#define FIREBASE_AUTH "1AI8CBnuuNaEaRkK2I4WoBqs4rmdJWbMm6IF3Upe"
#define WIFI_SSID "AndroidAP" //"Home"
#define WIFI_PASSWORD "bght0836" //"23456789"

#define LED LED_BUILTIN


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
const int hygrometer = A0;  //Hygrometer sensor analog pin output at pin A0 of Arduino
//Variables 
int value, oldValue;
boolean flash=0;
int dropsR;

const int NBSTEPS = 400;
const int STEPTIME = 2000;   //μS 2000 motor - 200000 led
int Step = 0;
int phases = 8;
boolean Clockwise = true;
String dropsS;
int arrayDefault[4] = {LOW, LOW, LOW, LOW};

int stepsMatrix[8][4] = {
  {LOW, LOW, LOW, HIGH},
  {LOW, LOW, HIGH, HIGH},
  {LOW, LOW, HIGH, LOW},
  {LOW, HIGH, HIGH, LOW},
  {LOW, HIGH, LOW, LOW},
  {HIGH, HIGH, LOW, LOW},
  {HIGH, LOW, LOW, LOW},
  {HIGH, LOW, LOW, HIGH},
};

int stepsMatrix_[4][4] = {
  {LOW, LOW, LOW, HIGH},
  {LOW, LOW, HIGH, LOW},
  {LOW, HIGH, LOW, LOW},
  {HIGH, LOW, LOW, LOW}
};

unsigned long lastTime = 0L;
unsigned long timeS = 0L;
int stepsDrop=400;
int i;

void writeStep(int outArray[4]);
void stepper();
void setDirection();

void setup(){
  pinMode(LED,OUTPUT);
  digitalWrite(LED,0);

  Serial.begin(9600);
  
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
Serial.print("connecting");
while (WiFi.status() != WL_CONNECTED) {
  Serial.print(".");
  delay(500);
}

Serial.println();
Serial.print("connected: ");
Serial.println(WiFi.localIP());
Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
delay(1000);

}

void loop(){
  // When the plant is watered well the sensor will read a value 380~400, I will keep the 400 
  // value but if you want you can change it below. 
  value = analogRead(hygrometer);   //Read analog value 
  value = constrain(value,400,1023);  //Keep the ranges!
  value = map(value,400,1023,100,0);  //Map value : 400 will be 100 and 1023 will be 0
  Serial.print("Soil humidity: ");
  Serial.print(value);
  Serial.println("%");

  // light led when system is ready for watering
  if (value < Firebase.getInt("glastra1/e_water/lowLevel")) {
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

Serial.println("Move stepper to drop drops");
  unsigned long currentMicros;
  int stepsLeft = 400*noDrops;
  timeS = 0;
  lastTime = micros();
  while (stepsLeft > 0) {
    currentMicros = micros();
    if (currentMicros - lastTime >= STEPTIME) {
      stepper();
      timeS += micros() - lastTime;
      lastTime = micros();
      stepsLeft--;
    }
    delay(1);
  }
  
}

void writeStep(int outArray[4]) {
  digitalWrite(IN1, outArray[0]);
  digitalWrite(IN2, outArray[1]);
  digitalWrite(IN3, outArray[2]);
  digitalWrite(IN4, outArray[3]);
}

void stepper() {
  if ((Step >= 0) && (Step < phases)) {
    writeStep(stepsMatrix[Step]);
  } else {
    writeStep(arrayDefault);
  }
  setDirection();
}

void setDirection() {
  (Clockwise == true) ? (Step++) : (Step--);

  if (Step > phases-1) {
    Step = 0;
  } else if (Step < 0) {
    Step = phases-1;
  }
}

