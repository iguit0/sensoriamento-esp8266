#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <FirebaseArduino.h>
#include <Ticker.h>

// Configuration variables
#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define FIREBASE_HOST "FIREBASE_HOST"
#define FIREBASE_AUTH "FIREBASE_AUTH"

// Definition of output pins, sensor type and interval time for publishing sensor readings
#define SENSOR_PIN 0 // A0
#define PUBLISH_INTERVAL 1000*60*2 // Publicar a cada 2 minutos

// Constants
const int VAL_HIGH = 1024; // DRY
const int VAL_LOW = 800; // WET

// Creation of objects for the sensor and ticker and definition of the variable to control data publication
Ticker ticker;
bool publishNewState = true;

// Function to control data publication in the Firebase database
void publish(){
  publishNewState = true;
}

// Soil sensor capture starts
void setupPins() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
}

void setupWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.print("Conectado na rede: ");
  Serial.println(WIFI_SSID);
  Serial.print("Endereco IP: ");
  Serial.println(WiFi.localIP());
}

void setupFirebase(){
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  if (Firebase.failed()) Serial.println(Firebase.error());
  Firebase.setInt("umidade", 0);
}

void setup() {
  Serial.begin(115200);

  setupPins();
  setupWifi();    
  setupFirebase();

  // Register the ticker to publish from time to time
  ticker.attach_ms(PUBLISH_INTERVAL, publish);
}

void loop() {  
  // To DEBUG
  float valorADC = analogRead(SENSOR_PIN);
  Serial.print("\tRaw Value: ");
  Serial.print(valorADC);
  Serial.println(" ");
  int res = (((valorADC - VAL_LOW) * 100) / (VAL_HIGH - VAL_LOW));
  int fim = 100 - res;

  if(fim >= 100) fim = 100;
  if(fim <= 0) fim = 0;

  if(!isnan(fim)){
    delay(300);
    Serial.print("\tSoil Moisture: ");
    Serial.print(fim);
    Serial.print("%");
  }
  
  // Conditional for publication control according to the defined publication interval
  if(publishNewState) {
    Serial.println("\tPublicar Novo Estado");
    
    // Checking if value is valid
    if(!isnan(fim)) {
      // Publishing data to the Firebase database and displaying the data via serial
      Firebase.pushInt("umidade", fim);
      Serial.print("\tSoil Moisture: ");
      Serial.print(fim);
      Serial.print("%");
      
      publishNewState = false;
    } else {
      Serial.println("Error capturing moisture!");
      Serial.println("Error when trying to send data to Firebase");
    }
  }
  
  delay(100);
}
