#include <OneWire.h>
#include <DS1307new.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Servo.h>
#include <EEPROM.h>
#include <FastIO.h>
#include <I2CIO.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_SR.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_SR2W.h>
#include <LiquidCrystal_SR3W.h>
#define EEPROM_ADDR 0x50
#include <PID_v1.h>
#include <ArduinoJson.h>
#include <MQTT.h>
#include <PubSubClient.h>
#include <PubSubClient_JSON.h>
#include <ESP8266WiFi.h>
#include <stdlib.h>
#include <Time.h>
#include <TimeLib.h>
#include <sunMoon.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>

#include <Control.h>
#include <MyWiFi.h>
#include <MyEEPROM.h>
#include <Menu.h>
#include <ReadSensors.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).

#define ssid  "Kyivstar_Alpha"  // Имя вайфай точки доступа
#define pass  "R667fksX55fr" // Пароль от точки доступа

#define mqtt_server "m21.cloudmqtt.com" // Имя сервера MQTT
#define  mqtt_port 19787 // Порт для подключения к серверу MQTT
#define mqtt_user "udciowys" // Логи от сервер
#define mqtt_pass "ZXBwDFYgAU_K" // Пароль от сервера
#define mqtt_id "HotHouse" // Пароль от сервера

#define host "api.openweathermap.org"
#define wapi  "GET /data/2.5/weather?q=Kyiv,ua&appid=c557d50e325f16c912651e1054527de7 HTTP/1.1"
#define httpPort  80

#define OUR_latitude    55.751244     // координаты Киева
#define OUR_longtitude  37.618423
#define OUR_timezone    120                     // localtime with UTC difference in minutes
sunMoon  sm;

#define SDA_PIN 4
#define SCL_PIN 5

String line;
int NumBankSave = 0; 
int IfBankSave  = 0;
  
Servo rservo;
Servo lservo;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  //LiquidCrystal lcd(11, 10, 9, 8, 7, 6);   инициализация входов на дисплей 

WiFiClient wclient; // Use WiFi_funkClient class to create TCP connections
PubSubClient client(wclient, mqtt_server, mqtt_port);

Adafruit_BME280 bme;

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

// Must declare output stream before Adafruit_ESP8266 constructor; can be
// a SoftwareSerial stream, or Serial/Serial1/etc. for UART.

int BS, BL, BankSave;                                                 // Переменые для сохранения настроек 
double Temp = 37.7, deltaT = 0.2;               // температура выращивания, дельта Т
#define maxdeltaT 2                                        // максимальное значение дельтаТ для меню
#define mindeltaT 0.1                                      // минимальное  значение дельтаТ для меню
double maxTempFanStart = 37.9;                                        // максимально допустимая температура при которой включается аварийная продувка
#define maxTempFanStartMenuMin 16                                     // максимальное значение температуры работы вентилятора для меню
#define maxTempFanStartMenuMax 61                                     // минимальное значение температуры работы вентилятора для меню

int TimeFanWork = 60;                                                 // время проветривания теплицы от СО2 в секундах
#define maxTimeFanWork 900                                           // максимальное время проветривания теплицы от СО2 для меню в секундах
#define minTimeFanWork 5                                              // минимальное время проветривания теплицы от СО2 для меню в секундах
#define FadeTimeFanWork 1                                             // шаг изменения значения времени работы вентилятора в меню в секундах
int TimeIntervalFanWork = 30;                                         // интервал между включениями проветривания теплицы от СО2 в минутах
#define maxTimeIntervalFanWork 300                                    // максимальный интервал между включениями проветривания теплицы от СО2 для меню в минутах
#define minTimeIntervalFanWork 1                                      // минимальный интервал между включениями проветривания теплицы от СО2 для меню в минутах
#define FadeTimeIntervalFanWork 1                                     // шаг изменения значения интервала между включениями проветривания теплицы от СО2 в меню в минутах
int FanWorkFlag = 1;                                                  // флаг активности продувки теплицы от СО2

#define maxTempDanger 70                                   // максимальная температура выращивания для меню
#define minTempDanger 20                                   // минимальная температура выращивания для меню
#define FadeAmountTemp 0.1                                            // шаг изменения тепертуры в меню
int Humiditi = 60, deltaHumiditi = 1;             // влажность выращивания дельта влажности
int HumGroundnow,HumGround = 60;                                    // влажность почвы и тип теплицы
#define mindeltaHum 1                                                 // минимальная дельта влажности для меню
#define maxdeltaHum 10                                                // максимальная дельта влажности для меню
#define FadeAmountdeltaHum 1                                         // шаг изминения дельты влажности для меню
#define FadeAmountHum 1                                               // шаг изменения влажности в меню
#define MaximumHumiditi 100                                  // максимальное значение влажности для меню
#define MinimumHumiditi 0                                 // минимальное значение влажности для меню

