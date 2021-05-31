//Importamos librerias
#include <rgb_lcd.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

//Definimos el LCD
rgb_lcd lcd;

//Definimos el Servo
Servo myservo;

//Creamos la instancia del MFRC522
const int RST_PIN = 5;        // Pin 9 para el reset del RC522
const int SS_PIN = 53;        // Pin 10 para el SS (SDA) del RC522
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Crear instancia del MFRC522

//Definimos las tarjetas válidas
byte validKey1[4] = { 0x87, 0x7F, 0x95, 0xC8 };
byte validKey2[4] = { 0xC7, 0x37, 0x0C, 0xD9 };

//Variables globales
int pos = 0;
int buzzer = 3;
int ledPinAbrir = 4;
unsigned long previousMillis = 0;
const long interval = 1000;
int ledState = LOW;
int condicion = 0;

//Funciones complementarias

void lcdSetup(){
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Introduzca");
  lcd.setCursor(0, 1);
  lcd.print("la tarjeta:");
}

void lcdPrint(const char *str){
  lcd.clear();
  lcd.print(str);
}

void servoSetup(){
  myservo.attach(9); // conectar el servo al pin 9
}

void giroServo(){
  for (pos = 0; pos <= 90; pos += 1) { // girará de 0 a 90 grados
    // in steps of 1 degree
    myservo.write(pos);              // guardamos la nueva posicion del servo
    delay(15);                       // velocidad del giro
  }
  delay(5000);
  for (pos = 90; pos >= 0; pos -= 1) { // girará de 90 a 0 grados
    myservo.write(pos);              // guardamos la nueva posicion del servo
    delay(15);                       // velocidad del giro
  }
}

void pitido(){
  tone(buzzer,1000);
  delay(250);
  noTone(buzzer);
  delay(250);
}

void parpadeo(){
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(ledPinAbrir, ledState);
  }
}

bool isEqualArray(byte* arrayA, byte* arrayB, int length)
{
  for (int index = 0; index < length; index++)
  {
    if (arrayA[index] != arrayB[index]) return false;
  }
  return true;
}

void rfidSetup(){
  Serial.begin(9600); // Iniciar serial
  SPI.begin();        // Iniciar SPI
  mfrc522.PCD_Init(); // Iniciar MFRC522
}

void rfidFiltrar(){
  // Detectar tarjeta
  if (mfrc522.PICC_IsNewCardPresent())
  {
    //Seleccionamos una tarjeta
    if (mfrc522.PICC_ReadCardSerial())
    {
      // Comparar ID con las claves válidas
      if ((isEqualArray(mfrc522.uid.uidByte, validKey1, 4)) or (isEqualArray(mfrc522.uid.uidByte, validKey2, 4))){
        lcdPrint("Valido");
        condicion = 1;
      }else{
        lcdPrint("Invalido");
        condicion = 0;

      // Finalizar lectura actual
      mfrc522.PICC_HaltA();
    }
  }
  delay(250);
}
}

//Funciones principales de Arduino

void setup() {
  pinMode(buzzer,OUTPUT);
  pinMode(ledPinAbrir, OUTPUT);
  rfidSetup();
  servoSetup();
  lcdSetup();
}

void loop() {
  // put your main code here, to run repeatedly:
    rfidFiltrar();
    if (condicion == 1){
      pitido();
      giroServo();
      parpadeo();
      condicion = 0;
    }
}
