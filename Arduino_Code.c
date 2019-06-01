// Wireless RFID Door Lock Using NodeMCU


#include <Wire.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ACROBOTIC_SSD1306.h>
#include <MFRC522.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define RST_PIN 20 // RST-PIN 
#define SS_PIN  2  // SDA-PIN
#define Relay 10
#define BUZZER_PIN  D8

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance




//LEDs
int ledPin = 16; //green LED
int ledPin2 = 5; //red LED

//Wireless name and password
const char* ssid     = "AS"; // wireless network name
const char* password = "ahmedsalem"; // wireless network password


WiFiClient client;

void setup() {
  pinMode(Relay, OUTPUT);
  digitalWrite(Relay,0);

  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,LOW);
  pinMode(ledPin2,OUTPUT);
  digitalWrite(ledPin2,LOW);
  pinMode(BUZZER_PIN,OUTPUT);
  digitalWrite(BUZZER_PIN,LOW);

  Serial.begin(115200);    // Initialize serial communications
  SPI.begin();           // Init SPI bus
  mfrc522.PCD_Init();    // Init MFRC522

  // We start by connecting to a WiFi network

  delay(5000);
  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
}

//Authorized Access

void authorize() {
  Serial.println("AUTHORIZED");
  digitalWrite(Relay,1);
  digitalWrite(ledPin,HIGH);
  delay(5000);              // wait for 5 seconds 
  digitalWrite(Relay,0);
  digitalWrite(ledPin,0);
}

//Unauthorized Access

void reject() {
    Serial.println("NOT AUTHORIZED");
    digitalWrite(ledPin2,HIGH);
    digitalWrite(BUZZER_PIN,HIGH);
    delay(2000);
    digitalWrite(ledPin2,0);
    digitalWrite(BUZZER_PIN,0);
   }

// Helper routine to dump a byte array as hex values to Serial
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void loop() {
 
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {   
    delay(50);
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {   
    delay(50);
    return;
  }

////-------------------------------------------------RFID----------------------------------------------////


  // Shows the card ID on the serial console
  String content= "";
  String content2 = "";
  
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     content2.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : "%20"));
     content2.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  Serial.println();
  content.toUpperCase();
  content2.toUpperCase();
  Serial.println("ID:" + content);
  
  

////-------------------------------------------------SERVER----------------------------------------------////

  
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
HTTPClient http;  

Serial.println("Checking access authorization for ID:" + content);  //print request destination

http.begin("http://iot-network.000webhostapp.com/write/writer.php?id=" + content2);  //Specify request destination

int httpCode = http.GET();    //Send the request
String payload = http.getString(); //Get the request response payload

  Serial.println(httpCode);   //Print HTTP return code
 
  if (httpCode > 0) { //Check the returning code
  
  
  if(payload=="yes"){
   authorize();
  }else{
    reject();
    }
}else{
  Serial.println("No Response from Server...");
  }
 
http.end();   //Close connection
 }else{
  Serial.println("Connection dropped reconnecting...");
  WiFi.begin(ssid, password);
  
  }
  
}
