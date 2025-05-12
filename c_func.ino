// различные функции
#include <EEPROM.h>  // Підключаємо бібліотеку для роботи з EEPROM
byte shotPosSerw[NUM_SHOTS];

void savePositionsToEEPROM() {
  for (byte i = 0; i < NUM_SHOTS; i++) {
    EEPROM.write(i, shotPosSerw[i]);  // Записуємо нові позиції
  }
}

void loadPositionsFromEEPROM() {
  for (byte i = 0; i < NUM_SHOTS; i++) {
    byte pos = EEPROM.read(i);      // Зчитуємо значення
    if (pos < 0 || pos > 180) {     // Якщо значення некоректне
      shotPosSerw[i] = shotPos[i];  // Використовуємо значення за замовчуванням
    } else {
      shotPosSerw[i] = pos;  // Використовуємо значення з EEPROM
    }
  }
}

void serviceMode() {
  if (!digitalRead(BTN_PIN)) {
    byte serviceText[] = { _S, _E, _r, _U, _i, _C, _E };
    disp.runningString(serviceText, sizeof(serviceText), 150);
    while (!digitalRead(BTN_PIN))
      ;  // ждём отпускания
    delay(200);
    servoON();
    int servoPos = 0;
    long pumpTime = 0;
    timerMinim timer100(100);
    disp.displayInt(0);
    bool flag;
    for (;;) {
      servo.tick();
      enc.tick();

      if (timer100.isReady()) {  // период 100 мс
        // работа помпы со счётчиком
        if (!digitalRead(ENC_SW)) {
          if (flag) pumpTime += 100;
          else pumpTime = 0;
          disp.displayInt(pumpTime);
          pumpON();
          flag = true;
        } else {
          pumpOFF();
          flag = false;
        }

        // зажигаем светодиоды от кнопок
        for (byte i = 0; i < NUM_SHOTS; i++) {
          if (!digitalRead(SW_pins[i])) {
            strip.setLED(i, mCOLOR(GREEN));
          } else {
            strip.setLED(i, mCOLOR(BLACK));
          }
          strip.show();
        }
      }

      if (enc.isTurn()) {
        // крутим серво от энкодера
        pumpTime = 0;
        if (enc.isLeft()) {
          servoPos += 2;
        }
        if (enc.isRight()) {
          servoPos -= 2;
        }
        servoPos = constrain(servoPos, 0, 180);
        disp.displayInt(servoPos);
        servo.setTargetDeg(servoPos);
      }

      if (btn.holded()) {
        servo.setTargetDeg(0);
        break;
      }
    }
  }
  disp.clear();
  while (!servo.tick())
    ;
  servoOFF();
}

// выводим объём и режим
void dispMode() {
  disp.displayInt(thisVolume);
  if (workMode) disp.displayByte(0, _A);
  else {
    disp.displayByte(0, _P);
    pumpOFF();
  }
}

