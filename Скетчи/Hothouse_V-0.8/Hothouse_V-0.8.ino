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
int Bit[4];                                                         //для записи float в eeprom

PID myPID(&Tnow, &Output, &Temp, consKp, consKi, consKd, DIRECT);  //Specify the links and initial tuning parameters

uint8_t strelka_vverh_vniz[8] = { B00100, B01110, B11111, B00000, B11111, B01110, B00100 };     // закодирована в двоичной системе СТРЕЛКА ВВЕРХ ВНИЗ 
uint8_t temp_cel[8]           = { B11000, B11000, B00110, B01001, B01000, B01001, B00110 };     // закодирована в двоичной системе ГРАДУС ЦЕЛЬСИЯ
uint8_t temp_del[8]           = { B00000, B00100, B00100, B01010, B01010, B10001, B11111 };     // закодирована в двоичной системе ДЕЛЬТА
uint8_t   Hot_ON[8]           = { B01111, B10000, B00111, B00001, B01110, B10000, B01111 };     // закодирована в двоичной системе ОБОГРЕВ
uint8_t   Fan_ON[8]           = { B11011, B11011, B11011, B00100, B00100, B11011, B11011 };     // закодирована в двоичной системе ОБДУВ
uint8_t   Hum_ON[8]           = { B00100, B00100, B01110, B11011, B10111, B10111, B01110 };     // закодирована в двоичной системе УВЛАЖНЕНИЕ                             
uint8_t   WiFi_funk_ON[8]     = { B00000, B11111, B01110, B10101, B01110, B00100, B00000 };     // закодирована в двоичной системе WiFi_funk                             



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


void WriteBit(float Write)
{
  
  Bit[0] = abs(Write);
  Bit[1] = abs((Write - Bit[0]) * 100);
   }
void WriteZero(float Write)
{ int zero = 0;
  while(Write<10)
{Write/=10;zero++;}
Bit[0] = Write;
Bit[1] = zero;
}

float ReadBit(float Read)
{
  Read = Bit[0] + Bit[1] /100;
  return Read;
  }
float ReadZero(float Read)
{Read = Read*Bit[1];
return Read;}
void SaveToEEPROM(int BankSave)                                                         // запись данных во внутренний EEPROM 
{
  WriteBit(Temp);
  BS = BankSave * 20 + 0;     EEPROM.write(BS, Bit[0]);           delay(100);    // запись  температуры выращивания * 100 в ячейку "0" банка "bank"
  BS = BankSave * 20 + 1;     EEPROM.write(BS, Bit[1]);           delay(100);    // запись  температуры выращивания * 100 в ячейку "0" банка "bank"

  WriteBit(deltaT);
  BS = BankSave * 20 + 2;     EEPROM.write(BS, Bit[0]);           delay(100);    // запись  температуры выращивания * 100 в ячейку "0" банка "bank"
  BS = BankSave * 20 + 3;     EEPROM.write(BS, Bit[1]);           delay(100);    // запись  температуры выращивания * 100 в ячейку "0" банка "bank"

  BS = BankSave * 20 + 4;     EEPROM.write(BS, Humiditi);          delay(100);    // запись дельты влажности выращивания в ячейку "4" банка "bank"
  BS = BankSave * 20 + 5;     EEPROM.write(BS, deltaHumiditi);     delay(100);    // запись дельты влажности выращивания в ячейку "5" банка "bank"
  
  WriteBit(maxTempFanStart);                                                                              
  BS = BankSave * 20 + 6;     EEPROM.write(BS, Bit[0]);           delay(100);    // запись  температуры продувки * 100 в ячейку "7" банка "bank"
  BS = BankSave * 20 + 7;     EEPROM.write(BS, Bit[1]);           delay(100);
  WriteBit(TimeFanWork);
  BS = BankSave * 20 + 8;     EEPROM.write(BS, Bit[1]);           delay(100);    // запись higtBit температуры продувки * 100 в ячейку "8" банка "bank"
  BS = BankSave * 20 + 9;     EEPROM.write(BS, Bit[1]);           delay(100);   
  
  WriteBit(TimeIntervalFanWork);
  BS = BankSave * 20 + 10;     EEPROM.write(BS, Bit[0]);           delay(100);    // запись  времени вентиляции теплицы от СО2 в ячейку "9" банка "bank"
  BS = BankSave * 20 + 11;     EEPROM.write(BS, Bit[1]);           delay(100);    // запись higtBit температуры продувки * 100 в ячейку "8" банка "bank"
  
  BS = BankSave * 20 + 13;    EEPROM.write(BS, FanWorkFlag);        delay(100);    // запись флага активностивентиляциями теплицы от СО2 в ячейку "13" банка "bank"
  BS = BankSave * 20 + 14;    EEPROM.write(BS, HumGround);          delay(100);    // запись Нужной вдоажности почвы в ячейку "14" банка "bank"
                              EEPROM.write(131, consKp);            delay(100);    // запись флага активностивентиляциями теплицы от СО2 в ячейку "13" банка "bank"
                              EEPROM.write(132, consKi);            delay(100);    // запись Нужной вдоажности почвы в ячейку "14" банка "bank"
                              EEPROM.write(133, consKd);            delay(100);    // запись флага активностивентиляциями теплицы от СО2 в ячейку "13" банка "bank"
                              EEPROM.write(135, TypeHouse);         delay(100);    // запись Нужной вдоажности почвы в ячейку "14" банка "bank"
 
 }
