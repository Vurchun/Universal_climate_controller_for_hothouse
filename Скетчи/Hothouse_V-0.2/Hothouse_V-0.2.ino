#include <DS1307new.h>
#include <GyverEncoder.h>
#include <Wire.h>
#include <SPI.h>
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
#include <Blynk.h>
#include <PubSubClient.h>
#include <PubSubClient_JSON.h>
#include <stdlib.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <Adafruit_ESP8266.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
#define auth   "YourAuthToken"
char wapi[] = "GET /data/2.5/weather?q=Kyiv,ua&appid=c557d50e325f16c912651e1054527de7 HTTP/1.1";
char auth[] = "YourAuthToken"; 
char ssid[] = "WiFi";                      // Your WiFi credentials.
char pass[] = "10072003WiFi";            // Set password to "" for open networks.
#define host "api.openweathermap.org"
String line; 
  
Servo Rservo;
Servo Lservo;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  //LiquidCrystal lcd(11, 10, 9, 8, 7, 6);   инициализация входов на дисплей 20*4
WiFiClient client; // Use WiFi_funkClient class to create TCP connections
Adafruit_BME280 bme;
#define ESP_RX   1
#define ESP_TX   3
#define ESP_RST  17

// Must declare output stream before Adafruit_ESP8266 constructor; can be
// a SoftwareSerial stream, or Serial/Serial1/etc. for UART.
Encoder Enc(14, 12, 13, TYPE1);                     // CLK, DT, SW, тип (TYPE1 / TYPE2): TYPE1 одношаговый, TYPE2 двухшаговый. Если ваш энкодер работает странно, смените тип
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
#define mindeltaHum 1                                                 // минимальная дельта влажности для меню
#define maxdeltaHum 10                                                // максимальная дельта влажности для меню
#define FadeAmountdeltaHum 1                                         // шаг изминения дельты влажности для меню
#define FadeAmountHum 1                                               // шаг изменения влажности в меню
#define MaximumHumiditi 100                                  // максимальное значение влажности для меню
#define MinimumHumiditi 0                                 // минимальное значение влажности для меню

float tempK;                   // достаем температуру из структуры main
float tempC;                         // переводим кельвины в цельси
float tempKmin;            // и так далее
float tempCmin;
float tempKmax ;
float tempCmax ;
int pressurehPa;          // достаем давление из структуры main
float pressure;              // давление окружения
double Press ;               // давление внутри
int humidity ;             // достаем влажность из структуры main
float windspeed;             // достаем скорость ветра из структуры main
int winddeg;                   // достаем направление ветра из структуры main




int i = 0; int k = 0;
double Tnow;                                                          // Реальная температура на данный момент в инкубаторе на DS18b20
int hum;                                                              // Реальная влажность на данный момент в инкубаторе на DHT22
int MainMenu = 0, SubMenu = 0, FlagMenu = 0;                          // переменные для управления меню
int Rotate = 1;                                                       // направолние вращения двигателя (1-вправо, 0-влево)
int tf = 0;
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
PID myPID(&Tnow, &Output, &Temp, consKp, consKi, consKd, DIRECT);  //Specify the links and initial tuning parameters

uint8_t strelka_vverh_vniz[8] = { B00100, B01110, B11111, B00000, B11111, B01110, B00100 };     // закодирована в двоичной системе СТРЕЛКА ВВЕРХ ВНИЗ 
uint8_t temp_cel[8]           = { B11000, B11000, B00110, B01001, B01000, B01001, B00110 };     // закодирована в двоичной системе ГРАДУС ЦЕЛЬСИЯ
uint8_t temp_del[8]           = { B00000, B00100, B00100, B01010, B01010, B10001, B11111 };     // закодирована в двоичной системе ДЕЛЬТА
uint8_t   Hot_ON[8]           = { B01111, B10000, B00111, B00001, B01110, B10000, B01111 };     // закодирована в двоичной системе ОБОГРЕВ
uint8_t   Fan_ON[8]           = { B11011, B11011, B11011, B00100, B00100, B11011, B11011 };     // закодирована в двоичной системе ОБДУВ
uint8_t   Hum_ON[8]           = { B00100, B00100, B01110, B11011, B10111, B10111, B01110 };     // закодирована в двоичной системе УВЛАЖНЕНИЕ                             
uint8_t   WiFi_funk_ON[8]          = { B00000, B11111, B01110, B10101, B01110, B00100, B00000 };     // закодирована в двоичной системе WiFi_funk                             



