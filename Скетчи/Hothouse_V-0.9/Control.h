void StartFan() // Открытие и закрытие окон и печать их состояний на дисплей 
{
  if (Tnow >= (maxTempFanStart + (deltaT / 2)))
  {
    FlagTempFan = 1;
  }
  else
  {
    if (Tnow < (maxTempFanStart - (deltaT / 2)))
    {
      FlagTempFan = 0;
    }
  }
  if (FanWorkFlag == 1) {
    switch (FlagTimeFan) {
    case 1: {
      if ((currentMillis - TimeIntervalFaning) > TimeFaning) {
        FlagTimeFan = 0;
        TimeIntervalFaning = currentMillis;
      } break; }
    case 0: {
      if ((currentMillis - TimeFaning) > TimeFaningInterval) {
        FlagTimeFan = 1;
        TimeFaning = currentMillis;
      } break; }
    }
  }
       if (FlagTempFan == 1 || FlagTimeFan == 1 && windspeed < 15) { rservo.write(180); lservo.write(180);lcd.setCursor(19, 2); lcd.print("\6"); }
  else if (FlagTempFan == 1 || FlagTimeFan == 1 && windspeed < 10 && winddeg < 45  && winddeg > 135) { rservo.write(0); rservo.write(90);  lcd.setCursor(19, 2); lcd.print("\6"); }
  else if (FlagTempFan == 1 || FlagTimeFan == 1 && windspeed < 10 && winddeg > 315 && winddeg < 225) { lservo.write(0); lservo.write(90); lcd.setCursor(19, 2); lcd.print("\6"); }
  else { rservo.write(0); rservo.write(0); lcd.setCursor(19, 2); lcd.print(" "); }
}

void StartHot()                                                     // включение и отключение обогрева и печать их состояний на дисплей
{
  if (Tnow < (Temp - (deltaT / 2)))
  {
  myPID.SetTunings(consKp, consKi, consKd);
  myPID.Compute();
  analogWrite(PinHot, Output);
  lcd.setCursor(19, 0);
  lcd.print("\5");
  
}
  else
  {
    if (Tnow >= (Temp + (deltaT / 2)))
    {
      digitalWrite(PinHot, HIGH);
      if (Tnow < (Temp + (deltaT / 2)))
      {
        lcd.setCursor(19, 0);
        lcd.print(" ");
      }
    }
  }
}

void StartHum()
{ 
  if (hum < (Humiditi - (deltaHumiditi / 2)))
  {
    digitalWrite(PinHum, LOW);
    lcd.setCursor(19, 1);
    lcd.print("\7");
  
  }
  else
  {
    if (hum >(Humiditi + (deltaHumiditi / 2)))
    {
      digitalWrite(PinHum, HIGH);
      lcd.setCursor(19, 1);
      lcd.print(" ");

    }
  }
   if (TypeHouse == 0){
    SoilMoistureRead();
 if (HumGroundnow < (HumGround - (deltaHumiditi / 2)))
  {
    digitalWrite(PinHumGroundControl, LOW);
    lcd.setCursor(19, 1);
    lcd.print("\7");
  
  }
  else
  {
    if (HumGroundnow >(HumGround + (deltaHumiditi / 2)))
    {
      digitalWrite(PinHumGround, HIGH);
      lcd.setCursor(19, 1);
      lcd.print(" ");

    }
  }
 }
}

void StartLite()
{ 
   tmElements_t  tm;                             // specific time
  sm.init(OUR_timezone, OUR_latitude, OUR_longtitude);
  time_t sRise = sm.sunRise();
  time_t sSet  = sm.sunSet();
  
  if (sRise > RTC.hour * 3600 + RTC.minute * 60 + RTC.second)
  {
    digitalWrite(PinLite, LOW);
    lcd.setCursor(19, 1);
    lcd.print("\7");
  
  }
  else
  {
    if (sSet > RTC.hour * 3600 + RTC.minute * 60 + RTC.second)
    {
      digitalWrite(PinLite, HIGH);
      lcd.setCursor(19, 1);
      lcd.print(" ");

    }
  }
}