float tempK,tempC,tempKmin,tempCmin,tempKmax,tempCmax ;                  // достаем температуру из структуры main

int pressurehPa;          // достаем давление из структуры main
float pressure;              // давление окружения
double Press ;               // давление внутри
int humidity ;             // достаем влажность из структуры main
float windspeed;             // достаем скорость ветра из структуры main
int winddeg;                   // достаем направление ветра из структуры main




int i = 0; int k = 0;
double Tnow;                                                          // Реальная температура на данный момент в теплице на BME280
int hum;                                                              // Реальная влажность на данный момент в теплице на BME280
int TypeHouse = 1;
int VarHouse[3] ={0,1,2};
int MainMenu = 0, SubMenu = 0, FlagMenu = 0;                          // переменные для управления меню
int NOWyear, NOWmonth, NOWday, NOWhour, NOWminute, NOWsecond;
int Setyear, Setmonth, Setday, Sethour, Setminute, Setsecond;
bool TIFlagd = 0, TIFlagf = 0;                                         // флаг индикации таймера выращивания 0 - таймер не активен, 1 - таймер активен
int TRyear, TRmonth, TRday, TRhour, TRminute, TRsecond;               // переменные в которых будет сохранена дата начала выращивания
int bank = 0;                                                         // номер банка записи и загрузки настроек выращивания
int FlagTempFan;
int FlagTimeFan = 0;    // 
//Define Variables we'll be connecting to
double Output;
//Define the aggressive and conservative Tuning Parameters
double consKp=1, consKi=0.05, consKd=0.25;
int Bit[2];                                                         //для записи float в eeprom
int Start = 0;

PID myPID(&Tnow, &Output, &Temp, consKp, consKi, consKd, DIRECT);  //Specify the links and initial tuning parameters

uint8_t strelka_vverh_vniz[8] = { B00100, B01110, B11111, B00000, B11111, B01110, B00100 };     // закодирована в двоичной системе СТРЕЛКА ВВЕРХ ВНИЗ 
uint8_t temp_cel[8]           = { B11000, B11000, B00110, B01001, B01000, B01001, B00110 };     // закодирована в двоичной системе ГРАДУС ЦЕЛЬСИЯ
uint8_t temp_del[8]           = { B00000, B00100, B00100, B01010, B01010, B10001, B11111 };     // закодирована в двоичной системе ДЕЛЬТА
uint8_t   Hot_ON[8]           = { B01111, B10000, B00111, B00001, B01110, B10000, B01111 };     // закодирована в двоичной системе ОБОГРЕВ
uint8_t   Fan_ON[8]           = { B11011, B11011, B11011, B00100, B00100, B11011, B11011 };     // закодирована в двоичной системе ОБДУВ
uint8_t   Hum_ON[8]           = { B00100, B00100, B01110, B11011, B10111, B10111, B01110 };     // закодирована в двоичной системе УВЛАЖНЕНИЕ                             
uint8_t   WiFi_funk_ON[8]     = { B00000, B00000, B01110, B10101, B01110, B00100, B00000 };     // закодирована в двоичной системе WiFi_funk                             



#define PinHot  0                             // первоначальное подключение обогрева на реле №1 
#define PinLite 2                             // первоначальное подключениe обдувa на реле №2
#define PinHum  16                            // первоначальное подключение увлажнителя на реле №3
#define PinHumGroundControl 10                // первоначальное подключение датчика влажности земли на пин 10
#define PinHumGround 14
float voltage ;            // напряжение на аккумуляторе
int netpower ;           // наличие сети
 
int m;                                   // значения отображения информации на табле
int buttons_Menu;             // значения кнопок меню 
int PressingButtons;

unsigned long int currentTime = 0 ; 
unsigned long int loopTime = 0 ; 