#define PinHot  15                            // первоначальное подключение обогрева на реле №1 
#define PinLite  9                             // первоначальное подключениe обдувa на реле №2
#define PinHum  10                             // первоначальное подключение увлажнителя на реле №3

float voltage ;            // напряжение на аккумуляторе
int netpower ;           // наличие сети
 
int m;                                   // значения отображения информации на табле
int buttons_Menu;             // значения кнопок меню 
int PressingButtons;

unsigned long currentTime;
unsigned long loopTime;

unsigned long currentMillis;
unsigned long StartMillis = 0 ;                                              //счетчик прошедшего времени для AutoStartMenu
#define interval 15000                                                 //задержка автовозврата к StartMenu 7сек
unsigned long BME280readMillis = 0 ;                                           //счетчик прошедшего времени для интервала чтения с датчика температуры
#define BME280interval 100                                          //опрос датчика температуры каждую 1 сек
unsigned long MenuRewriteMillis = 0 ;                                          //счетчик прошедшего времени для обновления главного меню
#define MenuRewriteinterval 500                                        //обновление главного меню через 0,5сек
unsigned long WiFi_funkwriteMillis = 0;
unsigned long TimeFaning = 0;                                                    //счетчик прошедшего времени для продувки теплицы от СО2
unsigned long TimeIntervalFaning;                                          //счетчик прошедшего времени для интервала между продувками теплицы от СО2
unsigned long TimeFaningInterval = TimeFanWork * 1000;                 //длительность работы вентилятора при продувке теплицы от СО2 
unsigned long TimeIntervalFaningInterval = TimeIntervalFanWork * 60000;//длительность интервала между продувками теплицы от СО2