void LoadFromEEPROM(int BankLoad)                                                       // загрузка данных из внутреннего EEPROM
{
  BL = BankLoad * 20 + 0;     Bit[0] = EEPROM.read(BL);            delay(100);    // чтение  температуры выращивания *100 из ячейки "0"
  BL = BankLoad * 20 + 1;     Bit[1] = EEPROM.read(BL);            delay(100);    // чтение higtBit температуры выращивания *100 из ячейки "1"
 Temp = ReadBit(Bit[0]);
  BL = BankLoad * 20 + 2;     Bit[0] = EEPROM.read(BL);            delay(100);    // чтение lowBit дельтаТ температуры выращивания *100 из ячейки "2"
  BL = BankLoad * 20 + 3;     Bit[1] = EEPROM.read(BL);            delay(100);    // чтение higtBit дельтаТ температуры выращивания *100 из ячейки "3"
 deltaT = ReadBit(Bit[0]);
  BL = BankLoad * 20 + 4;     Humiditi = EEPROM.read(BL);           delay(100);    // чтение дельты влажности выращивания из ячейки "4"
  BL = BankLoad * 20 + 5;     deltaHumiditi = EEPROM.read(BL);      delay(100);    // чтение дельты влажности выращивания из ячейки "5"
                                                                                          
  BL = BankLoad * 20 + 7;     Bit[0] = EEPROM.read(BL);            delay(100);    // чтение lowBit температуры продувки *100 из ячейки "7"
  BL = BankLoad * 20 + 8;     Bit[1] = EEPROM.read(BL);            delay(100);    // чтение higtBit температуры продувки *100 из ячейки "8"
 maxTempFanStart = ReadBit(Bit[0]);
  BL = BankLoad * 20 + 9;     Bit[0] = EEPROM.read(BL);            delay(100);     // чтение lowBit времени вентиляции теплицы от СО2 из ячейки "9"
  BL = BankLoad * 20 + 10;    Bit[1] = EEPROM.read(BL);            delay(100);     // чтение higtBit времени вентиляции теплицы от СО2 из ячейки "10"
 TimeFanWork =  ReadBit(Bit[0]);
  BL = BankLoad * 20  + 11;   Bit[0] = EEPROM.read(BL);            delay(100);      // чтение lowBit интервала между вентиляциями теплицы от СО2 из ячейки "11"
  BL = BankLoad * 20 + 12;    Bit[1] = EEPROM.read(BL);            delay(100);      // чтение higtBit интервала между вентиляциями теплицы от СО2 из ячейки "12"
 TimeIntervalFanWork = ReadBit(Bit[0]);
  BL = BankLoad * 20 + 13;    FanWorkFlag = EEPROM.read(BL);        delay(100);     // чтение флага активностивентиляциями теплицы от СО2 из ячейки "13"
  BL = BankLoad * 20 + 14;    HumGround = EEPROM.read(BL);          delay(100);     // чтение Нужной вдоажности почвы в ячейку "14" банка "bank"
 
                              consKp = EEPROM.read(131);            delay(100);    //чтение Пропорционального коефициента             
                              consKi = EEPROM.read(132);            delay(100);    //чтение Интегрального коефициента                 
                              consKd = EEPROM.read(133);            delay(100);    //чтение Дефиринциального коефициента 
                              TypeHouse = EEPROM.read(135);         delay(100);    //чтение Типа теплицы 
 
  TimeFaningInterval = TimeFanWork * 1000;                 //длительность работы вентилятора при продувке теплицы от СО2 
  TimeIntervalFaningInterval = TimeIntervalFanWork * 60000;//длительность интервала между продувками теплицы от СО2
}
void setup()
{
  lcd.begin(20, 4);
  Wire.begin();
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

  lcd.createChar(1, strelka_vverh_vniz);  lcd.createChar(3, temp_cel);  lcd.createChar(4, temp_del);  lcd.createChar(5, Hot_ON);  lcd.createChar(6, Fan_ON);  lcd.createChar(7, Hum_ON); lcd.createChar(8, WiFi_funk_ON);

  lcd.setCursor(0, 0);  lcd.print("*-*-*-*-****-*-*-*-*");
  lcd.setCursor(0, 1);  lcd.print("    AutoHothouse    ");
  lcd.setCursor(0, 2);  lcd.print("  By_Vurchun_V-0.8  ");
  lcd.setCursor(0, 3);  lcd.print("*-*-*-*-****-*-*-*-*");
  delay(1500);
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
      if ((currentMillis - TimeFaning) > TimeFaningInterval) {
        FlagTimeFan = 0;
        TimeIntervalFaning = currentMillis;
      } break; }
    case 0: {
      if ((currentMillis - TimeIntervalFaning) > TimeIntervalFaningInterval) {
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

void callback(const MQTT::Publish& pub) {
  Serial.print(pub.topic());
  Serial.print(" => ");
    uint8_t buf[100];
    int read;
  if (pub.has_stream()) {

    while (read = pub.payload_stream()->read(buf, 100)) {
      Serial.write(buf, read);
    }
    pub.payload_stream()->stop();
    Serial.println("");
  } else
  if(String(pub.topic()) == "Hothouse/ControlHum")                   { Humiditi = pub.payload_string().toInt(); }// проверяем из нужного ли нам топика пришли данные // преобразуем полученные данные в тип integer
  if(String(pub.topic()) == "Hothouse/ControlTemp")                  { Temp = pub.payload_string().toInt(); }// проверяем из нужного ли нам топика пришли данные // преобразуем полученные данные в тип integer
  if(String(pub.topic()) == "Hothouse/TimeIntervalFanWorkntrolTemp") { TimeIntervalFanWork = pub.payload_string().toInt(); }// проверяем из нужного ли нам топика пришли данные // преобразуем полученные данные в тип integer
  if(String(pub.topic()) == "Hothouse/TimeFanWork")                  { TimeFanWork = pub.payload_string().toInt(); }// проверяем из нужного ли нам топика пришли данные // преобразуем полученные данные в тип integer 
  if(String(pub.topic()) == "Hothouse/maxTempFanStart")              { maxTempFanStart = pub.payload_string().toInt(); }// проверяем из нужного ли нам топика пришли данные // преобразуем полученные данные в тип integer
  if(String(pub.topic()) == "Hothouse/deltaT")                       { deltaT = pub.payload_string().toInt(); }// проверяем из нужного ли нам топика пришли данные // преобразуем полученные данные в тип integer
  if(String(pub.topic()) == "Hothouse/deltaHumiditi")                { deltaHumiditi = pub.payload_string().toInt(); }// проверяем из нужного ли нам топика пришли данные // преобразуем полученные данные в тип integer
  if(String(pub.topic()) == "Hothouse/NumBankSave")                  { NumBankSave = pub.payload_string().toInt(); }// проверяем из нужного ли нам топика пришли данные // преобразуем полученные данные в тип integer
  if(String(pub.topic()) == "Hothouse/IfBankSave")                   { IfBankSave = pub.payload_string().toInt(); }// проверяем из нужного ли нам топика пришли данные // преобразуем полученные данные в тип integer
  if(String(pub.topic()) == "Hothouse/BankSave")                     { BankSave = pub.payload_string().toInt(); }// проверяем из нужного ли нам топика пришли данные // преобразуем полученные данные в тип integer
  if(String(pub.topic()) == "Hothouse/ConstP")                       { BankSave = pub.payload_string().toInt(); }// проверяем из нужного ли нам топика пришли данные // преобразуем полученные данные в тип integer
  if(String(pub.topic()) == "Hothouse/ConstI")                       { BankSave = pub.payload_string().toInt(); }// проверяем из нужного ли нам топика пришли данные // преобразуем полученные данные в тип integer
  if(String(pub.topic()) == "Hothouse/ConstD")                       { BankSave = pub.payload_string().toInt(); }// проверяем из нужного ли нам топика пришли данные // преобразуем полученные данные в тип integer

}

void jsonGet() {
  
  if (!wclient.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
    Serial.println(wapi);
    Serial.println("Host: api.openweathermap.org");
    Serial.println("Connection: close");
    Serial.println();
 
  delay(1500);
  // Read all the lines of the reply from server and print them to Serial
  while(wclient.available()){
    line = wclient.readStringUntil('\r'); 
  }
  Serial.print(line);
  Serial.println();
  Serial.println("closing connection");
}
 
void WiFi_funk() 
{
  voltage = ads.readADC_SingleEnded(1) * 0.185 / 1000 * 5.75;     //подсчет заряда аккумулятора
  if( voltage > 13.5 ) netpower = 1;
  else netpower = 0;
  delay(10);
  int i = 0,ReadySaved=0;
  client.publish("Hothouse/ReadySaved",String(ReadySaved));     
  
    while (i<10){
    lcd.setCursor(18, 3); lcd.print("\8");           //Отправка значений на сервер    
    delay(500);i++;
    }
if (WiFi.status() == WL_CONNECTED) {
//client object makes connection to server
if (!client.connected()) {
Serial.println("Connecting to MQTT server");
//Authenticating the client object
if (client.connect(MQTT::Connect("mqtt_client_name").set_auth(mqtt_user, mqtt_pass))) {
Serial.println("Connected to MQTT server");
} 
else 
{
Serial.println("Could not connect to MQTT server");   
}
}
}

   
    lcd.setCursor(18, 3); lcd.print("\8");           //Отправка значений на сервер    
      Serial.println("Start");delay(50);
  client.publish("Hothouse/Temp",String(Tnow));           Serial.println("Tnow = ");        Serial.println(Tnow);delay(50);
  client.publish("Hothouse/ControlTemp",String(Temp));    Serial.println("ControlTemp = "); Serial.println(Temp);delay(50);
  client.publish("Hothouse/deltaT",String(deltaT));       Serial.println("deltaT = "); Serial.println(deltaT);  delay(50);
  client.publish("Hothouse/Hum",String(hum));             Serial.println("HumiditiNow = "); Serial.println(hum);delay(50);
  client.publish("Hothouse/ControlHum",String(Humiditi)); Serial.println("ControlHum = ");  Serial.println(Humiditi);delay(50);
  client.publish("Hothouse/deltaHumiditi",String(deltaHumiditi));    Serial.println("deltaHumiditi = "); Serial.println(deltaHumiditi);delay(50);
  client.publish("Hothouse/Power",String(voltage));       Serial.println("Voltage = ");     Serial.println(voltage);delay(50);
  client.publish("Hothouse/NetPower",String(netpower));   Serial.println("NetPower = ");    Serial.println(netpower);delay(50);
  client.publish("Hothouse/ConstP",String(consKp));       Serial.println("ConstP = "); Serial.println(consKp);delay(50);
  client.publish("Hothouse/ConstI",String(consKi));       Serial.println("ConstI = "); Serial.println(consKi);delay(50);
  client.publish("Hothouse/ConstD",String(consKd));      Serial.println("ConstD = "); Serial.println(consKd);delay(50);
  client.publish("Hothouse/maxTempFanStart",String(maxTempFanStart)); Serial.println("maxTempFanStart = "); Serial.println(maxTempFanStart);delay(50);
  client.publish("Hothouse/TimeFanWork",String(TimeFanWork));  Serial.println("TimeFanWork = "); Serial.println(TimeFanWork);delay(50);
  client.publish("Hothouse/TimeIntervalFanWork",String(TimeIntervalFanWork));  Serial.println("TimeIntervalFanWork = "); Serial.println(TimeIntervalFanWork);delay(50);
 
  client.subscribe("Hothouse/NumBankSave");         client.set_callback(callback);// подписывааемся по топик с данными для банка загрузки 
  client.subscribe("Hothouse/IfBankSave"); client.set_callback(callback);// подписывааемся по топик с данными для подтверждения перезагрузки  
 
     Serial.println("Finish");
     Serial.println("");
 
    if(IfBankSave == 1){
    IfBankSave = 0;
    while(BankSave<1){
    if(BankSave == 1){SaveToEEPROM(NumBankSave);break;}
    Serial.println("");
  client.subscribe("Hothouse/deltaHumiditi");client.set_callback(callback); // подписывааемся по топик с данными для светодиода 
  client.subscribe("Hothouse/deltaT"); client.set_callback(callback);// подписывааемся по топик с данными для дельты температуры   
  client.subscribe("Hothouse/maxTempFanStart"); client.set_callback(callback);// подписывааемся по топик с данными для критичной температуры   
  client.subscribe("Hothouse/TimeFanWork"); client.set_callback(callback);// подписывааемся по топик с данными для работы вентилятора   
  client.subscribe("Hothouse/TimeIntervalFanWork "); client.set_callback(callback);// подписывааемся по топик с данными для периода вентилятора  
  client.subscribe("Hothouse/ControlHum"); client.set_callback(callback);// подписывааемся по топик с данными для заданной температуры
  client.subscribe("Hothouse/ControlTemp"); client.set_callback(callback);// подписывааемся по топик с данными для заданной влажности 
  client.subscribe("Hothouse/BankSave"); client.set_callback(callback);// подписывааемся по топик с данными для банка загрузки 
  client.subscribe("Hothouse/ConstP"); client.set_callback(callback);// подписывааемся по топик с данными для подтверждения перезагрузки  
  client.subscribe("Hothouse/ConstI"); client.set_callback(callback);// подписывааемся по топик с данными для подтверждения перезагрузки  
  client.subscribe("Hothouse/ConstD"); client.set_callback(callback);// подписывааемся по топик с данными для подтверждения перезагрузки  
    Serial.println("");


    }
    }
jsonGet();
     StaticJsonBuffer<2000> jsonBuffer;                   /// буфер на 2000 символов
   JsonObject& root = jsonBuffer.parseObject(line);     // Обрабатываем String
   if (!root.success()) {
    Serial.println("parseObject() failed");             // если ошибка, сообщаем об этом
     jsonGet();                                         // опрашиваем сервер еще раз
    return;                                             // и запускаем заново 
  }                    
  
  tempK = root["main"]["temp"];                   // достаем температуру из структуры main
  tempC = tempK - 273.15;                         // переводим кельвины в цельси
  tempKmin = root["main"]["temp_min"];            // и так далее
  tempCmin = tempKmin - 273.15;
  tempKmax = root["main"]["temp_max"];
  tempCmax = tempKmax - 273.15;
  pressurehPa = root["main"]["pressure"];          // достаем давление из структуры main
  pressure = pressurehPa/1.333;
  humidity = root["main"]["humidity"];             // достаем влажность из структуры main
  windspeed = root["wind"]["speed"];             // достаем скорость ветра из структуры main
  winddeg = root["wind"]["deg"];                   // достаем направление ветра из структуры main
    
}   
          


void loop()
{
  PressingButtons = 0;
  currentMillis = millis();
  RTC.getTime(); 

 

  if (FlagMenu == 0)
  {
    NOWyear = RTC.year; NOWmonth = RTC.month; NOWday = RTC.day; NOWhour = RTC.hour; NOWminute = RTC.minute; NOWsecond = RTC.second;
  }  
  if (currentTime >= loopTime)
  {currentTime = loopTime - interval;
    PressKeyMenu();
    switch (PressingButtons) {
case 1: {                                                                                                        // обработка события нажатия кнопки "МЕНЮ"
      if ((MainMenu == 0 && SubMenu == 0) || (MainMenu == 0 && SubMenu == 1)) { MainMenu = 1;  SubMenu = 0;  StartMillis = currentMillis;  delay(200); }
      else {
        if (MainMenu == 4 && SubMenu == 0) { PreSetTime(); SubMenu = 1; StartMillis = currentMillis;  delay(200); }
        else {
          if (MainMenu != 0 && SubMenu == 0) { SubMenu = 1;  StartMillis = currentMillis;  delay(200); }
          else {
            if (MainMenu == 4 && SubMenu != 0) { SetTime(); SubMenu = 0; FlagMenu = 0; StartMillis = currentMillis; delay(200); }
            else { if (MainMenu != 0 && SubMenu != 0) { SubMenu = 0;  FlagMenu = 0;  StartMillis = currentMillis;  delay(200); } }
          }
        }
      }break; }
    case 2: {                                                                                                        // обработка события нажатия кнопки "ВВЕРХ"
      if (MainMenu != 0 && SubMenu == 0) { MainMenu--;  StartMillis = currentMillis;  delay(200);  if (MainMenu < 1)  MainMenu = 6; }
      else {
        if (MainMenu == 1 && SubMenu != 0 && FlagMenu == 0) { SubMenu--;  StartMillis = currentMillis;  delay(200);  if (SubMenu < 1)  SubMenu = 9; }
        else {
          if (MainMenu == 4 && SubMenu != 0 && FlagMenu == 0) { SubMenu--;  StartMillis = currentMillis;  delay(200);  if (SubMenu < 1)  SubMenu = 6; }
          else {
            if (MainMenu == 5 && SubMenu != 0 && FlagMenu == 0) { SubMenu--;  StartMillis = currentMillis;  delay(200);  if (SubMenu < 1)  SubMenu = 5; }
            else {
              if (MainMenu == 6 && SubMenu != 0 && FlagMenu == 0) { SubMenu--;  StartMillis = currentMillis;  delay(200);  if (SubMenu < 1)  SubMenu = 3; }
              else {
                if (MainMenu == 1 && FlagMenu == 1) {
                  switch (SubMenu) {
                  case 1: {  Temp += FadeAmountTemp;            StartMillis = currentMillis; if (Temp >= maxTempDanger)Temp = maxTempDanger;      delay(100);  break; }
                  case 2: {  deltaT += FadeAmountTemp;          StartMillis = currentMillis; if (deltaT >= maxdeltaT)deltaT = maxdeltaT;        delay(100);  break; }
                  case 3: {  Humiditi += FadeAmountHum;         StartMillis = currentMillis; if (Humiditi >= MaximumHumiditi)Humiditi = MaximumHumiditi;  delay(100);  break; }
                  case 4: {  k++; if (k > 3) k = 3; TypeHouse = VarHouse[k]; StartMillis = currentMillis; delay(200); EEPROM.write(135, TypeHouse); delay(100); break; }        
                  case 5: {  deltaHumiditi += + FadeAmountdeltaHum; StartMillis = currentMillis; if (deltaHumiditi >= maxdeltaHum)deltaHumiditi = maxdeltaHum;           delay(100);  break; }
                  case 6: {  StartMillis = currentMillis; if (FanWorkFlag == 0) FanWorkFlag = 1; else if (FanWorkFlag == 1) FanWorkFlag = 0; delay(100);break; }
                 }
                }
                else {
                  if (MainMenu == 2 && FlagMenu == 0 && SubMenu == 1) { bank++; StartMillis = currentMillis;  delay(200); if (bank > 4) bank = 4; }
                  else {
                    if (MainMenu == 3 && FlagMenu == 0 && SubMenu == 1) { bank++; StartMillis = currentMillis;  delay(200); if (bank > 4) bank = 4; }
                    else {
                      if (MainMenu == 4 && FlagMenu == 1) {
                        switch (SubMenu) {
                        case 1: {  NOWyear++;    if (NOWyear > 2030)  NOWyear = 2030;  delay(200);  GoSetTime(NOWyear);  break; }
                        case 2: {  NOWmonth++;   if (NOWmonth > 12)   NOWmonth = 12;   delay(200);  GoSetTime(NOWmonth);  break; }
                        case 3: {  NOWday++;     if (NOWday > 31)     NOWday = 31;     delay(200);  GoSetTime(NOWday);  break; }
                        case 4: {  NOWhour++;    if (NOWhour > 23)    NOWhour = 23;    delay(200);  GoSetTime(NOWhour);  break; }
                        case 5: {  NOWminute++;  if (NOWminute > 59)  NOWminute = 59;  delay(200);  GoSetTime(NOWminute);  break; }
                        case 6: {  NOWsecond++;  if (NOWsecond > 59)  NOWsecond = 59;  delay(200);  GoSetTime(NOWsecond);  break; }
                        }
                      }
                      else {
                        if (MainMenu == 5 && FlagMenu == 1) {
                          switch (SubMenu) {
                          case 1: {  consKp += 0.01;                    StartMillis = currentMillis; if (consKp >= 100)consKp = 100;      delay(100); EEPROM.write(131,consKp);  break; }
                          case 2: {  consKi += 0.01;                    StartMillis = currentMillis; if (consKp >= 100)consKp = 100;      delay(100); EEPROM.write(132,consKi);  break; }
                          case 3: {  consKd += 0.01;                    StartMillis = currentMillis; if (consKp >= 100)consKp = 100;      delay(100); EEPROM.write(133,consKd);  break; }
                          case 4: { maxTempFanStart += FadeAmountTemp;                    StartMillis = currentMillis; if (maxTempFanStart >= maxTempFanStartMenuMax)maxTempFanStart = maxTempFanStartMenuMax;        delay(100);  break; }
                          case 5: { TimeFanWork += FadeTimeFanWork; StartMillis = currentMillis; if (TimeFanWork >= maxTimeFanWork) TimeFanWork = maxTimeFanWork; TimeFaningInterval = TimeFanWork * 1000;  delay(100);  break; }
                          case 6: { TimeIntervalFanWork += FadeTimeIntervalFanWork; StartMillis = currentMillis; if (TimeIntervalFanWork >= maxTimeIntervalFanWork)TimeIntervalFanWork = maxTimeIntervalFanWork; TimeIntervalFaningInterval = TimeIntervalFanWork * 60000; delay(100);  break; }
                   }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      } break; 
     }
    case 3: {                                                                                                        // обработка события нажатия кнопки "ВНИЗ"
      if (MainMenu != 0 && SubMenu == 0) { MainMenu++;  StartMillis = currentMillis;  delay(200);  if (MainMenu > 6)  MainMenu = 1; }
      else {
        if (MainMenu == 1 && SubMenu != 0 && FlagMenu == 0) { SubMenu++;  StartMillis = currentMillis;  delay(200);  if (SubMenu > 9)  SubMenu = 1; }
        else {
          if (MainMenu == 4 && SubMenu != 0 && FlagMenu == 0) { SubMenu++;  StartMillis = currentMillis;  delay(200);  if (SubMenu > 6)  SubMenu = 1; }
          else {
            if (MainMenu == 5 && SubMenu != 0 && FlagMenu == 0) { SubMenu++;  StartMillis = currentMillis;  delay(200);  if (SubMenu > 6)  SubMenu = 1; }
            else {
              if (MainMenu == 6 && SubMenu != 0 && FlagMenu == 0) { SubMenu++;  StartMillis = currentMillis;  delay(200);  if (SubMenu > 3)  SubMenu = 1; }
              else {
                if (MainMenu == 1 && FlagMenu == 1) {
                  switch (SubMenu) {
                  case 1: {  Temp -= FadeAmountTemp;                      StartMillis = currentMillis;  if (Temp <= minTempDanger)  Temp = minTempDanger;      delay(100);  break; }
                  case 2: {  deltaT -= FadeAmountTemp;                  StartMillis = currentMillis;  if (deltaT <= mindeltaT)  deltaT = mindeltaT;        delay(100);  break; }
                  case 3: {  Humiditi -= FadeAmountHum;                 StartMillis = currentMillis;  if (Humiditi <= MinimumHumiditi)  Humiditi = MinimumHumiditi;  delay(100);  break; }
                  case 4: {  deltaHumiditi -= FadeAmountdeltaHum;  StartMillis = currentMillis;  if (deltaHumiditi <= mindeltaHum)  deltaHumiditi = mindeltaHum;           delay(100);  break; }
                  case 5: {  k--; if (k < 0) k = 0; TypeHouse = VarHouse[k]; StartMillis = currentMillis; delay(200); EEPROM.write(135, TypeHouse); delay(100); break; }       
                  case 6: {  StartMillis = currentMillis; if (FanWorkFlag == 0) FanWorkFlag = 1; else if (FanWorkFlag == 1) FanWorkFlag = 0; delay(100);break; }
                }
                }
                else {
                  if (MainMenu == 2 && FlagMenu == 0 && SubMenu == 1) { bank--; StartMillis = currentMillis;  delay(200); if (bank < 0) bank = 0; }
                  else {
                    if (MainMenu == 3 && FlagMenu == 0 && SubMenu == 1) { bank--; StartMillis = currentMillis;  delay(200); if (bank < 0) bank = 0; }
                    else {
                      if (MainMenu == 4 && FlagMenu == 1) {
                        switch (SubMenu) {
                        case 1: {  NOWyear--;    StartMillis = currentMillis;  if (NOWyear < 2015) NOWyear = 2015; delay(200);  GoSetTime(NOWyear);  break; }
                        case 2: {  NOWmonth--;   StartMillis = currentMillis;  if (NOWmonth < 1) NOWmonth = 1;   delay(200);  GoSetTime(NOWmonth);  break; }
                        case 3: {  NOWday--;     StartMillis = currentMillis;  if (NOWday < 1) NOWday = 1;     delay(200);  GoSetTime(NOWday);  break; }
                        case 4: {  NOWhour--;    StartMillis = currentMillis;  if (NOWhour < 0) NOWhour = 0;    delay(200);  GoSetTime(NOWhour);  break; }
                        case 5: {  NOWminute--;  StartMillis = currentMillis;  if (NOWminute < 0) NOWminute = 0;  delay(200);  GoSetTime(NOWminute);  break; }
                        case 6: {  NOWsecond--;  StartMillis = currentMillis;  if (NOWsecond < 0) NOWsecond = 0;  delay(200);  GoSetTime(NOWsecond);  break; }
                        }
                      }
                      else {
                        if (MainMenu == 5 && FlagMenu == 1) {
                          switch (SubMenu) {
                          case 1: {  consKp -= 0.01;                    StartMillis = currentMillis; if (consKp >= 0.01)consKp = 0.01;      delay(100); EEPROM.write(131,consKp); break; }
                          case 2: {  consKi -= 0.01;                    StartMillis = currentMillis; if (consKp >= 0.01)consKp = 0.01;      delay(100); EEPROM.write(132,consKi); break; }
                          case 3: {  consKd -= 0.01;                    StartMillis = currentMillis; if (consKp >= 0.01)consKp = 0.01;      delay(100); EEPROM.write(133,consKd); break; }
                          case 4: { maxTempFanStart -= FadeAmountTemp;                      StartMillis = currentMillis;  if (maxTempFanStart <= maxTempFanStartMenuMin)  maxTempFanStart = maxTempFanStartMenuMin;        delay(100);  break; }
                          case 5: { TimeIntervalFanWork -= FadeTimeIntervalFanWork;     StartMillis = currentMillis;  if (TimeIntervalFanWork <= minTimeIntervalFanWork)  TimeIntervalFanWork = minTimeIntervalFanWork;  TimeIntervalFaningInterval = TimeIntervalFanWork * 60000;   delay(100);  break; }
                          case 6: { TimeFanWork -=FadeTimeFanWork;                             StartMillis = currentMillis;  if (TimeFanWork <= minTimeFanWork)  TimeFanWork = minTimeFanWork; TimeFaningInterval = TimeFanWork * 1000; delay(100);  break; }
                  }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      } break; 
     }
    case 4: {                                                                                                        // обработка события нажатия кнопки "ВЫБОР"
      if (MainMenu == 0 && SubMenu == 0) { MainMenu = 0;  SubMenu = 1;  delay(200); }
      else {
        if (MainMenu == 0 && SubMenu == 1) { MainMenu = 0;  SubMenu = 0;  delay(200); }
        //else {  if ( MainMenu == 2 && SubMenu == 1 ) { SaveToEEPROM ( bank ); lcd.setCursor(0, 1); lcd.print( "    saving...   " ); }
        //else {  if ( MainMenu == 3 && SubMenu == 1 ) { LoadFromEEPROM ( bank ); lcd.setCursor(0, 1); lcd.print( "    loading...  " ); }
        else {
          if (MainMenu != 0 && SubMenu != 0 && FlagMenu == 0) { FlagMenu = 1;  delay(200); }
          else { if (MainMenu != 0 && SubMenu != 0 && FlagMenu == 1) { FlagMenu = 0;  delay(200); } }
        }
      }
      break; }
    case 5: {                                                                                                        // обработка события нажатия кнопки "Открытие окон"
      StartFan();
      break; }
    }
  }
 
 
    switch (MainMenu) {
    case 0:      // главное меню "0"
      switch (SubMenu) {
      case 0:  m = 0; break;  // вывод на экран главного меню "0" подменю "0"
      case 1:  m = 1; break;  // вывод на экран главного меню "0" подменю "1"
      } 
      break;
    case 1:      // главное меню "1"
      switch (SubMenu) {
      case 0:  m = 10; break;  // вывод на экран главного меню "1" подменю "0"
      case 1:  m = 11; break;  // вывод на экран главного меню "1" подменю "1"
      case 2:  m = 12; break;  // вывод на экран главного меню "1" подменю "2"
      case 3:  m = 13; break;  // вывод на экран главного меню "1" подменю "3"
      case 4:  m = 14; break;  // вывод на экран главного меню "1" подменю "4"
      case 5:  m = 15; break;  // вывод на экран главного меню "1" подменю "5
      case 6:  m = 16; break;  // вывод на экран главного меню "1" подменю "5"
      } 
      break;
    case 2:      // главное меню "2"
      switch (SubMenu) {
      case 0:  m = 20; break;  // вывод на экран главного меню "2" подменю "0"
      case 1:  m = 21; break; // вывод на экран главного меню "2" подменю "1"
      } 
      break;
    case 3:      // главное меню "3"
      switch (SubMenu) {
      case 0:  m = 30; break;  // вывод на экран главного меню "3" подменю "0"
      case 1:  m = 31; break;  // вывод на экран главного меню "3" подменю "1"
      } 
      break;
    case 4:
      switch (SubMenu) {
      case 0:  m = 40; break;  // вывод на экран главного меню "4" подменю "0"
      case 1:  m = 41; break;  // вывод на экран главного меню "4" подменю "1"
      case 2:  m = 42; break;  // вывод на экран главного меню "4" подменю "2"
      case 3:  m = 43; break;  // вывод на экран главного меню "4" подменю "3"
      case 4:  m = 44; break;  // вывод на экран главного меню "4" подменю "4"
      case 5:  m = 45; break;  // вывод на экран главного меню "4" подменю "5"
      case 6:  m = 46; break;  // вывод на экран главного меню "4" подменю "6"
      } 
      break;
    case 5:
      switch (SubMenu) {
      case 0:  m = 50; break;  // вывод на экран главного меню "5" подменю "0"
      case 1:  m = 51; break;  // вывод на экран главного меню "5" подменю "1"
      case 2:  m = 52; break;  // вывод на экран главного меню "5" подменю "2"
      case 3:  m = 53; break;  // вывод на экран главного меню "5" подменю "3"
      case 4:  m = 54; break;  // вывод на экран главного меню "5" подменю "4"
      case 5:  m = 55; break;  // вывод на экран главного меню "5" подменю "5"
      case 6:  m = 56; break;  // вывод на экран главного меню "5" подменю "6"
      } 
      break;
    case 6:
      switch (SubMenu) {
      case 0:  m = 60; break;  // вывод на экран главного меню "6" подменю "0"
      case 1:  m = 61; break;  // вывод на экран главного меню "6" подменю "1"
      case 2:  m = 62; break;  // вывод на экран главного меню "6" подменю "2"
      case 3:  m = 63; break;  // вывод на экран главного меню "6" подменю "3"
      }
    }

    
 
 
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
  case 15: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("  Type HotHouse ")); lcd.setCursor(0, 1); lcd.print(F("Type =  "));switch(TypeHouse){case 0:lcd.print("earthe");lcd.print("hidro");case 2:lcd.print("aer");}               lcd.print("                  ");             PrintMenuWrite(FlagMenu);                         delay(100);                break; } 
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
