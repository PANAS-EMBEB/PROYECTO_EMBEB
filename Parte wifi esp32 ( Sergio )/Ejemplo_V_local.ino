// LA finalidad de esta práctica es establecer una conexión entre la placa Esp32 ( conectada a red wifi local ), y un dispositivo con internet ( Móvil, Pc, Tablet ) conectado a 
// internet en cualquier otra parte del mundo. Una vez realizada la conexión vamos a poder abrir o cerrar un relé, mediante una interfaz. Esto tiene muchísimas aplicaciones, 
// desde domotizar una bombilla a domotizar una puerta de garaje, obviamente con varias adaptaciones. En mi caso, la idea es aplicarlo a una puerta con apertura eléctrica, para poder
// salir de casa sin llaves, y poder abrir la puerta al volver o abrire la puerta a un amigo cuando estás de vacaciones. Obviamente, este proyecto nos puede parecer arriesgado, por carecer
// de seguridad al aplicarlo a la apetura de una puerta, y los riesgos que ello conlleva. Para aumentar la seguridad, al realizar la conexión se va a pedir una contraseña para poder
// abrir la puerta.

#include <WiFi.h>
//--------------------------------------------
// Replace with your network credentials
const char* ssid = "MOVISTAR_BD3E";          //Nombre de nuestra red Wifi.
const char* password = "r2RUYTzQzXmuPsDodDVs";                   //Contraseña de nuestra red wifi.
//--------------------------------------------
//--------------------------------------------

WiFiServer server(80);
unsigned long T_Control=0;      
String header;
unsigned long currentTime = millis(); 
unsigned long previousTime = 0; 
const long timeoutTime = 2000;


// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 2;
const int output27 = 3;

void setup() {
  Serial.begin(115200UL);
  Serial2.begin(115200UL);
  pinMode(output26, OUTPUT);
  digitalWrite(output26, LOW);
  pinMode(output27, OUTPUT);
  digitalWrite(output27, LOW);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   

  if (client) {                             
    previousTime = currentTime;
    Serial.println("New Client.");         
    String currentLine = "";                
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  
      currentTime = millis();
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                    
          
          if (currentLine.length() == 0) {
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /26/on") >= 0) {
              Serial.println("GPIO 26 on");
              output26State = "on";
              digitalWrite(output26, HIGH);
              Serial.println("1");
              Serial2.write(1);
            } else if (header.indexOf("GET /26/off") >= 0) {
              Serial.println("GPIO 26 off");
              output26State = "off";
              digitalWrite(output26, LOW);
              Serial.println("2");
              Serial2.write(2);

            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              digitalWrite(output27, HIGH);
              Serial.println("3");
              Serial2.write(3);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              digitalWrite(output27, LOW);
              Serial.println("4");
              Serial2.write(4);
            } 
            
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: sans-serif; display: center; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 20px 60px;");
            client.println("text-decoration: none; font-size: 35px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            client.println("<body><h1>Principal Door</h1>");
            
                   
            if (output26State=="off") {
              client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            client.println("<p>GPIO 27 - State " + output27State + "</p>");
            // If the output27State is off, it displays the ON button       
            if (output27State=="off") {
              client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
               
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