unsigned long int currentMillis;
unsigned long int StartMillis = 0 ;                                              //счетчик прошедшего времени для AutoStartMenu
#define interval 15000                                                 //задержка автовозврата к StartMenu 7сек
unsigned long int BME280readMillis = 0 ;                                           //счетчик прошедшего времени для интервала чтения с датчика температуры
#define BME280interval 100                                          //опрос датчика температуры каждую 0.1 сек
unsigned long int I2CGroundreadMillis = 0 ;                                           //счетчик прошедшего времени для интервала чтения с датчика температуры
#define I2CGroundinterval 250                                          //опрос датчика температуры каждую 0.25 сек
unsigned long int MenuRewriteMillis = 0 ;                                          //счетчик прошедшего времени для обновления главного меню
#define MenuRewriteinterval 500                                        //обновление главного меню через 0,5сек
unsigned long int WiFi_funkwriteMillis = 0;
unsigned long int TimeFaning = 0;                                                    //счетчик прошедшего времени для продувки теплицы от СО2
unsigned long int TimeIntervalFaning;                                          //счетчик прошедшего времени для интервала между продувками теплицы от СО2
unsigned long int TimeFaningInterval = TimeFanWork * 1000;                 //длительность работы вентилятора при продувке теплицы от СО2 
unsigned long int TimeIntervalFaningInterval = TimeIntervalFanWork * 60000;//длительность интервала между продувками теплицы от СО2


void setup()
{
  lcd.begin(20, 4);
  Serial.begin(115200);             // Запускаем вывод данных на серийный порт 
  Wire.begin(SDA_PIN, SCL_PIN);
  EEPROM.begin(512);
  ads.begin();
 
  rservo.attach(0);
  lservo.attach(2);
  myPID.SetMode(AUTOMATIC);
  Serial.println();
   
  Serial.print("Connecting to ");
  Serial.println(ssid);

  lcd.setCursor(0, 0);  lcd.print("   Connecting to    ");
  lcd.setCursor(0, 1);  lcd.print(ssid);
  lcd.setCursor(0, 2);  lcd.print("       Pass         ");
  lcd.setCursor(0, 3);  lcd.print(pass);
  Serial.println();
   
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
 Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  
   Serial.println("WiFi connected");  
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());
   
  loopTime = currentTime;
 
  pinMode(PinHot, OUTPUT);                       // Реле №1 на 2 выходе
  pinMode(PinLite, OUTPUT);                      // Реле №2 на 3 выходе
  pinMode(PinHum, OUTPUT);                       // Реле №3 на 4 выходе
  pinMode(PinHumGroundControl, OUTPUT);                 // Управление датчиком на 5 выходе
  pinMode(PinHumGround, OUTPUT);                 // Управление датчиком на 5 выходе
 
  LoadFromEEPROM(0);        
                                               // --------------- чтение из EEPROM установок настройки теплицы сохраненных в банк настроек №1
                                             // --------------- чтение из EEPROM установок и времени начала выращивания таймера выращивания
  TRyear = EEPROM.read(120) + 2000; delay(100);                                         // чтение года начала выращивания 
  TRmonth = EEPROM.read(121); delay(100);                                                // чтение месяца начала выращивания
  TRday = EEPROM.read(122); delay(100);                                                // чтение дня начала выращивания
  TRhour = EEPROM.read(123); delay(100);                                                // чтение часа начала выращивания
  TRminute = EEPROM.read(124); delay(100);                                                // чтение минуты начала выращивания
  TRsecond = EEPROM.read(125); delay(100);                                                // чтение секунды начала выращивания
  TIFlagd = EEPROM.read(126); delay(100);                                                // чтение флага ВКЛЮЧЕН (1) / ВЫКЛЮЧЕН (0)  индикации таймера выращивания
  TIFlagf = EEPROM.read(127); delay(100);                                                // чтение флага ВКЛЮЧЕН (1) / ВЫКЛЮЧЕН (0)  таймера выращивания
                                                 // --------------- чтение из EEPROM установок и времени начала выращивания таймера выращивания

                          // --------------- чтение из EEPROM установок настройки теплицы сохраненных в банк настроек №0

  lcd.createChar(1, strelka_vverh_vniz); lcd.createChar(2, WiFi_funk_ON);  lcd.createChar(3, temp_cel);  lcd.createChar(4, temp_del);  lcd.createChar(5, Hot_ON);  lcd.createChar(6, Fan_ON);  lcd.createChar(7, Hum_ON);

  lcd.setCursor(0, 0);  lcd.print("*-*-*-*-****-*-*-*-*");
  lcd.setCursor(0, 1);  lcd.print("    AutoHothouse    ");
  lcd.setCursor(0, 2);  lcd.print("  By_Vurchun_V-0.9  ");
  lcd.setCursor(0, 3);  lcd.print("*-*-*-*-****-*-*-*-*");
  delay(500);
  lcd.clear();
  currentTime = millis();
  loopTime = currentTime;

}
void RASEEPROMSTimer()
{
  TRyear = RTC.year - 2000; TRmonth = RTC.month; TRday = RTC.day; TRhour = RTC.hour; TRminute = RTC.minute; TRsecond = RTC.second;
  EEPROM.write(120, TRyear); delay(100);                                             // запись года начала выращивания в ячейку 120 EEPROM
  EEPROM.write(121, TRmonth); delay(100);                                             // запись месяца начала выращивания в ячейку 121 EEPROM
  EEPROM.write(122, TRday); delay(100);                                             // запись дня начала выращивания в ячейку 122 EEPROM
  EEPROM.write(123, TRhour); delay(100);                                             // запись часа начала выращивания в ячейку 123 EEPROM
  EEPROM.write(124, TRminute); delay(100);                                             // запись минуты начала выращивания в ячейку 124 EEPROM
  EEPROM.write(125, TRsecond); delay(100);                                             // запись секунды начала выращивания в ячейку 125 EEPROM                                                                 
}


