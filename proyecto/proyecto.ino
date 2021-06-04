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
byte validKey2[4] = { 0x01, 0x02, 0x03, 0x04 };
byte validKey3[4] = { 0xF2, 0xFC, 0x73, 0xEC };

//Variables globales
int pos = 0;
int buzzer = 3;
int ledPinRojo = 4;
int ledPinVerde = 6;
int condicion = 0;

//Funciones complementarias

void lcdSetup(){
  lcd.clear();
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
  for (pos = 0; pos <= 100; pos += 1) { // girará de 0 a 90 grados
    // in steps of 1 degree
    myservo.write(pos);              // guardamos la nueva posicion del servo
    delay(15);                       // velocidad del giro
  }
  digitalWrite(ledPinRojo,LOW);
  digitalWrite(ledPinVerde,HIGH);
  delay(5000);
  for (pos = 100; pos >= 0; pos -= 1) { // girará de 90 a 0 grados
    myservo.write(pos);              // guardamos la nueva posicion del servo
    delay(15);                       // velocidad del giro
  }
  digitalWrite(ledPinVerde,LOW);
  digitalWrite(ledPinRojo,HIGH);
  lcdSetup();
}

void pitido(){
  tone(buzzer,1000);
  delay(250);
  noTone(buzzer);
  delay(250);
}

void pitidoErroneo(){
  tone(buzzer,400);
  delay(700);
  noTone(buzzer);
  delay(700);
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
      if ((isEqualArray(mfrc522.uid.uidByte, validKey1, 4)) or (isEqualArray(mfrc522.uid.uidByte, validKey2, 4)) or (isEqualArray(mfrc522.uid.uidByte, validKey3, 4))){
        Serial.println("valido");
        lcdPrint("Endavant");
        condicion = 1;
      }else{
        Serial.println("Tarjeta invalida");
        lcdPrint("Invalido");
        condicion = 0;
        pitidoErroneo();
        delay(2000);
        lcdSetup();

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
  pinMode(ledPinRojo, OUTPUT);
  pinMode(ledPinVerde, OUTPUT);
  digitalWrite(ledPinRojo,HIGH);
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
      condicion = 0;
    }
}