// наливайка, опрос кнопок
void flowTick() {
  if (FLOWdebounce.isReady()) {
    for (byte i = 0; i < NUM_SHOTS; i++) {
      bool swState;
      int sensorValue = analogRead(SW_pins[i]);
      byte pinIndex = i / 2;  // 0 для A0, 1 для A1, і т.д.

      // Показ значень на екрані для калібрування
      switch(pinIndex) {
        case 0: if (A0_SHOW_VALUE) disp.displayInt(sensorValue); break;
        case 1: if (A1_SHOW_VALUE) disp.displayInt(sensorValue); break;
        case 2: if (A2_SHOW_VALUE) disp.displayInt(sensorValue); break;
        case 3: if (A3_SHOW_VALUE) disp.displayInt(sensorValue); break;
        case 4: if (A4_SHOW_VALUE) disp.displayInt(sensorValue); break;
        case 5: if (A5_SHOW_VALUE) disp.displayInt(sensorValue); break;
      }

      // Обробка для всіх пар стаканів (A0-A5)
      if (i % 2 == 0) {  // Перший стакан пари
        switch(pinIndex) {
          case 0:  // A0
            if (sensorValue > A0_BOTH - SENSOR_TOLERANCE && sensorValue < A0_BOTH + SENSOR_TOLERANCE) {
              swState = true;
            } else {
              swState = (sensorValue > A0_FIRST - SENSOR_TOLERANCE && sensorValue < A0_FIRST + SENSOR_TOLERANCE);
            }
            break;
          case 1:  // A1
            if (sensorValue > A1_BOTH - SENSOR_TOLERANCE && sensorValue < A1_BOTH + SENSOR_TOLERANCE) {
              swState = true;
            } else {
              swState = (sensorValue > A1_FIRST - SENSOR_TOLERANCE && sensorValue < A1_FIRST + SENSOR_TOLERANCE);
            }
            break;
          case 2:  // A2
            if (sensorValue > A2_BOTH - SENSOR_TOLERANCE && sensorValue < A2_BOTH + SENSOR_TOLERANCE) {
              swState = true;
            } else {
              swState = (sensorValue > A2_FIRST - SENSOR_TOLERANCE && sensorValue < A2_FIRST + SENSOR_TOLERANCE);
            }
            break;
          case 3:  // A3
            if (sensorValue > A3_BOTH - SENSOR_TOLERANCE && sensorValue < A3_BOTH + SENSOR_TOLERANCE) {
              swState = true;
            } else {
              swState = (sensorValue > A3_FIRST - SENSOR_TOLERANCE && sensorValue < A3_FIRST + SENSOR_TOLERANCE);
            }
            break;
          case 4:  // A4
            if (sensorValue > A4_BOTH - SENSOR_TOLERANCE && sensorValue < A4_BOTH + SENSOR_TOLERANCE) {
              swState = true;
            } else {
              swState = (sensorValue > A4_FIRST - SENSOR_TOLERANCE && sensorValue < A4_FIRST + SENSOR_TOLERANCE);
            }
            break;
          case 5:  // A5
            if (sensorValue > A5_BOTH - SENSOR_TOLERANCE && sensorValue < A5_BOTH + SENSOR_TOLERANCE) {
              swState = true;
            } else {
              swState = (sensorValue > A5_FIRST - SENSOR_TOLERANCE && sensorValue < A5_FIRST + SENSOR_TOLERANCE);
            }
            break;
        }
      } else {  // Другий стакан пари
        switch(pinIndex) {
          case 0:  // A0
            if (sensorValue > A0_BOTH - SENSOR_TOLERANCE && sensorValue < A0_BOTH + SENSOR_TOLERANCE) {
              swState = true;
            } else {
              swState = (sensorValue > A0_SECOND - SENSOR_TOLERANCE && sensorValue < A0_SECOND + SENSOR_TOLERANCE);
            }
            break;
          case 1:  // A1
            if (sensorValue > A1_BOTH - SENSOR_TOLERANCE && sensorValue < A1_BOTH + SENSOR_TOLERANCE) {
              swState = true;
            } else {
              swState = (sensorValue > A1_SECOND - SENSOR_TOLERANCE && sensorValue < A1_SECOND + SENSOR_TOLERANCE);
            }
            break;
          case 2:  // A2
            if (sensorValue > A2_BOTH - SENSOR_TOLERANCE && sensorValue < A2_BOTH + SENSOR_TOLERANCE) {
              swState = true;
            } else {
              swState = (sensorValue > A2_SECOND - SENSOR_TOLERANCE && sensorValue < A2_SECOND + SENSOR_TOLERANCE);
            }
            break;
          case 3:  // A3
            if (sensorValue > A3_BOTH - SENSOR_TOLERANCE && sensorValue < A3_BOTH + SENSOR_TOLERANCE) {
              swState = true;
            } else {
              swState = (sensorValue > A3_SECOND - SENSOR_TOLERANCE && sensorValue < A3_SECOND + SENSOR_TOLERANCE);
            }
            break;
          case 4:  // A4
            if (sensorValue > A4_BOTH - SENSOR_TOLERANCE && sensorValue < A4_BOTH + SENSOR_TOLERANCE) {
              swState = true;
            } else {
              swState = (sensorValue > A4_SECOND - SENSOR_TOLERANCE && sensorValue < A4_SECOND + SENSOR_TOLERANCE);
            }
            break;
          case 5:  // A5
            if (sensorValue > A5_BOTH - SENSOR_TOLERANCE && sensorValue < A5_BOTH + SENSOR_TOLERANCE) {
              swState = true;
            } else {
              swState = (sensorValue > A5_SECOND - SENSOR_TOLERANCE && sensorValue < A5_SECOND + SENSOR_TOLERANCE);
            }
            break;
        }
      }

      if (swState && shotStates[i] == NO_GLASS) {
        timeoutReset();
        shotStates[i] = EMPTY;
        strip.setLED(i, mCOLOR(RED));
        LEDchanged = true;
        DEBUG("set glass");
        DEBUG(i);
      }
      if (!swState && shotStates[i] != NO_GLASS) {
        shotStates[i] = NO_GLASS;
        strip.setLED(i, mCOLOR(BLACK));
        LEDchanged = true;
        timeoutReset();
        if (i == curPumping) {
          curPumping = -1;
          systemState = WAIT;
          WAITtimer.reset();
          pumpOFF();
        }
        DEBUG("take glass");
        DEBUG(i);
      }
    }

    if (workMode) {
      flowRoutnie();
    } else {
      if (btn.clicked()) {
        systemON = true;
        timeoutReset();
      }
      if (systemON) flowRoutnie();
    }
  }
}

