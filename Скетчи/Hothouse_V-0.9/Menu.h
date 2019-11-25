void Menu()
{ 
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
    }
    
 
 
