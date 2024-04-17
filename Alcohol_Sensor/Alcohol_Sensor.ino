#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // Initialize the LCD library

const int mq3Pin = A0; // Analog pin for MQ3 sensor
const int threshold = 100; // Set your threshold value here
const int blowDuration = 5; // Duration for blowing in seconds
const int delayBeforeResult = 5; // Delay before displaying the result in seconds
const int resultHoldDuration = 20; // Duration to hold the result on the screen in seconds
const int preparingDuration = 10; // Duration for preparing in seconds

const int readyLED = 8; // Pin for the LED indicating device ready
const int blowingLED = 9; // Pin for the LED indicating user blowing

void setup() {
  lcd.begin(16, 2); // Initialize the LCD screen
  pinMode(mq3Pin, INPUT); // Set MQ3 pin as input
  pinMode(readyLED, OUTPUT); // Set ready LED pin as output
  pinMode(blowingLED, OUTPUT); // Set blowing LED pin as output
  Serial.begin(9600); // Initialize serial communication for debugging
}

void loop() {
  // Preparing stage with progress bar
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Preparing...");
  
  // Turn on the ready LED
  digitalWrite(readyLED, HIGH);
  digitalWrite(blowingLED, LOW);
  
  for (int i = 0; i < preparingDuration; i++) {
    lcd.write(219); // Character for filling the progress bar
    delay(preparingDuration * 1000 / 16); // 16 characters in total, evenly distributed across the duration
  }

  // Blowing stage with progress bar
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Blow for 5 sec");
  
  // Turn off the ready LED and turn on the blowing LED
  digitalWrite(readyLED, LOW);
  digitalWrite(blowingLED, HIGH);
  
  // Wait until alcohol level rises indicating user blowing
  unsigned long startTime = millis();
  unsigned long elapsedTime = 0;
  int initialSensorValue = analogRead(mq3Pin);
  while (elapsedTime < 5000) {
    int currentSensorValue = analogRead(mq3Pin);
    if (abs(currentSensorValue - initialSensorValue) > threshold) {
      elapsedTime = millis() - startTime;
    }
  }
  
  // Display progress bar
  lcd.setCursor(0, 1);
  for (int i = 0; i < blowDuration; i++) {
    lcd.write(219); // Character for filling the progress bar
    delay(1000);
  }
  
  int alcoholLevel = measureAlcoholLevel();
  
  // Ensure alcohol level is never less than 0
  if (alcoholLevel < 0) {
    alcoholLevel = 0;
  }
  
  // Display result
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wait for result");
  delay(delayBeforeResult * 1000); // Delay before displaying the result
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alcohol Level:");
  lcd.setCursor(0, 1);
  lcd.print(alcoholLevel);
  lcd.print("%");

  delay(resultHoldDuration * 1000); // Hold the result for 20 seconds
}

int measureAlcoholLevel() {
  // Measure alcohol level using MQ3 sensor
  int sensorValue = analogRead(mq3Pin);
  int alcoholLevel = map(sensorValue, 0, 1023, 0, 100); // Map the sensor value to percentage

  return alcoholLevel;
}