void setup()
{
  lcd.begin(20, 4);
  Wire.begin();
  Serial.begin(115200);             // Запускаем вывод данных на серийный порт 
  Rservo.attach(0);
  Lservo.attach(2);
  myPID.SetMode(AUTOMATIC);
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  Blynk.run();
  WiFi.begin(ssid, pass);
  Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,5), 8080);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  loopTime = currentTime;

  pinMode(PinHot, OUTPUT);                       // Реле №1 на 2 выходе
  pinMode(PinLite, OUTPUT);                       // Реле №2 на 3 выходе
  pinMode(PinHum, OUTPUT);                       // Реле №3 на 4 выходе
  digitalWrite(PinHot, HIGH);  digitalWrite(PinLite, HIGH);  digitalWrite(PinHum, HIGH);  // первоначальное отключение всех устройств при загрузке с паузой в 0,2сек чтобы не было скачка падения напряжения на БП
                                                   // --------------- чтение из EEPROM привязки номера исполнительного устройства к типу регулируемой велечины
                        // --------------- чтение из EEPROM привязки номера исполнительного устройства к типу регулируемой велечины



                          // --------------- чтение из EEPROM установок настройки теплицы сохраненных в банк настроек №0
  byte lowByte = EEPROM.read(0); delay(100);                                            // чтение lowByte температуры выращивания *100 из ячейки "0"
  byte higtByte = EEPROM.read(1); delay(100);                                            // чтение higtByte температуры выращивания *100 из ячейки "1"
  Temp = (((lowByte << 0) & 0xFF) + ((higtByte << 8) & 0xFF00)) / 100.00;
  lowByte = EEPROM.read(2); delay(100);                                                 // чтение lowByte дельтаТ температуры выращивания *100 из ячейки "2"
  higtByte = EEPROM.read(3); delay(100);                                                 // чтение higtByte дельтаТ температуры выращивания *100 из ячейки "3"
  deltaT = (((lowByte << 0) & 0xFF) + ((higtByte << 8) & 0xFF00)) / 100.00;
  Humiditi = EEPROM.read(4); delay(100);                                       // чтение дельты влажности выращивания из ячейки "4"
  deltaHumiditi = EEPROM.read(5);  delay(100);                                 // чтение дельты влажности выращивания из ячейки "5"
                                                                             // чтение времени через которое происходит переворот яиц из ячейки "6"
  lowByte = EEPROM.read(7);  delay(100);                                                // чтение lowByte температуры продувки *100 из ячейки "7"
  higtByte = EEPROM.read(8);  delay(100);                                                // чтение higtByte температуры продувки *100 из ячейки "8"
  maxTempFanStart = (((lowByte << 0) & 0xFF) + ((higtByte << 8) & 0xFF00)) / 100.00;
  lowByte = EEPROM.read(9);  delay(100);                                                // чтение lowByte времени вентиляции теплицы от СО2 из ячейки "9"
  higtByte = EEPROM.read(10); delay(100);                                                // чтение higtByte времени вентиляции теплицы от СО2 из ячейки "10"
  TimeFanWork = ((lowByte << 0) & 0xFF) + ((higtByte << 8) & 0xFF00);
  lowByte = EEPROM.read(11); delay(100);                                                // чтение lowByte интервала между вентиляциями теплицы от СО2 из ячейки "11"
  higtByte = EEPROM.read(12); delay(100);                                                // чтение higtByte интервала между вентиляциями теплицы от СО2 из ячейки "12"
  TimeIntervalFanWork = ((lowByte << 0) & 0xFF) + ((higtByte << 8) & 0xFF00);
  FanWorkFlag = EEPROM.read(13); delay(100);                                             // чтение флага активностивентиляциями теплицы от СО2 из ячейки "13"
  TimeFaningInterval = TimeFanWork * 1000;                                                 //чтение длительности работы вентилятора при продувке теплицы от СО2 
  TimeIntervalFaningInterval = TimeIntervalFanWork * 60000;                                //чтение длительности интервала между продувками теплицы от СО2
  consKp = EEPROM.read(131); delay(100);                                            //чтение Пропорционального коефициента             
  consKi = EEPROM.read(132); delay(100);                                            //чтение Интегрального коефициента                 
  consKd = EEPROM.read(133); delay(100);                                             //чтение Дефиринциального коефициента      
  

           
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



                                         
  lcd.createChar(1, strelka_vverh_vniz);  lcd.createChar(3, temp_cel);  lcd.createChar(4, temp_del);  lcd.createChar(5, Hot_ON);  lcd.createChar(6, Fan_ON);  lcd.createChar(7, Hum_ON); lcd.createChar(8, WiFi_funk_ON);

  lcd.setCursor(0, 0);  lcd.print("*-*-*-*-****-*-*-*-*");
  lcd.setCursor(0, 1);  lcd.print("   AutoHothouse   ");
  lcd.setCursor(0, 2);  lcd.print("  By_Vurchun_V-0.1  ");
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