void PreSetTime()
{
  Setyear = RTC.year; Setmonth = RTC.month; Setday = RTC.day; Sethour = RTC.hour; Setminute = RTC.minute; Setsecond = RTC.second;
}
void GoSetTime(int InputX){
  switch (SubMenu) {
  case 1:  Setyear   = InputX;    break;
  case 2:  Setmonth  = InputX;   break;
  case 3:  Setday    = InputX;     break;
  case 4:  Sethour   = InputX;    break;
  case 5:  Setminute = InputX;  break;
  case 6:  Setsecond = InputX;  break;
  }
}

void SetTime()
{
  RTC.getTime();
  RTC.stopClock();
  RTC.fillByYMD(Setyear, Setmonth,  Setday   );  delay(250);
  RTC.fillByHMS(Sethour, Setminute, Setsecond);  delay(250);
  RTC.setTime();
  delay(100);
  RTC.startClock();
}
void PrintMenuWrite(int FlagM)
{
  switch (FlagM) {
  case 0:  lcd.setCursor(15, 1);  lcd.print("\1");  break;
  case 1:  lcd.setCursor(15, 1);  lcd.print("*" );   break;
  }
}

int Sec, SecPer, Min, MinPer, Hou, HouPer, Dey, DeyPer, Mon, MonPer, Yer;
void TimerCalculatePrint()
{  
  if (TIFlagf == 1) {
    if (RTC.second >= TRsecond) { Sec = RTC.second - TRsecond;          SecPer = 0; }
    else { Sec = 60 + (RTC.second - TRsecond);          SecPer = 1; }
    if ((RTC.minute - SecPer) >= TRminute) { Min = RTC.minute - TRminute - SecPer; MinPer = 0; }
    else { Min = 60 + (RTC.minute - TRminute) - SecPer; MinPer = 1; }
    if ((RTC.hour - MinPer) >= TRhour) { Hou = RTC.hour - TRhour - MinPer; HouPer = 0; }
    else { Hou = 24 + (RTC.hour - TRhour) - MinPer; HouPer = 1; }
    if ((RTC.day - HouPer) >= TRday) { Dey = RTC.day - TRday - HouPer; DeyPer = 0; }
    else {
      switch (RTC.month) {
      case 2: { Dey = 31 + (RTC.day - TRday) - HouPer; DeyPer = 1; break; }
      case 3: {
        if (RTC.year % 4 == 0 && RTC.year % 100 != 0 || RTC.year % 400 == 0)
        {
          Dey = 29 + (RTC.day - TRday) - HouPer; DeyPer = 1;
        }
        else
        {
          Dey = 28 + (RTC.day - TRday) - HouPer; DeyPer = 1;
        }
        break; }
      case 4:  { Dey = 31 + (RTC.day - TRday) - HouPer; DeyPer = 1; break; }
      case 5:  { Dey = 30 + (RTC.day - TRday) - HouPer; DeyPer = 1; break; }
      case 6:  { Dey = 31 + (RTC.day - TRday) - HouPer; DeyPer = 1; break; }
      case 7:  { Dey = 30 + (RTC.day - TRday) - HouPer; DeyPer = 1; break; }
      case 8:  { Dey = 31 + (RTC.day - TRday) - HouPer; DeyPer = 1; break; }
      case 9:  { Dey = 31 + (RTC.day - TRday) - HouPer; DeyPer = 1; break; }
      case 10: { Dey = 30 + (RTC.day - TRday) - HouPer; DeyPer = 1; break; }
      case 11: { Dey = 31 + (RTC.day - TRday) - HouPer; DeyPer = 1; break; }
      case 12: { Dey = 30 + (RTC.day - TRday) - HouPer; DeyPer = 1; break; }
      case 1:  { Dey = 31 + (RTC.day - TRday) - HouPer; DeyPer = 1; break; }
      }
    }
    if ((RTC.month - DeyPer) >= TRmonth) { Mon = RTC.month - TRmonth - DeyPer; MonPer = 0; }
    else { Mon = 12 + (RTC.day - TRmonth) - DeyPer; MonPer = 1; }
    Yer = RTC.year - TRyear;
  }
  }







