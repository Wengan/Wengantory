/* Imprime en pantalla el string c, en la linea a in (1,2)
en el caracter b in (0..16)
Hay que llamar a las librerías

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

ANTES del setup hay que iniciar:
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address

*/

void mostrarpantalla(int a, int b, String c){
 lcd.backlight();
 delay(100);
 lcd.setCursor(a,b);
 lcd.print(c);
 delay(2000);
 lcd.noBacklight();
 }
 
void esperarRespuesta()
{
  while(mySerial.available()==0) continue;
   while(mySerial.available()!=0)
   Serial.write(mySerial.read());
}