void SaveToEEPROM(int BankSave)                                                         // запись данных во внутренний EEPROM 
{
  int TIC = int(Temp * 100); byte lowByte = ((TIC >> 0) & 0xFF); byte higtByte = ((TIC >> 8) & 0xFF);
  BS = BankSave * 20 + 0;     EEPROM.write(BS, lowByte);                  delay(100);    // запись lowByte температуры выращивания * 100 в ячейку "0" банка "bank"
  BS = BankSave * 20 + 1;     EEPROM.write(BS, higtByte);                 delay(100);    // запись higtByte температуры выращивания * 100 в ячейку "1" банка "bank"
  int DTIC = int(deltaT * 100);  lowByte = ((DTIC >> 0) & 0xFF); higtByte = ((DTIC >> 8) & 0xFF);
  BS = BankSave * 20 + 2;     EEPROM.write(BS, lowByte);                  delay(100);    // запись lowByte дельтаТ температуры выращивания * 100 в ячейку "2" банка "bank"
  BS = BankSave * 20 + 3;     EEPROM.write(BS, higtByte);                 delay(100);    // запись higtByte дельтаТ температуры выращивания * 100 в ячейку "3" банка "bank"
  BS = BankSave * 20 + 4;     EEPROM.write(BS, Humiditi);       delay(100);    // запись дельты влажности выращивания в ячейку "4" банка "bank"
  BS = BankSave * 20 + 5;     EEPROM.write(BS, deltaHumiditi);  delay(100);    // запись дельты влажности выращивания в ячейку "5" банка "bank"
                                                                                // запись времени через которое происходит переворот яиц в ячейку "6" банка "bank"
  int TICc = int(maxTempFanStart * 100); lowByte = ((TICc >> 0) & 0xFF); higtByte = ((TICc >> 8) & 0xFF);
  BS = BankSave * 20 + 7;     EEPROM.write(BS, lowByte);                  delay(100);    // запись lowByte температуры продувки * 100 в ячейку "7" банка "bank"
  BS = BankSave * 20 + 8;     EEPROM.write(BS, higtByte);                 delay(100);    // запись higtByte температуры продувки * 100 в ячейку "8" банка "bank"
  lowByte = ((TimeFanWork >> 0) & 0xFF); higtByte = ((TimeFanWork >> 8) & 0xFF);
  BS = BankSave * 20 + 9;     EEPROM.write(BS, lowByte);                  delay(100);    // запись lowByte времени вентиляции теплицы от СО2 в ячейку "9" банка "bank"
  BS = BankSave * 20 + 10;    EEPROM.write(BS, higtByte);                 delay(100);    // запись higtByte времени вентиляции теплицы от СО2 в ячейку "10" банка "bank"
  lowByte = ((TimeIntervalFanWork >> 0) & 0xFF); higtByte = ((TimeIntervalFanWork >> 8) & 0xFF);
  BS = BankSave * 20 + 11;    EEPROM.write(BS, lowByte);                  delay(100);    // запись lowByte интервала между вентиляциями теплицы от СО2 в ячейку "11" банка "bank"
  BS = BankSave * 20 + 12;    EEPROM.write(BS, higtByte);                 delay(100);    // запись higtByte интервала между вентиляциями теплицы от СО2 в ячейку "12" банка "bank"
  BS = BankSave * 20 + 13;    EEPROM.write(BS, FanWorkFlag);              delay(100);    // запись флага активностивентиляциями теплицы от СО2 в ячейку "13" банка "bank"
 }