// поиск и заливка - ИСПРАВЛЕННАЯ ВЕРСИЯ
void flowRoutnie() {
  static byte lastPumped = 255; // Последний налитый стакан
  static unsigned long lastPumpTime = 0; // Время последнего наливания
  
  if (systemState == SEARCH) {
    bool noGlass = true;
    
    // Защита от слишком частого переключения (минимум 1 секунда между наливаниями)
    if (millis() - lastPumpTime < 1000) {
      return;
    }

    for (byte i = 0; i < NUM_SHOTS; i++) {
      // Ищем только EMPTY стаканы, которые не были последними
      if (shotStates[i] == EMPTY && i != lastPumped) {  
        noGlass = false;
        parking = false;
        curPumping = i;
        systemState = MOVING;
        shotStates[curPumping] = IN_PROCESS;
        servoON();
        servo.attach();
        
        // Плавное движение с коррекцией
        int targetPos = shotPos[curPumping];
        if (shotPos[curPumping] < servo.getCurrentDeg()) {
          targetPos -= previousShot;
        }
        
        servo.setTargetDeg(constrain(targetPos, 0, 180));
        DEBUG("Found glass: ");
        DEBUG(curPumping);
        break;
      }
    }

    if (noGlass && !parking) {
      servoON();
      servo.setTargetDeg(0);
      if (servo.tick()) {
        servoOFF();
        systemON = false;
        parking = true;
        lastPumped = 255; // Сбрасываем последний налитый
        DEBUG("No glasses");
      }
    }
    
  } else if (systemState == MOVING) {
    if (servo.tick()) {
      systemState = PUMPING;
      FLOWtimer.setInterval((long)thisVolume * time50ml / 50);
      FLOWtimer.reset();
      pumpON();
      strip.setLED(curPumping, mCOLOR(YELLOW));
      strip.show();
      DEBUG("Filling glass ");
      DEBUG(curPumping);
    }
    
  } else if (systemState == PUMPING) {
    if (FLOWtimer.isReady()) {
      pumpOFF();
      shotStates[curPumping] = READY;
      strip.setLED(curPumping, mCOLOR(LIME));
      strip.show();
      lastPumped = curPumping; // Запоминаем последний налитый
      lastPumpTime = millis(); // Запоминаем время
      curPumping = -1;
      systemState = WAIT;
      WAITtimer.reset();
      DEBUG("Filling complete");
    }
    
  } else if (systemState == WAIT) {
    if (WAITtimer.isReady()) {
      systemState = SEARCH;
      timeoutReset();
      DEBUG("Searching...");
    }
  }
}


// отрисовка светодиодов по флагу (100мс)
void LEDtick() {
  if (LEDchanged && LEDtimer.isReady()) {
    LEDchanged = false;
    strip.show();
  }
}

// сброс таймаута
void timeoutReset() {
  if (!timeoutState) disp.brightness(7);
  timeoutState = true;
  TIMEOUTtimer.reset();
  TIMEOUTtimer.start();
  DEBUG("timeout reset");
}

// сам таймаут
void timeoutTick() {
  if (systemState == SEARCH && timeoutState && TIMEOUTtimer.isReady()) {
    DEBUG("timeout");
    timeoutState = false;
    disp.brightness(1);
    POWEROFFtimer.reset();
    jerkServo();
    if (volumeChanged) {
      volumeChanged = false;
      EEPROM.put(0, thisVolume);
    }
  }

  // дёргаем питание серво, это приводит к скачку тока и powerbank не отключает систему
  if (!timeoutState && TIMEOUTtimer.isReady()) {
    if (!POWEROFFtimer.isReady()) {  // пока не сработал таймер полного отключения
      jerkServo();
    } else {
      disp.clear();
    }
  }
}

void jerkServo() {
  if (KEEP_POWER) {
    disp.brightness(7);
    servoON();
    servo.attach();
    servo.write(random(0, 4));
    delay(200);
    servo.detach();
    servoOFF();
    disp.brightness(1);
  }
}
