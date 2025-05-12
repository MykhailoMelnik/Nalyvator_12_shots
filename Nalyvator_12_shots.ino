// ======== НАСТРОЙКИ ========
#define NUM_SHOTS 12       // количество рюмок (оно же кол-во светодиодов и кнопок!)
#define TIMEOUT_OFF 30     // таймаут на выключение (перестаёт дёргать привод), минут
#define SWITCH_LEVEL 0    // кнопки 1 - высокий сигнал при замыкании, 0 - низкий
#define INVERSE_SERVO 1   // инвертировать направление вращения серво

// положение серво над центрами рюмок
const byte shotPos[] = {29, 40, 61, 89, 90, 100, 110, 129, 140, 150, 160, 170};

// время заполнения 50 мл
const long time50ml = 3000;

// стартова позиція серво (до -100)
const long startServoPoint = 0;

// Значення корекції для повернення назад
const long previousShot = 5;

#define KEEP_POWER 0    // 1 - система поддержания питания ПБ, чтобы он не спал

// отладка
#define DEBUG_UART 1

// Значення для аналогових датчиків
// A0
#define A0_SHOW_VALUE 0  // 1 - показувати значення на екрані, 0 - не показувати
#define A0_BOTH 400    // значення коли обидва натиснуті
#define A0_FIRST 500   // значення для першого стакана
#define A0_SECOND 600  // значення для другого стакана

// A1
#define A1_SHOW_VALUE 0  // 1 - показувати значення на екрані, 0 - не показувати
#define A1_BOTH 400    // значення коли обидва натиснуті
#define A1_FIRST 500   // значення для першого стакана
#define A1_SECOND 600  // значення для другого стакана

// A2
#define A2_SHOW_VALUE 0  // 1 - показувати значення на екрані, 0 - не показувати
#define A2_BOTH 400    // значення коли обидва натиснуті
#define A2_FIRST 500   // значення для першого стакана
#define A2_SECOND 600  // значення для другого стакана

// A3
#define A3_SHOW_VALUE 0  // 1 - показувати значення на екрані, 0 - не показувати
#define A3_BOTH 400    // значення коли обидва натиснуті
#define A3_FIRST 500   // значення для першого стакана
#define A3_SECOND 600  // значення для другого стакана

// A4
#define A4_SHOW_VALUE 0  // 1 - показувати значення на екрані, 0 - не показувати
#define A4_BOTH 400    // значення коли обидва натиснуті
#define A4_FIRST 500   // значення для першого стакана
#define A4_SECOND 600  // значення для другого стакана

// A5
#define A5_SHOW_VALUE 0  // 1 - показувати значення на екрані, 0 - не показувати
#define A5_BOTH 400    // значення коли обидва натиснуті
#define A5_FIRST 500   // значення для першого стакана
#define A5_SECOND 600  // значення для другого стакана

#define SENSOR_TOLERANCE 30  //відхилення від базового значення

// =========== ПИНЫ ===========
#define PUMP_POWER 3
#define SERVO_POWER 4
#define SERVO_PIN 5
#define LED_PIN 6
#define BTN_PIN 7
#define ENC_SW 8
#define ENC_DT 9
#define ENC_CLK 10
#define DISP_DIO 11
#define DISP_CLK 12

const byte SW_pins[] = {A0, A0, A1, A1, A2, A2, A3, A3, A4, A4, A5, A5};

// =========== ЛИБЫ ===========
#include <GyverTM1637.h>
#include <ServoSmooth.h>
#include <microLED.h>
#include <EEPROM.h>
#include "encUniversalMinim.h"
#include "buttonMinim.h"
#include "timer2Minim.h"

// =========== ДАТА ===========
#define COLOR_DEBTH 2   // цветовая глубина: 1, 2, 3 (в байтах)
LEDdata leds[NUM_SHOTS];  // буфер ленты типа LEDdata (размер зависит от COLOR_DEBTH)
microLED strip(leds, NUM_SHOTS, LED_PIN);  // объект лента

GyverTM1637 disp(DISP_CLK, DISP_DIO);

// пин clk, пин dt, пин sw, направление (0/1), тип (0/1)
encMinim enc(ENC_CLK, ENC_DT, ENC_SW, 1, 1);

ServoSmooth servo;

buttonMinim btn(BTN_PIN);
buttonMinim encBtn(ENC_SW);
timerMinim LEDtimer(100);
timerMinim FLOWdebounce(35);
timerMinim FLOWtimer(2000);
timerMinim WAITtimer(1000);
timerMinim TIMEOUTtimer(150000);   // таймаут дёргания приводом
timerMinim POWEROFFtimer(TIMEOUT_OFF * 60000L);

bool LEDchanged = false;
bool pumping = false;
int8_t curPumping = -1;

enum {NO_GLASS, EMPTY, IN_PROCESS, READY} shotStates[NUM_SHOTS];
enum {SEARCH, MOVING, WAIT, PUMPING} systemState;
bool workMode = false;  // 0 manual, 1 auto
int thisVolume = 50;
bool systemON = false;
bool timeoutState = false;
bool volumeChanged = false;
bool parking = false;

// =========== МАКРО ===========
#define servoON() digitalWrite(SERVO_POWER, 1)
#define servoOFF() digitalWrite(SERVO_POWER, 0)
#define pumpON() digitalWrite(PUMP_POWER, 1)
#define pumpOFF() digitalWrite(PUMP_POWER, 0)

#if (DEBUG_UART == 1)
#define DEBUG(x) Serial.println(x)
#else
#define DEBUG(x)
#endif
