// Wengan

// Lyquid Cristal 
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

// DHT cosas
#include "DHT.h" 
#define DHTPIN 2 
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE); 

// Cosas SIM900
#include <SoftwareSerial.h>
SoftwareSerial mySerial(7,8);

// cosas Phant
const String PUBLIC_KEY = "0lK0GjXQqRu7p2DW9xdV";
const String PRIVATE_KEY = "D6YBNeP8MZsMgKzjWxZ6";

// Variables
float hum = 0, lluv = 0, rad = 0, temp = 0, wat = 0, mois;
// Moisture sensor cosas
const int waterlevel =A0;
// Drop 
#define DROPPIN A3
// Sensor de Radiacion
#define RADIACIONPIN A2
// Moisture
#define MOISTUREPIN A1

void setup()
{
  // Conexion a SIM900
  mySerial.begin(19200);
  
  // Comunicacion con PC
  Serial.begin(9600);
  
  // Sensor DHT
  dht.begin();
  delay(500);
}

void loop()
{
  //Conectarse a la red
  medirSensores();
  conectarAlaRed();
  subirDatos();
  sleep();

}
void sleep(){
  delay(300000);
}
void conectarAlaRed(){

  mySerial.println("at+cipshut");
  esperarRespuesta();
  
  Serial.println( "=== Checkeando conexion a la red GPRS === la respuesta debe ser +CGREG: 1,5 sino no funciona");
  mySerial.println("at+cgreg?");
  esperarRespuesta();
  
  Serial.println(" === Attach la SIM === ");
  mySerial.println("at+cgatt=1");
  esperarRespuesta();
  
  mySerial.println("at+sapbr=3,1,\"Contype\",\"GPRS\"");
  esperarRespuesta();
  
  mySerial.println("at+sapbr=3,1,\"APN\",\"internet.simple\"");
  esperarRespuesta();
  
  mySerial.println("at+sapbr=3,1,\"USER\",\"\"");
  esperarRespuesta();
  
  mySerial.println("at+sapbr=3,1,\"PWD\",\"\"");
  esperarRespuesta();
  
  mySerial.println("at+sapbr=1,1");
  esperarRespuesta();
}

void subirDatos(){
  mySerial.println("at+httpinit");
  esperarRespuesta();
  
  mySerial.println("at+httppara=\"CID\",1");
  esperarRespuesta();
  
  mySerial.print("at+httppara=\"URL\",\"data.sparkfun.com/input/");
  mySerial.print(PUBLIC_KEY);
  mySerial.print("?private_key=");
  mySerial.print(PRIVATE_KEY);
  mySerial.print("&hum=");
  mySerial.print(hum);
  mySerial.print("&lluv=");
  mySerial.print(lluv);
  mySerial.print("&rad=");
  mySerial.print(rad);
  mySerial.print("&temp=");
  mySerial.print(temp);
  mySerial.print("&wat=");
  mySerial.print(wat);
  mySerial.println("\"");
  esperarRespuesta();
  
  mySerial.println("at+httpaction=0");
  esperarRespuesta();
  
  mySerial.println("at+httpterm");
  esperarRespuesta();
}

void medirSensores(){
  hum = dht.readHumidity(); //se lee la humedad
  temp = dht.readTemperature(); // se lee la temperatura
  wat=analogRead(waterlevel);
  rad = analogRead(RADIACIONPIN);
  lluv = analogRead(DROPPIN);
  mois = analogRead(MOISTUREPIN); 
}

