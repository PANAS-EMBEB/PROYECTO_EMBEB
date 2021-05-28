//Importamos librerias
#include <rgb_lcd.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

//Definimos el LCD
rgb_lcd lcd;

//Definimos el Servo
Servo myservo;

//Variables globales
int pos = 0;
int buzzer = 3;
int ledPinAbrir = 4;
unsigned long previousMillis = 0;
const long interval = 1000;
int ledState = LOW;


//Funciones complementarias

void lcdSetup(){
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Introduzca");
  lcd.setCursor(0, 1);
  lcd.print("la tarjeta:");
}

void lcdPrint(const char *str){
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

//Funciones principales de Arduino

void setup() {
  // put your setup code here, to run once:
  pinMode(buzzer,OUTPUT);
  pinMode(ledPinAbrir, OUTPUT);
  servoSetup();
  lcdSetup();
}

void loop() {
  // put your main code here, to run repeatedly:
  parpadeo();
}
