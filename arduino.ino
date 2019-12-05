/* 
* DESENVOLVIDO POR:
*   IGOR LUCIO ROCHA ALVES
*   UFV - UNIVERSIDADE FEDERAL DE VICOSA
*   CAMPUS RIO PARANAIBA
* CONTATO:
*   igor.lucio@ufv.br
*   igorsk89@gmail.com
*   twitter.com/iguit0
*/

//Inclusao de bibliotecas
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <FirebaseArduino.h>
#include <Ticker.h>

//Definicao do SSID e da senha da rede sem fio
#define WIFI_SSID "nome da rede wifi"
#define WIFI_PASSWORD "senha da rede wifi"

//Definicao do Host e do Segredo do Firebase
#define FIREBASE_HOST "host firebase"
#define FIREBASE_AUTH "chave firebase"

//Definicao dos pinos de saida, tipo de sensor e tempo de intervalo de publicacao das leituras do sensor
#define SENSOR_PIN 0 // A0
#define PUBLISH_INTERVAL 1000*60*2 // Publicar a cada 2 minutos

//Definicao de constantes - seco e molhado
const int VAL_HIGH = 1024; // SECO
const int VAL_LOW = 800; // MOLHADO

//Criacao de objetos para o sensor e para o "ticker" e definicao da variavel para controle de publicao de dados
Ticker ticker;
bool publishNewState = true;

//Funcao para controle de publicacao de dados no banco de dados do Firebase
void publish(){
  publishNewState = true;
}

//Inicia-se a captura do sensor de solo
void setupPins(){
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

  // Registra o ticker para publicar de tempos em tempos
  ticker.attach_ms(PUBLISH_INTERVAL, publish);
}

void loop() {  
  // Para debugar
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
  
  // Condicional para controle de publicacao de acordo com o intervalo de publicacao definido
  if(publishNewState) {
    Serial.println("\tPublicar Novo Estado");
    
    //Verificao se valor e valido
    if(!isnan(fim)) {
      // Publicacao dos dados para o banco de dados do Firebase e exibicao os dados pela serial
      Firebase.pushInt("umidade", fim);
      Serial.print("\tSoil Moisture: ");
      Serial.print(fim);
      Serial.print("%");
      
      publishNewState = false;
    }else {
      Serial.println("Erro ao capturar umidade!");
      Serial.println("Erro ao tentar enviar dados ao Firebase");
    }
  }
  
  delay(100);
}