void LoadFromEEPROM(int BankLoad)                                                       // загрузка данных из внутреннего EEPROM
{
  BL = BankLoad * 20 + 0; byte lowByte = EEPROM.read(BL);            delay(100);    // чтение lowByte температуры выращивания *100 из ячейки "0"
  BL = BankLoad * 20 + 1;     byte higtByte = EEPROM.read(BL);            delay(100);    // чтение higtByte температуры выращивания *100 из ячейки "1"
  Temp = (((lowByte << 0) & 0xFF) + ((higtByte << 8) & 0xFF00)) / 100.00;
  BL = BankLoad * 20 + 2;     lowByte = EEPROM.read(BL);                 delay(100);    // чтение lowByte дельтаТ температуры выращивания *100 из ячейки "2"
  BL = BankLoad * 20 + 3;     higtByte = EEPROM.read(BL);                 delay(100);    // чтение higtByte дельтаТ температуры выращивания *100 из ячейки "3"
  deltaT = (((lowByte << 0) & 0xFF) + ((higtByte << 8) & 0xFF00)) / 100.00;
  BL = BankLoad * 20 + 4;     Humiditi = EEPROM.read(BL);       delay(100);    // чтение дельты влажности выращивания из ячейки "4"
  BL = BankLoad * 20 + 5;     deltaHumiditi = EEPROM.read(BL);  delay(100);    // чтение дельты влажности выращивания из ячейки "5"
                                                                                            // чтение времени через которое происходит переворот яиц из ячейки "6"
  BL = BankLoad * 20 + 7;     lowByte = EEPROM.read(BL);                 delay(100);    // чтение lowByte температуры продувки *100 из ячейки "7"
  BL = BankLoad * 20 + 8;     higtByte = EEPROM.read(BL);                 delay(100);    // чтение higtByte температуры продувки *100 из ячейки "8"
  maxTempFanStart = (((lowByte << 0) & 0xFF) + ((higtByte << 8) & 0xFF00)) / 100.00;
  BL = BankLoad * 20 + 9;     lowByte = EEPROM.read(BL);  delay(100);                   // чтение lowByte времени вентиляции теплицы от СО2 из ячейки "9"
  BL = BankLoad * 20 + 10;    higtByte = EEPROM.read(BL);  delay(100);                   // чтение higtByte времени вентиляции теплицы от СО2 из ячейки "10"
  TimeFanWork = ((lowByte << 0) & 0xFF) + ((higtByte << 8) & 0xFF00);
  BL = BankLoad * 20 + 11;    lowByte = EEPROM.read(BL); delay(100);                    // чтение lowByte интервала между вентиляциями теплицы от СО2 из ячейки "11"
  BL = BankLoad * 20 + 12;    higtByte = EEPROM.read(BL); delay(100);                    // чтение higtByte интервала между вентиляциями теплицы от СО2 из ячейки "12"
  TimeIntervalFanWork = ((lowByte << 0) & 0xFF) + ((higtByte << 8) & 0xFF00);
  BL = BankLoad * 20 + 13;    FanWorkFlag = EEPROM.read(BL); delay(100);                 // чтение флага активностивентиляциями теплицы от СО2 из ячейки "13"
  TimeFaningInterval = TimeFanWork * 1000;                 //длительность работы вентилятора при продувке теплицы от СО2 
  TimeIntervalFaningInterval = TimeIntervalFanWork * 60000;//длительность интервала между продувками теплицы от СО2
}


void PressKeyMenu()                                                                      // Вычиление нажатия кнопок  
{
  PressingButtons = 0;
        if (Enc.isHolded() == 1)  PressingButtons = 1;          // меню     //при удержании кнопки  
  else  if (Enc.isRight()  == 1)  PressingButtons = 2;          // вверх    //при повороте направо
  else  if (Enc.isRight()  == 1)  PressingButtons = 3;          // вниз     //при повороте налево 
  else  if (Enc.isClick()  == 1)  PressingButtons = 4;          // выбор    //при нажатии и отпускании кнопки  
  else  if (Enc.isFastR() == 1 || Enc.isFastL() == 1)  PressingButtons = 5;          // переворот    //при быстром повороте     
  else  PressingButtons = 0; 
  Serial.print("  Button key =");Serial.print(PressingButtons); Serial.println(" ");delay(100);           
}

void PreSetTime()
{
  Setyear = RTC.year; Setmonth = RTC.month; Setday = RTC.day; Sethour = RTC.hour; Setminute = RTC.minute; Setsecond = RTC.second;
}
void GoSetTime(int InputX)
{
  switch (SubMenu) {
  case 1:  Setyear = InputX;    break;
  case 2:  Setmonth = InputX;   break;
  case 3:  Setday = InputX;     break;
  case 4:  Sethour = InputX;    break;
  case 5:  Setminute = InputX;  break;
  case 6:  Setsecond = InputX;  break;
  }
}
void SetTime()
{
  RTC.getTime();
  RTC.stopClock();
  RTC.fillByYMD(Setyear, Setmonth, Setday);      delay(250);
  RTC.fillByHMS(Sethour, Setminute, Setsecond);  delay(250);
  RTC.setTime();
  delay(100);
  RTC.startClock();
}
void PrintMenuWrite(int FlagM)
{
  switch (FlagM) {
  case 0:  lcd.setCursor(15, 1);  lcd.print("\1");  break;
  case 1:  lcd.setCursor(15, 1);  lcd.print("*");   break;
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
       if (FlagTempFan == 1 || FlagTimeFan == 1 && windspeed < 15) { Rservo.write(180); Lservo.write(180);lcd.setCursor(19, 2); lcd.print("\6"); }
  else if (FlagTempFan == 1 || FlagTimeFan == 1 && windspeed < 10 && winddeg < 45  && winddeg > 135) { Rservo.write(0); Rservo.write(90);  lcd.setCursor(19, 2); lcd.print("\6"); }
  else if (FlagTempFan == 1 || FlagTimeFan == 1 && windspeed < 10 && winddeg > 315 && winddeg < 225) { Lservo.write(0); Lservo.write(90); lcd.setCursor(19, 2); lcd.print("\6"); }
  else { Rservo.write(0); Rservo.write(0); lcd.setCursor(19, 2); lcd.print(" "); }
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
}
void StartLite()
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
}
void TempRead()                                                     // Чтение значений с датчика BME280
{
  if (currentMillis - BME280readMillis > BME280interval)
  {
    Temp = bme.readTemperature();
    Press = bme.readPressure() / 100.0F;  
    hum = bme.readHumidity();
    }
}

