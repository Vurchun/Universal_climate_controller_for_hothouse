#include <ArduinoJson.h>
#include <MQTT.h>
#include <PubSubClient.h>
#include <PubSubClient_JSON.h>
#include <ESP8266WiFi.h>
#include <stdlib.h>
#include <FastIO.h>
#include <I2CIO.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_SR.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal_SR2W.h>
#include <LiquidCrystal_SR3W.h>

#define ssid  "Kyivstar_Alpha"  // Имя вайфай точки доступа
#define pass  "R667fksX55fr" // Пароль от точки доступа

#define mqtt_server "m21.cloudmqtt.com" // Имя сервера MQTT
#define mqtt_port 19787 // Порт для подключения к серверу MQTT
#define mqtt_user "udciowys" // Логи от сервер
#define mqtt_pass "ZXBwDFYgAU_K" // Пароль от сервера
#define mqtt_id "HotHouse" // Пароль от сервера

#define host "api.openweathermap.org"
#define wapi  "GET /data/2.5/weather?q=Kyiv,ua&appid=c557d50e325f16c912651e1054527de7 HTTP/1.1"
#define httpPort  80

String line;
int NumBankSave = 0; 
int IfBankSave  = 0;

Serial.begin(115200);             // Запускаем вывод данных на серийный порт lcd.begin(20, 4);
  Wire.begin(SDA_PIN, SCL_PIN);
 


  WiFi.begin(ssid, pass);
 Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
  Serial.print("Connecting to ");
  Serial.println(ssid);

  lcd.setCursor(0, 0);  lcd.print("   Connecting to    ");
  lcd.setCursor(0, 1);  lcd.print(ssid);
  lcd.setCursor(0, 2);  lcd.print("       Pass         ");
  lcd.setCursor(0, 3);  lcd.print(pass);
  Serial.println();

    Serial.print(".");
    delay(1000);
  }
  
   Serial.println("WiFi connected");  
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());
   
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
   int i = 0,ReadySaved=0;
  client.publish("Hothouse/ReadySaved",String(ReadySaved));     
  
    while (i<10){
    lcd.setCursor(19, 3); lcd.print("\2");           //Отправка значений на сервер    
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

   
    lcd.setCursor(19, 3); lcd.print("\2");           //Отправка значений на сервер    
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
          
