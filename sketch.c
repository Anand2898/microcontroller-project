#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// =====================================
// PIN DEFINITIONS
// =====================================

const int SIGNAL_LED = 2;      // Yellow LED - GO signal
const int WIN_LED = 3;         // Green LED - Result
const int BUTTON_PIN = 4;      // Push button
const int RED_LED = LED_BUILTIN;

// Performance bar LEDs
const int BAR_LED_1 = 5;
const int BAR_LED_2 = 6;
const int BAR_LED_3 = 7;
const int BAR_LED_4 = 8;
const int BAR_LED_5 = 9;

// =====================================
// LCD
// =====================================

LiquidCrystal_I2C lcd(0x27, 16, 2);

// =====================================
// GAME STATES
// =====================================

enum GameState {
 WAITING,
 COUNTDOWN,
 GO,
 RESULT,
 FALSE_START
};

GameState gameState = WAITING;

// =====================================
// VARIABLES
// =====================================

unsigned long startTime = 0;
unsigned long reactionTime = 0;

bool lastButtonState = HIGH;

// =====================================
// BUTTON FUNCTION
// =====================================

bool buttonPressed() {

 bool currentState = digitalRead(BUTTON_PIN);

 if (lastButtonState == HIGH && currentState == LOW) {

   delay(20);

   lastButtonState = currentState;

   return true;
 }

 lastButtonState = currentState;

 return false;
}

// =====================================
// READY SCREEN
// =====================================

void showReady() {

 lcd.clear();

 lcd.setCursor(0, 0);
 lcd.print("Reaction Game!");

 lcd.setCursor(0, 1);
 lcd.print("Press Button");
}

// =====================================
// COUNTDOWN
// =====================================

void showCountdown(int number) {

 lcd.clear();

 lcd.setCursor(0, 0);
 lcd.print("Get Ready...");

 lcd.setCursor(7, 1);
 lcd.print(number);
}

// =====================================
// GO SCREEN
// =====================================

void showGo() {

 lcd.clear();

 lcd.setCursor(5, 0);
 lcd.print("GO!!!");

 lcd.setCursor(0, 1);
 lcd.print("PRESS NOW!");
}

// =====================================
// SCORE CALCULATION
// =====================================

String getScore(unsigned long time) {

 if (time < 200) {
   return "EXCELLENT";
 }
 else if (time < 300) {
   return "GOOD";
 }
 else if (time < 450) {
   return "MEDIOCRE";
 }
 else if (time < 600) {
   return "POOR";
 }
 else {
   return "VERY POOR";
 }
}

// =====================================
// TURN OFF ALL LEDs
// =====================================

void allLEDsOff() {

 digitalWrite(SIGNAL_LED, LOW);
 digitalWrite(WIN_LED, LOW);
 digitalWrite(RED_LED, LOW);

 digitalWrite(BAR_LED_1, LOW);
 digitalWrite(BAR_LED_2, LOW);
 digitalWrite(BAR_LED_3, LOW);
 digitalWrite(BAR_LED_4, LOW);
 digitalWrite(BAR_LED_5, LOW);
}

// =====================================
// RESULT LED
// =====================================

void showResultLED(unsigned long time) {

 // Yellow GO LED OFF
 digitalWrite(SIGNAL_LED, LOW);

 // Green LED ON when result is displayed
 digitalWrite(WIN_LED, HIGH);

 // Red LED OFF
 digitalWrite(RED_LED, LOW);
}

// =====================================
// PERFORMANCE BAR
// =====================================