void Get() {
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {      /// подключаемся к серверу 
    Serial.println("connection failed");
    return;
  }
   /// если подключились, отправляем чего от сервера хотим
              // сам GET запрос с ID и ключем
    client.println("GET /data/2.5/weather?q=Kyiv,ua&appid=c557d50e325f16c912651e1054527de7 HTTP/1.1");
              // говорим к какому хосту обращаемся (на сервере может быть несколько сайтов)
    client.println("Host: api.openweathermap.org");
              // говорим что закончили
    client.println("Connection: close");
    client.println();
 
  delay(1000);  // ждем немного 
                // читаем ответ и отправляем его в Serial
  while(client.available()){
   String line = client.readStringUntil('\r'); 
    Serial.print(line);
  }
 
  Serial.println();
  Serial.println("closing connection");
}

void jsonGet() {
  
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
    client.println(wapi);
    client.println("Host: api.openweathermap.org");
    client.println("Connection: close");
    client.println();
 
  delay(1500);
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    line = client.readStringUntil('\r'); 
  }
  Serial.print(line);
  Serial.println();
  Serial.println("closing connection");
}
void WiFi_funk()
{jsonGet();
     StaticJsonBuffer<2000> jsonBuffer;                   /// буфер на 2000 символов
   JsonObject& root = jsonBuffer.parseObject(line);     // скармиваем String
   if (!root.success()) {
    Serial.println("parseObject() failed");             // если ошибка, сообщаем об этом
     jsonGet();                                         // пинаем сервер еще раз
    return;                                             // и запускаем заного 
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

  
  voltage = analogRead(17) * 5.72 / 1024 * 4.575;     //подщёт заряда аккумулятора
  if( voltage > 13.8 ) netpower = 1;
  else netpower = 0;
  delay(150);
    lcd.setCursor(18, 3); lcd.print("\8");           //Отправка значений на сервер                   
    Blynk.virtualWrite(V1, Tnow);                  
    Blynk.virtualWrite(V2, Temp); 
    Blynk.virtualWrite(V3, hum);                    
    Blynk.virtualWrite(V4, Humiditi);
    Blynk.virtualWrite(V5, netpower); 
    Blynk.virtualWrite(V6, voltage); 
    BLYNK_WRITE(V20) {Temp = param.asStr();                }
    BLYNK_WRITE(V21) {Humiditi = param.asStr();            }
    BLYNK_WRITE(V22) {ssid = param.asStr();     EEPROM.write(130, ssid); delay(10);}
    BLYNK_WRITE(V23) {pass = param.asStr();     EEPROM.write(131, pass); delay(10);}
    BLYNK_WRITE(V24) {wapi = param.asStr();     EEPROM.write(132, wapi); delay(10);} 
    BLYNK_WRITE(V25) {TimeIntervalFanWork = param.asStr(); }
    BLYNK_WRITE(V26) {TimeFanWork = param.asStr();         }
    BLYNK_WRITE(V27) {maxTempFanStart = param.asStr();     }
    BLYNK_WRITE(V28) {deltaT = param.asStr();              }
    BLYNK_WRITE(V29) {deltaHumiditi = param.asStr();       }
  delay(100) SaveToEEPROM(0);             
  delay(250);                  
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
  if (currentTime >= (loopTime))
  {
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
                  case 1: {  Temp += FadeAmountTemp;                    StartMillis = currentMillis; if (Temp >= maxTempDanger)Temp = maxTempDanger;      delay(100);  break; }
                  case 2: {  deltaT += FadeAmountTemp;                StartMillis = currentMillis; if (deltaT >= maxdeltaT)deltaT = maxdeltaT;        delay(100);  break; }
                  case 3: {  Humiditi += FadeAmountHum;               StartMillis = currentMillis; if (Humiditi >= MaximumHumiditi)Humiditi = MaximumHumiditi;  delay(100);  break; }
                  case 4: {  deltaHumiditi += + FadeAmountdeltaHum; StartMillis = currentMillis; if (deltaHumiditi >= maxdeltaHum)deltaHumiditi = maxdeltaHum;           delay(100);  break; }
                  case 5: { StartMillis = currentMillis; if (FanWorkFlag == 0) FanWorkFlag = 1; else if (FanWorkFlag == 1) FanWorkFlag = 0; delay(100);break; }
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
                  case 5: { StartMillis = currentMillis; if (FanWorkFlag == 0) FanWorkFlag = 1; else if (FanWorkFlag == 1) FanWorkFlag = 0; delay(100);break; }
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
    case 5: {                                                                                                        // обработка события нажатия кнопки "ПЕРЕВОРОТ"
      StartFan();
      break; }
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
      case 5:  m = 15; break;  // вывод на экран главного меню "1" подменю "5"
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

    if (MainMenu == 4 && SubMenu != 0) {}
    else {
      if (currentMillis - StartMillis > interval) {
        StartMillis = currentMillis;
        if (MainMenu == 0 && SubMenu == 0) { MainMenu = 0;  SubMenu = 1; }
        else {
          if (MainMenu == 0 && SubMenu == 1) { MainMenu = 0;  SubMenu = 0; }
          else { MainMenu = 0;  SubMenu = 0; }
        }
      }
    }
  }
  switch (m) {
  case 0: {  lcd.clear();lcd.setCursor(0, 0);  lcd.print("T="); lcd.print(Tnow); lcd.print("\3 (");lcd.print(Temp);    lcd.print("\3)");  
             lcd.setCursor(0, 1);  lcd.print("H="); lcd.print(hum);  lcd.print("%("); lcd.print(Humiditi); lcd.print("%)");
             if( voltage >= 13 ){lcd.print("220V - ON"); }else{ lcd.print("Bat="); lcd.print(voltage);}
             lcd.setCursor(0, 2);
             if (RTC.hour < 10) lcd.print(0); lcd.print(RTC.hour); lcd.print(":"); if (RTC.minute < 10) lcd.print(0); lcd.print(RTC.minute); lcd.print(":"); if (RTC.second < 10) lcd.print(0); lcd.print(RTC.second);
             TempRead(); StartFan();;  StartHot();  StartHum(); TimerCalculatePrint();WiFi_funk();FlagMenu = 0;             break; }
  case 10: {  lcd.clear(); lcd.setCursor(0, 1); lcd.print(F("    Setting     ")); lcd.setCursor(0, 2); lcd.print(F("   incubation   ")); lcd.setCursor(15, 1);           lcd.print("\1                ");                                                               delay(100);FlagMenu = 0;   break; }
  case 11: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("Temperature inc ")); lcd.setCursor(0, 1); lcd.print(F("t =             ")); lcd.print(Temp);                lcd.print("\3                ");             PrintMenuWrite(FlagMenu);                         delay(100);                break; }
  case 12: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("    Delta T     ")); lcd.setCursor(0, 1); lcd.print(F("\4t =           ")); lcd.print(deltaT);              lcd.print("\3                ");             PrintMenuWrite(FlagMenu);                         delay(100);                break; }
  case 13: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("   Humiditi     ")); lcd.setCursor(0, 1); lcd.print(F("H =             ")); lcd.print(Humiditi);            lcd.print("%                 ");             PrintMenuWrite(FlagMenu);                         delay(100);                break; }
  case 14: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("    Delta H     ")); lcd.setCursor(0, 1); lcd.print(F("\4h =           ")); lcd.print(deltaHumiditi);       lcd.print("%                 ");             PrintMenuWrite(FlagMenu);                         delay(100);                break; }
  case 20: {  lcd.clear(); lcd.setCursor(0, 1); lcd.print(F("  Save setting  ")); lcd.setCursor(0, 2); lcd.print(F("   to  EEPROM   ")); lcd.setCursor(15, 1);           lcd.print("\1                ");                                                               delay(100); FlagMenu = 0;  break; }
  case 21: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("Save setting to ")); lcd.setCursor(0, 1); lcd.print(F("bank ")); lcd.print(bank); if (FlagMenu == 0) {      lcd.print(" press set"); delay(100); } else { SaveToEEPROM(bank);   lcd.print(" saving...");   delay(100); }              break; }
  case 30: {  lcd.clear(); lcd.setCursor(0, 1); lcd.print(F("  Load setting  ")); lcd.setCursor(0, 2); lcd.print(F("  from  EEPROM  ")); lcd.setCursor(15, 1);           lcd.print("\1                ");                                                               delay(100); FlagMenu = 0;  break; }
  case 31: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("Load setting    ")); lcd.setCursor(0, 1); lcd.print(F("bank ")); lcd.print(bank); if (FlagMenu == 0) {      lcd.print(" press set"); delay(100); } else { LoadFromEEPROM(bank); lcd.print(" loading..");   delay(100); }              break; }
  case 40: {  lcd.clear(); lcd.setCursor(0, 1); lcd.print(F("      Time      ")); lcd.setCursor(0, 2); lcd.print(F("     setting    ")); lcd.setCursor(15, 1);           lcd.print("\1                ");                                                               delay(100); FlagMenu = 0;  break; }
  case 41: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("  setup  year   ")); lcd.setCursor(5, 1); lcd.print(Setyear);                                                                                           PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 42: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("  setup  month  ")); lcd.setCursor(6, 1); lcd.print(Setmonth);                                                                                          PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 43: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("   setup  day   ")); lcd.setCursor(6, 1); lcd.print(Setday);                                                                                            PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 44: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("  setup  hour   ")); lcd.setCursor(6, 1); lcd.print(Sethour);                                                                                           PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 45: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("  setup minute  ")); lcd.setCursor(6, 1); lcd.print(Setminute);                                                                                         PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 46: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("  setup second  ")); lcd.setCursor(6, 1); lcd.print(Setsecond);                                                                                         PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 50: {  lcd.clear(); lcd.setCursor(0, 1); lcd.print(F("    Exstra      ")); lcd.setCursor(0, 2); lcd.print("     Setting      ");  lcd.setCursor(15, 1);           lcd.print("\1                ");                                                               delay(100); FlagMenu = 0;  break; }
  case 51: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("      K p       ")); lcd.setCursor(0, 1); lcd.print(F("P = "));             lcd.print(consKp);                                                          PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 52: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("      K i       ")); lcd.setCursor(0, 1); lcd.print(F("I = "));             lcd.print(consKi);                                                          PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 53: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("      K d       ")); lcd.setCursor(0, 1); lcd.print(F("D = "));             lcd.print(consKd);                                                          PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 54: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("   Cooling T    ")); lcd.setCursor(0, 1); lcd.print(F("t = "));             lcd.print(maxTempFanStart);     lcd.print("\3                ");            PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 55: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("CO2 faning time ")); lcd.setCursor(0, 1); lcd.print(F("time = "));          lcd.print(TimeFanWork);         lcd.print(" sec.             ");            PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  case 56: {  lcd.clear(); lcd.setCursor(0, 0); lcd.print(F("CO2 fan interval")); lcd.setCursor(0, 1); lcd.print(F("time = "));          lcd.print(TimeIntervalFanWork); lcd.print(" min.             ");            PrintMenuWrite(FlagMenu);                          delay(100);                break; }
  }}
