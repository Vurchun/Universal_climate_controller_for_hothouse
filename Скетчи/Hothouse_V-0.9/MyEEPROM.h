
void WriteBit(float Write)
{ Bit[0] = floor(Write);
  Bit[1] = floor((Write - Bit[0]) * 100);
   } 
void WriteZero(float Write)
{ int zero = 0;
  while(Write > 10){Write = Write / 10; zero++;}
  Bit[0] = Write * 10; Bit[1] = zero - 1;
}

float ReadBit()
{ float Read = (Bit[0] * 100 + Bit[1]);
  Read = Read / 100;
  return Read;
  }
float ReadZero()
{
  float Read = Bit[0] * pow(10,Bit[1]);
  return Read;
}

void SaveToEEPROM(int BankSave)                                                         // запись данных во внутренний EEPROM 
{
  WriteBit(Temp);
  BS = BankSave * 20 + 0;     EEPROM.write(BS, Bit[0]);           delay(50);    // запись  температуры выращивания в ячейку "0" банка "bank"
  BS = BankSave * 20 + 1;     EEPROM.write(BS, Bit[1]);           delay(50);    // запись  температуры выращивания в ячейку "1" банка "bank"

 WriteBit(deltaT);
  BS = BankSave * 20 + 2;     EEPROM.write(BS, Bit[0]);           delay(50);    // запись дельты температуры выращивания в ячейку "2" банка "bank"
  BS = BankSave * 20 + 3;     EEPROM.write(BS, Bit[1]);           delay(50);    // запись дельты температуры выращивания в ячейку "3" банка "bank"

  BS = BankSave * 20 + 4;     EEPROM.write(BS, Humiditi);         delay(50);    // запись дельты влажности выращивания в ячейку "4" банка "bank"
  BS = BankSave * 20 + 5;     EEPROM.write(BS, deltaHumiditi);    delay(50);    // запись дельты влажности выращивания в ячейку "5" банка "bank"
  
 WriteBit(maxTempFanStart);                                                                              
  BS = BankSave * 20 + 6;     EEPROM.write(BS, Bit[0]);           delay(50);    // запись  температуры продувки в ячейку "6" банка "bank"
  BS = BankSave * 20 + 7;     EEPROM.write(BS, Bit[1]);           delay(50);    // запись  температуры продувки в ячейку "7" банка "bank"
 
 WriteZero(TimeFanWork);
  BS = BankSave * 20 + 8;     EEPROM.write(BS, Bit[0]);           delay(50);    // запись  температуры продувки в ячейку "8" банка "bank"
  BS = BankSave * 20 + 9;     EEPROM.write(BS, Bit[1]);           delay(50);   
  
 WriteZero(TimeIntervalFanWork);
  BS = BankSave * 20 + 10;     EEPROM.write(BS, Bit[0]);          delay(50);    // запись  времени вентиляции теплицы от СО2 в ячейку "9" банка "bank"
  BS = BankSave * 20 + 11;     EEPROM.write(BS, Bit[1]);          delay(50);    // запись  температуры продувки  в ячейку "8" банка "bank"
 
  BS = BankSave * 20 + 13;    EEPROM.write(BS, FanWorkFlag);      delay(50);    // запись флага активностивентиляциями теплицы от СО2 в ячейку "13" банка "bank"
  BS = BankSave * 20 + 14;    EEPROM.write(BS, HumGround);        delay(50);    // запись Нужной вдоажности почвы в ячейку "14" банка "bank"

 WriteBit(consKp);                            
  BS =                131;    EEPROM.write(BS, Bit[0]);           delay(50);    // запись флага активностивентиляциями теплицы от СО2 в ячейку "13" банка "bank"
  BS =                132;    EEPROM.write(BS, Bit[1]);           delay(50);    // запись флага активностивентиляциями теплицы от СО2 в ячейку "13" банка "bank"
 WriteBit(consKi);
  BS =                133;    EEPROM.write(BS, Bit[0]);           delay(50);    // запись Нужной вдоажности почвы в ячейку "14" банка "bank"
  BS =                134;    EEPROM.write(BS, Bit[1]);           delay(50);    // запись Нужной вдоажности почвы в ячейку "14" банка "bank"
  
 WriteBit(consKd);
  BS =                135;    EEPROM.write(BS, Bit[0]);           delay(50);    // запись флага активностивентиляциями теплицы от СО2 в ячейку "13" банка "bank"
  BS =                136;    EEPROM.write(BS, Bit[1]);           delay(50);    // запись флага активностивентиляциями теплицы от СО2 в ячейку "13" банка "bank"
  
  BS =                137;    EEPROM.write(BS, TypeHouse);        delay(50);    // запись Нужной вдоажности почвы в ячейку "14" банка "bank"
   EEPROM.commit();
 }
 
