#include <ReadSensors.h>
#include <PID_v1.h>
#include <Servo.h>
#include <Time.h>
#include <TimeLib.h>
#include <sunMoon.h>
#include <FastIO.h>
#include <I2CIO.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_SR.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_SR2W.h>
#include <LiquidCrystal_SR3W.h>

  Servo rservo;
  Servo lservo;
  PID myPID(&Tnow, &Output, &Temp, consKp, consKi, consKd, DIRECT);  //Specify the links and initial tuning parameters
 #define OUR_latitude    55.751244     // координаты Киева
 #define OUR_longtitude  37.618423
 #define OUR_timezone    120                     // localtime with UTC difference in minutes
  sunMoon  sm;

 #define SDA_PIN 4
 #define SCL_PIN 5

void StartFan(float Temp, float deltaT, float maxTempFanStart, float FanWorkFlag, float FlagTimeFan, float TimeFaning, float TimeIntervalFaning, float currentMillis) // Открытие и закрытие окон и печать их состояний на дисплей StartFan( Temp, deltaT, maxTempFanStart, FanWorkFlag, FlagTimeFan, TimeFaning, TimeIntervalFaning, currentMillis);               
{

  lcd.begin(20, 4);
  Wire.begin(SDA_PIN, SCL_PIN);
  rservo.attach(0);
  lservo.attach(2);
  float Tnow = BME280Read("Temp");
  
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
       if (FlagTempFan == 1 || FlagTimeFan == 1 && windspeed < 10 && winddeg < 45  && winddeg > 135) { rservo.write(180); rservo.write(180);  lcd.setCursor(19, 2); lcd.print("\6"); }
  else if (FlagTempFan == 1 || FlagTimeFan == 1 && windspeed < 10 && winddeg > 315 && winddeg < 225) { lservo.write(180); lservo.write(180);  lcd.setCursor(19, 2); lcd.print("\6"); }
  else { rservo.write(0); rservo.write(0); lcd.setCursor(19, 2); lcd.print(" "); }
}

void StartHot(float Temp, float deltaT, float PinHot, float Output, float consKp, float consKi, float consKd,)   // включение и отключение обогрева и печать их состояний на дисплей  StartHot(Temp, deltaT, PinHot, Output, consKp, consKi, consKd);                           
{
  lcd.begin(20, 4);
  Wire.begin(SDA_PIN, SCL_PIN);
  ads.begin();
  myPID.SetMode(AUTOMATIC);
    
  float Tnow = BME280Read("Temp");
  
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

void StartHum(int TypeHouse, int hum, int Humiditi, int deltaHuminiti, int HumGtound, int PinHumGroundControl)   //StartHum(TypeHouse, hum, Humiditi, deltaHuminiti, HumGtound, PinHumGroundControl);
{ 

  lcd.begin(20, 4);
  Wire.begin(SDA_PIN, SCL_PIN);
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
    int HumGroundnow = SoilMoistureRead();
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

void StartLite(int PinLite) //StartLite(PinLite);
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
