#include <LiquidCrystal.h>
#include "thingProperties.h"

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // Initialize the LCD library

const int mq3Pin = A0; // Analog pin for MQ3 sensor
const int threshold = 100; // Set your threshold value here
const int blowDuration = 5; // Duration for blowing in seconds
const int delayBeforeResult = 5; // Delay before displaying the result in seconds
const int resultHoldDuration = 20; // Duration to hold the result on the screen in seconds
const int preparingDuration = 10; // Duration for preparing in seconds

const int readyLED = 8; // Pin for the LED indicating device ready
const int blowingLED = 9; // Pin for the LED indicating user blowing
const int funDriveLED = 7; // Pin for the LED indicating fun drive time
const int lameDriveLED = 10; // Pin for the LED indicating lame drive time

void setup() {
  lcd.begin(16, 2); // Initialize the LCD screen
  pinMode(mq3Pin, INPUT); // Set MQ3 pin as input
  pinMode(readyLED, OUTPUT); // Set ready LED pin as output
  pinMode(blowingLED, OUTPUT); // Set blowing LED pin as output
  pinMode(funDriveLED, OUTPUT); // Set fun drive LED pin as output
  pinMode(lameDriveLED, OUTPUT); // Set lame drive LED pin as output
  Serial.begin(9600); // Initialize serial communication for debugging

  delay(1500);
  initProperties();
  //Connect to cloud and get info/errors
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
  //Wait for cloud connection
  while (ArduinoCloud.connected() != 1) {
    ArduinoCloud.update();
    delay(500);
  }
}

void loop() {
  // Preparing stage with progress bar
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Preparing...");
  
  for (int i = 0; i < preparingDuration; i++) {
    lcd.write(219); // Character for filling the progress bar
    delay(preparingDuration * 1000 / 16); // 16 characters in total, evenly distributed across the duration
  }

  // Turn on the ready LED after the preparation phase
  digitalWrite(readyLED, HIGH);
  
  // Blowing stage with progress bar
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Blow for 5 sec");
  
  // Wait until alcohol level rises indicating user blowing
  unsigned long startTime = millis();
  unsigned long elapsedTime = 0;
  int initialSensorValue = analogRead(mq3Pin);
  while (elapsedTime < 5000 && elapsedTime == 0) { // Keep the ready LED on until the user starts blowing
    int currentSensorValue = analogRead(mq3Pin);
    if (abs(currentSensorValue - initialSensorValue) > threshold) {
      elapsedTime = millis() - startTime;
    }
  }
  
  // Turn off the ready LED and turn on the blowing LED
  digitalWrite(readyLED, LOW);
  digitalWrite(blowingLED, HIGH);
  
  // Display progress bar
  lcd.setCursor(0, 1);
  for (int i = 0; i < blowDuration; i++) {
    lcd.write(219); // Character for filling the progress bar
    delay(1000);
  }
  
  // Turn off the blowing LED at the end of blowing phase
  digitalWrite(blowingLED, LOW);
  
  float alcoholLevel = measureAlcoholLevel();
  
  // Ensure alcohol level is never less than 0
  if (alcoholLevel < 0) {
    alcoholLevel = 0;
  }
  
  // Display result
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wait for result");
  delay(delayBeforeResult * 1000); // Delay before displaying the result
  
  ArduinoCloud.update();
  Serial.println(alcoholLevel);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BAC Level:");
  lcd.print(alcoholLevel);
  lcd.print("%");

  // Check if alcohol level is greater than or equal to 8% and turn on fun drive LED
  if (alcoholLevel >= 8) {
    digitalWrite(funDriveLED, HIGH);
    lcd.setCursor(0, 1);
    lcd.print(" Fun Drive Time!");
  }
  
  // Check if alcohol level is less than 8% and turn on lame drive LED
  if (alcoholLevel < 8) {
    digitalWrite(lameDriveLED, HIGH);
    lcd.setCursor(0, 1);
    lcd.print(" Lame Drive Time!");
  }
  
  delay(resultHoldDuration * 1000); // Hold the result for 20 seconds
  
  // Turn off the fun drive LED after the result hold duration
  digitalWrite(funDriveLED, LOW);
  // Turn off the lame drive LED after the result hold duration
  digitalWrite(lameDriveLED, LOW);
}

int measureAlcoholLevel() {
  // Measure alcohol level using MQ3 sensor
  int sensorValue = analogRead(mq3Pin);
  float alcoholLevel = map(sensorValue, 200, 3500, 0, 1); // Map the sensor value to percentage with linear scale such that 400 is the legal threshold
  // linear mapping chosen because MQSensorsLib library uses this model

  return alcoholLevel;
}
