void BME280Read()                                                     // Чтение значений с датчика BME280
{ 
  Serial.println(F("BME280 test"));
    bool status;
      status = bme.begin();  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
    
    Serial.println("-- Default Test --");
  
  
  if (currentMillis - BME280readMillis > BME280interval)
  {
    BME280readMillis = currentMillis;
    Tnow = bme.readTemperature();
    Press = bme.readPressure() / 100.0F;  
    hum = bme.readHumidity();
 }
}

void SoilMoistureRead(){
  if (currentMillis - I2CGroundreadMillis > I2CGroundinterval)
  {
  I2CGroundreadMillis = currentMillis;
  digitalWrite(PinHumGroundControl, HIGH);   
  HumGroundnow = ads.readADC_SingleEnded(2);
  HumGroundnow = map(HumGroundnow, 0, 1000, 0, 100);  // адаптируем значения от 0 до 100,
 
  digitalWrite(PinHumGroundControl, LOW);   
  
 } 
}

void PressKeyMenu()                                                                      // Вычиление нажатия кнопок  
{
  PressingButtons = 0;
  buttons_Menu = ads.readADC_SingleEnded(0);
  Serial.print("  Resistant key button module="); Serial.print(buttons_Menu); Serial.println(" ");
        if (buttons_Menu >= 60000 )  PressingButtons = 1;           // меню       
  else  if (buttons_Menu > 10000 && buttons_Menu < 13000) PressingButtons = 2;     // вверх       
  else  if (buttons_Menu > 3000 && buttons_Menu < 7000) PressingButtons = 3;     // вниз      
  else  if ( buttons_Menu > 15000 && buttons_Menu < 17000)PressingButtons = 4;      // выбор      
  else  if (buttons_Menu > 800 && buttons_Menu < 900) PressingButtons = 5;  // открития окон       
  else  PressingButtons = 0; 
 Serial.print("  Button key =");Serial.print(PressingButtons); Serial.println(" ");delay(50);           
}
