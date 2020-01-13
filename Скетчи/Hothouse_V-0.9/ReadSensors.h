#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <OneWire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

 Adafruit_BME280 bme;
 Adafruit_ADS1115 ads; 



float BME280Read(String Type)                                                     // Чтение значений с датчика BME280
{ 

   switch(Type){
    case "Temp":
     return bme.readTemperature();
      break;
    case "Hum":
     return bme.readPressure() / 100.0F; 
      break;
    case "Press":
     return bme.readHumidity();
      break;
 }
}

int SoilMoistureRead()
{
  return map(ads.readADC_SingleEnded(2);, 0, 1000, 0, 100);  // адаптируем значения от 0 до 100,
}

float Power(char Type)
{
  voltage = ads.readADC_SingleEnded(1) * 0.185 / 1000 * 5.75;     //подсчет заряда аккумулятора
   switch(Type){
    case Volt:
      return voltage     //подсчет заряда аккумулятора
       break;
    case Net: 
     if( voltage > 13.5 ) netpower = 1;
     else netpower = 0;
      return netpower;
       break;
   } 
}

int PressKeyMenu()                                                                      // Вычиление нажатия кнопок  
{
 int PressingButtons = 0;
 double buttons_Menu = ads.readADC_SingleEnded(0);
  Serial.print("  Resistant key button module="); Serial.print(buttons_Menu); Serial.println(" ");
        if (buttons_Menu >= 60000 )  PressingButtons = 1;           // меню       
  else  if (buttons_Menu > 10000 && buttons_Menu < 13000) PressingButtons = 2;     // вверх       
  else  if (buttons_Menu > 3000 && buttons_Menu < 7000) PressingButtons = 3;     // вниз      
  else  if ( buttons_Menu > 15000 && buttons_Menu < 17000)PressingButtons = 4;      // выбор      
  else  if (buttons_Menu > 800 && buttons_Menu < 900) PressingButtons = 5;  // открития окон       
  else  PressingButtons = 0; 
 Serial.print("  Button key ="); Serial.print(PressingButtons); Serial.println(" "); delay(50);  
 return PressingButtons;
}
