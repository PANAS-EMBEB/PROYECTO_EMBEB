// La finalidad de esta práctica es establecer una conexión entre la placa Esp32 ( conectada a red wifi local ), y un dispositivo con internet ( Móvil, Pc, Tablet ) conectado a 
// internet en cualquier otra parte del mundo. Una vez realizada la conexión vamos a poder abrir o cerrar un relé, mediante una interfaz. Esto tiene muchísimas aplicaciones, 
// desde domotizar una bombilla a domotizar una puerta de garaje, obviamente con varias adaptaciones. En mi caso, la idea es aplicarlo a una puerta con apertura eléctrica, para poder
// salir de casa sin llaves, y poder abrir la puerta al volver o abrire la puerta a un amigo cuando estás de vacaciones. Obviamente, este proyecto nos puede parecer arriesgado, por carecer
// de seguridad al aplicarlo a la apet ura de una puerta, y los riesgos que ello conlleva. Para aumentar la seguridad, al realizar la conexión se va a pedir una contraseña para poder
// abrir la puerta.

#include <WiFi.h>
#include <Wire.h>  // Include the required Wire library for I2C / integrer la librairie Wire pour utiliser l I2C

// Esta librería hace posible la comunicacion por i2c con el Arduino Mega 2560
//--------------------------------------------
// Replace with your network credentials
const char* ssid = "MOVISTAR_BD3E";               //Nombre de nuestra red Wifi.
const char* password = "r2RUYTzQzXmuPsDodDVs";   //Contraseña de nuestra red wifi.
//--------------------------------------------
//--------------------------------------------

WiFiServer server(80);

String header;
unsigned long currentTime = millis(); 
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

String State = "off";

void setup() {
  Serial.begin(512000);
  WiFi.begin(ssid, password);
  server.begin();
  Serial.println(WiFi.localIP());
  Wire.begin();
}

void loop(){
  WiFiClient client = server.available();   

  if (client) {                             
    previousTime = currentTime;
    String currentLine = "";                
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  
      currentTime = millis();
      if (client.available()) {             
        char c = client.read();             
        header += c;
        if (c == '\n') {                    
          
          if (currentLine.length() == 0) {
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Wire.beginTransmission(0x09);  // transmit to device #9 / transmission sur l arduino secondaire a l adresse 0x09 (=9 en decimale)
              Wire.write(2); // sends x / envoi de la valeur de x
              Wire.endTransmission();        // stop transmitting / arret de la transmission
              State = "on";
            } else if (header.indexOf("GET /26/off") >= 0) {
              Wire.beginTransmission(0x09);  // transmit to device #9 / transmission sur l arduino secondaire a l adresse 0x09 (=9 en decimale)
              Wire.write(3); // sends x / envoi de la valeur de x
              Wire.endTransmission();        // stop transmitting / arret de la transmission
              State = "off";
            } else if (header.indexOf("GET /27/on") >= 0) {
              Wire.beginTransmission(0x09);  // transmit to device #9 / transmission sur l arduino secondaire a l adresse 0x09 (=9 en decimale)
              Wire.write(4); // sends x / envoi de la valeur de x
              Wire.endTransmission();        // stop transmitting / arret de la transmission
            } 
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: sans-serif; display: center; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; border-radius: 60px; padding: 20px 60px;");
            client.println("text-decoration: none; font-size: 35px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            client.println("<body><h1>Center Control Parking</h1>");
            
            client.println("<p>General ON-OFF</p>");
       
            if (State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">GENERAL ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">GENERAL OFF</button></a></p>");
            } 
            client.println("<p>Remote Open</p>");

            client.println("<p><a href=\"/27/on\"><button class=\"button\">OPEN</button></a></p>");

            client.println("</body></html>");
            
            client.println();
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
      }
    }
    header = "";
    client.stop();
  }
}