void LoadFromEEPROM(int BankLoad)                                                       // загрузка данных из внутреннего EEPROM
{
  BL = BankLoad * 20 + 0;     Bit[0] = EEPROM.read(BL);            delay(50);    // чтение  температуры выращивания  из ячейки "0"
  BL = BankLoad * 20 + 1;     Bit[1] = EEPROM.read(BL);            delay(50);    // чтение  температуры выращивания  из ячейки "1"
 Temp = ReadBit();
 
  BL = BankLoad * 20 + 2;     Bit[0] = EEPROM.read(BL);            delay(50);    // чтение  дельтаТ температуры выращивания  из ячейки "2"
  BL = BankLoad * 20 + 3;     Bit[1] = EEPROM.read(BL);            delay(50);    // чтение  дельтаТ температуры выращивания  из ячейки "3"
 deltaT = ReadBit();
 
  BL = BankLoad * 20 + 4;     Humiditi = EEPROM.read(BL);          delay(50);    // чтение дельты влажности выращивания из ячейки "4"
  BL = BankLoad * 20 + 5;     deltaHumiditi = EEPROM.read(BL);     delay(50);    // чтение дельты влажности выращивания из ячейки "5"
                                                                                         
  BL = BankLoad * 20 + 6;     Bit[0] = EEPROM.read(BL);            delay(50);    // чтение  температуры продувки  из ячейки "7"
  BL = BankLoad * 20 + 7;     Bit[1] = EEPROM.read(BL);            delay(50);    // чтение  температуры продувки  из ячейки "8"
 maxTempFanStart = ReadBit();
 
  BL = BankLoad * 20 + 8;     Bit[0] = EEPROM.read(BL);            delay(50);     // чтение  времени вентиляции теплицы от СО2 из ячейки "9"
  BL = BankLoad * 20 + 9;     Bit[1] = EEPROM.read(BL);            delay(50);     // чтение  времени вентиляции теплицы от СО2 из ячейки "10"
 TimeFanWork =  ReadZero();
 
  BL = BankLoad * 20 + 10;    Bit[0] = EEPROM.read(BL);            delay(50);      // чтение  интервала между вентиляциями теплицы от СО2 из ячейки "11"
  BL = BankLoad * 20 + 11;    Bit[1] = EEPROM.read(BL);            delay(50);      // чтение  интервала между вентиляциями теплицы от СО2 из ячейки "12"
 TimeIntervalFanWork = ReadZero();
 
  BL = BankLoad * 20 + 12;    FanWorkFlag = EEPROM.read(BL);       delay(50);     // чтение флага активностивентиляциями теплицы от СО2 из ячейки "13"
  BL = BankLoad * 20 + 13;    HumGround = EEPROM.read(BL);         delay(50);     // чтение Нужной вдоажности почвы в ячейку "14" банка "bank"
 
  BL =                131;    Bit[0] = EEPROM.read(BL);            delay(50);    //чтение Пропорционального коефициента             
  BL =                132;    Bit[1] = EEPROM.read(BL);            delay(50);    //чтение Интегрального коефициента                 
 consKp = ReadBit();
  
  BL =                133;    Bit[0] = EEPROM.read(BL);            delay(50);    //чтение Дефиринциального коефициента 
  BL =                134;    Bit[1] = EEPROM.read(BL);            delay(50);    //чтение Пропорционального коефициента             
 consKi = ReadBit();
  
  BL =                135;    Bit[0] = EEPROM.read(BL);            delay(50);    //чтение Интегрального коефициента                 
  BL =                136;    Bit[1] = EEPROM.read(BL);            delay(50);    //чтение Дефиринциального коефициента 
 consKd = ReadBit();
  
  BL =                137;    TypeHouse = EEPROM.read(BL);        delay(50);    //чтение Типа теплицы 
  }
  