void loop()
{
  PressingButtons = 0;
  currentMillis = millis();
  RTC.getTime(); 

  if(Start < 3){
Temp = 20;
 deltaT = 0.1;
 Humiditi =80; // чтение дельты влажности выращивания из ячейки "4"
 deltaHumiditi = 1; maxTempFanStart = 30; TimeFanWork = 60; // чтение higtByte интервала между вентиляциями теплицы от СО2 из ячейки "12"
 TimeIntervalFanWork = 120;
 FanWorkFlag = 0; // чтение флага активностивентиляциями теплицы от СО2 из ячейки "13"
 TimeFaningInterval = TimeFanWork * 1000; //чтение длительности работы вентилятора при продувке теплицы от СО2 
 TimeIntervalFaningInterval = TimeIntervalFanWork * 60000; //чтение длительности интервала между продувками теплицы от СО2
 consKp = 1; //чтение Пропорционального коефициента 
 consKi = 1; //чтение Интегрального коефициента 
 consKd = 1; //чтение Дефиринциального коефициента 
 HumGround = 80; //чтение Нужной вдоажности почвы
 TypeHouse = 1; //чтение Типа теплицы 
 SaveToEEPROM(0);SaveToEEPROM(1);SaveToEEPROM(2);SaveToEEPROM(3);SaveToEEPROM(4);
 Start++;
  }

  Menu();
  switch (m) {
  case 0: {  lcd.clear();lcd.setCursor(0, 0);  lcd.print("T="); lcd.print(Tnow); lcd.print("\3 (");lcd.print(Temp);    lcd.print("\3)");  
             lcd.setCursor(0, 1);  lcd.print("H="); lcd.print(hum);  lcd.print("%("); lcd.print(Humiditi); lcd.print("%) ");if(TypeHouse == 0){lcd.print("Gr="); lcd.print(HumGroundnow);  lcd.print("%("); lcd.print(HumGround); lcd.print("%)");}
             lcd.setCursor(0, 2);if( voltage >= 13.5 ){lcd.print("220V - ON"); }else{ lcd.print("Bat="); lcd.print(voltage);}
             lcd.setCursor(0, 3);
             if (RTC.hour < 10) lcd.print(0); lcd.print(RTC.hour); lcd.print(":"); if (RTC.minute < 10) lcd.print(0); lcd.print(RTC.minute); lcd.print(":"); if (RTC.second < 10) lcd.print(0); lcd.print(RTC.second);       
             BME280Read(); StartFan();StartHot();StartLite();StartHum(); TimerCalculatePrint();WiFi_funk();                                                                                                                                                                   FlagMenu = 0;   break; }
  case 10: {  lcd.clear(); lcd.setCursor(0, 1); lcd.print(F("    Setting     ")); lcd.setCursor(0, 2); lcd.print(F("   Growing  ")); lcd.setCursor(15, 1);           lcd.print("\1                ");                                                               delay(100);FlagMenu = 0;   break; }
  case 11: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("   Temperature  ")); lcd.setCursor(0, 1); lcd.print(F("t =     ")); lcd.print(Temp);                lcd.print("\3                ");             PrintMenuWrite(FlagMenu);                         delay(100);                break; }
  case 12: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("    Delta T     ")); lcd.setCursor(0, 1); lcd.print(F("\4t =   ")); lcd.print(deltaT);              lcd.print("\3                ");             PrintMenuWrite(FlagMenu);                         delay(100);                break; }
  case 13: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("   Humiditi     ")); lcd.setCursor(0, 1); lcd.print(F("H =     ")); lcd.print(Humiditi);            lcd.print("%                 ");             PrintMenuWrite(FlagMenu);                         delay(100);                break; }
  case 14: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("    Delta H     ")); lcd.setCursor(0, 1); lcd.print(F("\4h =   ")); lcd.print(deltaHumiditi);       lcd.print("%                 ");             PrintMenuWrite(FlagMenu);                         delay(100);                break; }
  case 15: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("  Type HotHouse ")); lcd.setCursor(0, 1); lcd.print(F("Type =  "));switch(TypeHouse){case 0:{lcd.setCursor(10, 1);lcd.print("earthe");break;}case 1:{lcd.setCursor(10, 1);lcd.print("hidro");break;}case 2:{lcd.setCursor(10, 1);lcd.print("aer");break;}}   PrintMenuWrite(FlagMenu);                         delay(100);                break; } 
  case 20: {  lcd.clear(); lcd.setCursor(0, 1); lcd.print(F("  Save setting  ")); lcd.setCursor(0, 2); lcd.print(F("  to  EEPROM")); lcd.setCursor(15, 1);           lcd.print("\1                ");                                                               delay(100); FlagMenu = 0;  break; }
  case 21: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("Save setting to ")); lcd.setCursor(0, 1); lcd.print(F("bank    ")); lcd.print(bank); if (FlagMenu == 0) {      lcd.print(" press set"); delay(100); } else { SaveToEEPROM(bank);   lcd.print(" saving...");   delay(100); }              break; }
  case 30: {  lcd.clear(); lcd.setCursor(0, 1); lcd.print(F("  Load setting  ")); lcd.setCursor(0, 2); lcd.print(F("from  EEPROM")); lcd.setCursor(15, 1);           lcd.print("\1                ");                                                               delay(100); FlagMenu = 0;  break; }
  case 31: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("  Load setting  ")); lcd.setCursor(0, 1); lcd.print(F("bank ")); lcd.print(bank); if (FlagMenu == 0) {      lcd.print(" press set"); delay(100); } else { LoadFromEEPROM(bank); lcd.print(" loading..");   delay(100); }              break; }
  case 40: {  lcd.clear(); lcd.setCursor(0, 1); lcd.print(F("      Time      ")); lcd.setCursor(0, 2); lcd.print(F("  setting   ")); lcd.setCursor(15, 1);           lcd.print("\1                ");                                                               delay(100); FlagMenu = 0;  break; }
  case 41: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("  setup  year   ")); lcd.setCursor(5, 1); lcd.print(Setyear);                                                                                           PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 42: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("  setup  month  ")); lcd.setCursor(6, 1); lcd.print(Setmonth);                                                                                          PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 43: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("   setup  day   ")); lcd.setCursor(6, 1); lcd.print(Setday);                                                                                            PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 44: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("  setup  hour   ")); lcd.setCursor(6, 1); lcd.print(Sethour);                                                                                           PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 45: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("  setup minute  ")); lcd.setCursor(6, 1); lcd.print(Setminute);                                                                                         PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 46: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("  setup second  ")); lcd.setCursor(6, 1); lcd.print(Setsecond);                                                                                         PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 50: {  lcd.clear(); lcd.setCursor(0, 1); lcd.print(F("    Exstra      ")); lcd.setCursor(0, 2); lcd.print("     Setting  ");  lcd.setCursor(15, 1);           lcd.print("\1                ");                                                               delay(100); FlagMenu = 0;  break; }
  case 51: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("      K p       ")); lcd.setCursor(0, 1); lcd.print(F("P = "));             lcd.print(consKp);                                                          PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 52: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("      K i       ")); lcd.setCursor(0, 1); lcd.print(F("I = "));             lcd.print(consKi);                                                          PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 53: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("      K d       ")); lcd.setCursor(0, 1); lcd.print(F("D = "));             lcd.print(consKd);                                                          PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 54: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("   Cooling T    ")); lcd.setCursor(0, 1); lcd.print(F("t = "));             lcd.print(maxTempFanStart);     lcd.print("\3                ");            PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 55: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("CO2 faning time ")); lcd.setCursor(0, 1); lcd.print(F("time = "));          lcd.print(TimeFanWork);         lcd.print(" sec.             ");            PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 56: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("CO2 fan interval")); lcd.setCursor(0, 1); lcd.print(F("time = "));          lcd.print(TimeIntervalFanWork); lcd.print(" min.             ");            PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  } 
 }
