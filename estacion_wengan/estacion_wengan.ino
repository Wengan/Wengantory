// Wengan

// Lyquid Cristal 
/*#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
*/
//sleep
/*
 * Sketch for testing sleep mode with wake up on WDT.
 * Donal Morrissey - 2011.
 *
 */
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

//#define LED_PIN (13)

// Sleep se aplica cada 8 segundos y se aplicaa un while segun la cantidad de ciclos (loop)
volatile int f_wdt=1;
volatile int Loops = 225; //30 min / 8 seg = 225

ISR(WDT_vect)
{
  if(f_wdt == 0)
  {
    f_wdt=1;
  }
  else
  {
    //Serial.println("WDT Overrun!!!");
  }
}

void enterSleep(void)
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  sleep_enable();
  
  /* Now enter sleep mode. */
  sleep_mode();
  
  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
}


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
float hum = 0, lluv = 0, rad = 0, temp = 0, wat = 0, bat=0, panel=0, mois;
// Moisture sensor cosas
const int waterlevel =A0;
// Drop 
#define DROPPIN A3
// Sensor de Radiacion
#define RADIACIONPIN A2
// Moisture
#define MOISTUREPIN A1
// Voltaje Bateria
#define BATERIA A4
// Voltaje paneles
#define PANELES A5
// Ecendido de Sensore
#define PowerPin 3
//Cantida de Intentos para subir datos
const int numIntentos=3;

void setup()
{
  // Conexion a SIM900
  mySerial.begin(19200);
  
  // Comunicacion con PC
  Serial.begin(9600);
  
  // Sensor DHT
  dht.begin();
  delay(500);
  
  /*** Setup the WDT ***/
  
  /* Clear the reset flag. */
  MCUSR &= ~(1<<WDRF);
  
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).
   */
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /* set new watchdog timeout prescaler value */
  WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
  
  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);

  //Setear PowerPin
  pinMode(PowerPin,OUTPUT);
  digitalWrite(PowerPin,LOW);
  
}

void loop()
{
  //Medir Sensores
  medirSensores();

  // Enviar datos 
  powerUp(); // Encender GPRS
  esperarRespuesta();

  for(int j=1;j<numIntentos;j++){ // Intentar una cantidad de numIntentos enviar los datos
    if (conectarAlaRed()){ 
      delay(100);
      if (subirDatos()){
        delay(100); break;
      }
    }
    
    delay(100);
  
  }
  powerDown();
  esperarRespuesta();

  //Enviar a dormir
  sleep();



}

void sleep(){
  if(f_wdt == 1)
  {
    /* Toggle the LED */
    //digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    
    /* Don't forget to clear the flag. */
    f_wdt = 0;
    
    /* Re-enter sleep mode. */
    for(int i=1;i<Loops;i++)
      {
      enterSleep(); 
      }
  }
  
}

boolean conectarAlaRed(){
double espera = 3000;
  mySerial.println("at+cipshut");
  Serial.println("at+cipshut");
  esperarRespuesta();
  delay(espera);

  while(!verificarRespuesta("+CGREG: 1,5")){
  Serial.println( "=== Checkeando conexion a la red GPRS === la respuesta debe ser +CGREG: 1,5 sino no funciona");
  mySerial.println("at+cgreg?");
  esperarRespuesta();
  delay(espera*2);
  }
//  if(verificarRespuesta("+CGREG: 1,5")){return true;}
//  else{return false;} 
  
  Serial.println(" === Attach la SIM === ");
  mySerial.println("at+cgatt=1");
  esperarRespuesta();
  delay(espera);
  
  mySerial.println("at+sapbr=3,1,\"Contype\",\"GPRS\"");
  esperarRespuesta();
  delay(espera);
  
  mySerial.println("at+sapbr=3,1,\"APN\",\"internet.simple\"");
  esperarRespuesta();
  delay(espera);
  
  mySerial.println("at+sapbr=3,1,\"USER\",\"\"");
  esperarRespuesta();
  delay(espera);
  
  mySerial.println("at+sapbr=3,1,\"PWD\",\"\"");
  esperarRespuesta();
  delay(espera);
  
  mySerial.println("at+sapbr=1,1");
  esperarRespuesta();
  delay(espera);

  return true;
}

boolean subirDatos(){
  delay(5000);
  mySerial.println("at+httpinit");
  delay(5000);
  esperarRespuesta();
  
  mySerial.println("at+httppara=\"CID\",1");
  esperarRespuesta();
  delay(5000);
  
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
//  mySerial.print("&bat=");
//  mySerial.print(bat);
//  mySerial.print("&panel=");
//  mySerial.print(panel);
  mySerial.println("\"");
  delay(5000);
  esperarRespuesta();
  
  mySerial.println("at+httpaction=0");
  delay(5000);
  esperarRespuesta();
  boolean f_subirDatos = false;
  if(verificarRespuesta("+HTTPACTION:0,200,10")){f_subirDatos = true;}
//  else{return false;} 
    
  mySerial.println("at+httpterm");
  esperarRespuesta();
  return f_subirDatos;
}

void medirSensores(){
  //Encender Sensores
  digitalWrite(PowerPin,HIGH);
  delay(100);
  
  hum = dht.readHumidity(); //se lee la humedad
  temp = dht.readTemperature(); // se lee la temperatura
  wat=analogRead(waterlevel);
  rad = analogRead(RADIACIONPIN);
  lluv = analogRead(DROPPIN);
  mois = analogRead(MOISTUREPIN); 
  bat = analogRead(BATERIA);
  panel = analogRead(PANELES);
  //Apagar Sensores
  digitalWrite(PowerPin,LOW);
  
}