void showPerformanceBar(unsigned long time) {

 // Turn OFF all bar LEDs first

 digitalWrite(BAR_LED_1, LOW);
 digitalWrite(BAR_LED_2, LOW);
 digitalWrite(BAR_LED_3, LOW);
 digitalWrite(BAR_LED_4, LOW);
 digitalWrite(BAR_LED_5, LOW);

 // ===================================
 // EXCELLENT: < 200 ms
 // 5 LEDs
 // ===================================

 if (time < 200) {

   digitalWrite(BAR_LED_1, HIGH);
   digitalWrite(BAR_LED_2, HIGH);
   digitalWrite(BAR_LED_3, HIGH);
   digitalWrite(BAR_LED_4, HIGH);
   digitalWrite(BAR_LED_5, HIGH);
 }

 // ===================================
 // GOOD: 200 - 299 ms
 // 4 LEDs
 // ===================================

 else if (time < 300) {

   digitalWrite(BAR_LED_1, HIGH);
   digitalWrite(BAR_LED_2, HIGH);
   digitalWrite(BAR_LED_3, HIGH);
   digitalWrite(BAR_LED_4, HIGH);
 }

 // ===================================
 // MEDIOCRE: 300 - 449 ms
 // 3 LEDs
 // ===================================

 else if (time < 450) {

   digitalWrite(BAR_LED_1, HIGH);
   digitalWrite(BAR_LED_2, HIGH);
   digitalWrite(BAR_LED_3, HIGH);
 }

 // ===================================
 // POOR: 450 - 599 ms
 // 2 LEDs
 // ===================================

 else if (time < 600) {

   digitalWrite(BAR_LED_1, HIGH);
   digitalWrite(BAR_LED_2, HIGH);
 }

 // ===================================
 // VERY POOR: >= 600 ms
 // 1 LED
 // ===================================

 else {

   digitalWrite(BAR_LED_1, HIGH);
 }
}

// =====================================
// SHOW RESULT
// =====================================

void showResult() {

 String score = getScore(reactionTime);

 lcd.clear();

 lcd.setCursor(0, 0);
 lcd.print("Time: ");
 lcd.print(reactionTime);
 lcd.print(" ms");

 lcd.setCursor(0, 1);
 lcd.print(score);
}

// =====================================
// SETUP
// =====================================

void setup() {

 // Main LEDs
 pinMode(SIGNAL_LED, OUTPUT);
 pinMode(WIN_LED, OUTPUT);
 pinMode(RED_LED, OUTPUT);

 // Push button
 pinMode(BUTTON_PIN, INPUT_PULLUP);

 // Performance bar LEDs
 pinMode(BAR_LED_1, OUTPUT);
 pinMode(BAR_LED_2, OUTPUT);
 pinMode(BAR_LED_3, OUTPUT);
 pinMode(BAR_LED_4, OUTPUT);
 pinMode(BAR_LED_5, OUTPUT);

 // Turn everything OFF
 allLEDsOff();

 // Initialize I2C
 Wire.begin();

 // Initialize LCD
 lcd.init();
 lcd.backlight();

 // Show starting screen
 showReady();
}

// =====================================
// MAIN LOOP
// =====================================

void loop() {

 // ===================================
 // WAITING FOR BUTTON
 // ===================================

 if (gameState == WAITING) {

   if (buttonPressed()) {

     allLEDsOff();

     gameState = COUNTDOWN;

     // ================================
     // COUNTDOWN
     // ================================

     showCountdown(3);
     delay(1000);

     showCountdown(2);
     delay(1000);

     showCountdown(1);
     delay(1000);

     // ================================
     // GO
     // ================================

     showGo();

     // Yellow LED stays ON during GO
     digitalWrite(SIGNAL_LED, HIGH);

     // Start reaction timer
     startTime = millis();

     gameState = GO;
   }
 }

 // ===================================
 // WAITING FOR REACTION
 // ===================================

 else if (gameState == GO) {

   if (buttonPressed()) {

     // Calculate reaction time
     reactionTime = millis() - startTime;

     // Turn OFF yellow GO LED
     digitalWrite(SIGNAL_LED, LOW);

     // Show result on LCD
     showResult();

     // Turn ON green result LED
     showResultLED(reactionTime);

     // Show performance bar
     showPerformanceBar(reactionTime);

     gameState = RESULT;
   }
 }

 // ===================================
 // RESULT DISPLAY
 // ===================================

 else if (gameState == RESULT) {

   // Keep result displayed for 3 seconds
   delay(3000);

   // Turn OFF all LEDs
   allLEDsOff();

   // Return to ready screen
   showReady();

   gameState = WAITING;
 }

 // ===================================
 // FALSE START
 // ===================================

 else if (gameState == FALSE_START) {

   allLEDsOff();

   digitalWrite(RED_LED, HIGH);

   lcd.clear();

   lcd.setCursor(0, 0);
   lcd.print("TOO EARLY!");

   lcd.setCursor(0, 1);
   lcd.print("False Start!");

   delay(2500);

   digitalWrite(RED_LED, LOW);

   showReady();

   gameState = WAITING;
 }
}

