/*
  MQUnifiedsensor Library - reading an MQ3

  Demonstrates the use a MQ3 sensor.
  Library originally added 01 may 2019
  by Miguel A Califa, Yersson Carrillo, Ghiordy Contreras, Mario Rodriguez
 
  Added example
  modified 23 May 2019
  by Miguel Califa 

  Updated library usage
  modified 26 March 2020
  by Miguel Califa 

  Wiring:
  https://github.com/miguel5612/MQSensorsLib_Docs/blob/master/static/img/MQ_Arduino.PNG
  Please make sure arduino A0 pin represents the analog input configured on #define pin

 This example code is in the public domain.

*/

//Include the library
#include <MQUnifiedsensor.h>
/************************Hardware Related Macros************************************/
#define         Board                   ("Arduino UNO")
#define         Pin                     (A3)  //Analog input 3 of your arduino
/***********************Software Related Macros************************************/
#define         Type                    ("MQ-3") //MQ3
#define         Voltage_Resolution      (5)
#define         ADC_Bit_Resolution      (10) // For arduino UNO/MEGA/NANO
#define         RatioMQ3CleanAir        (60) //RS / R0 = 60 ppm 
/*****************************Globals***********************************************/
//Declare Sensor
MQUnifiedsensor MQ3(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin, Type);

void setup() {
  //Init the serial port communication - to debug the library
  Serial.begin(9600); //Init serial port

  //Set math model to calculate the PPM concentration and the value of constants
  MQ3.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ3.setA(4.8387); MQ3.setB(-2.68); // Configure the equation to to calculate Benzene concentration
  /*
    Exponential regression:
  Gas    | a      | b
  LPG    | 44771  | -3.245
  CH4    | 2*10^31| 19.01
  CO     | 521853 | -3.821
  Alcohol| 0.3934 | -1.504
  Benzene| 4.8387 | -2.68
  Hexane | 7585.3 | -2.849
  */

  /*****************************  MQ Init ********************************************/ 
  //Remarks: Configure the pin of arduino as input.
  /************************************************************************************/ 
  MQ3.init(); 
  
  /* 
    //If the RL value is different from 10K please assign your RL value with the following method:
    MQ3.setRL(10);
  */
  /*****************************  MQ CAlibration ********************************************/ 
  // Explanation: 
   // In this routine the sensor will measure the resistance of the sensor supposedly before being pre-heated
  // and on clean air (Calibration conditions), setting up R0 value.
  // We recomend executing this routine only on setup in laboratory conditions.
  // This routine does not need to be executed on each restart, you can load your R0 value from eeprom.
  // Acknowledgements: https://jayconsystems.com/blog/understanding-a-gas-sensor
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ3.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ3.calibrate(RatioMQ3CleanAir);
    Serial.print(".");
  }
  MQ3.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  /*****************************  MQ CAlibration ********************************************/ 
  MQ3.serialDebug(true);
}

void loop() {
  MQ3.update(); // Update data, the arduino will read the voltage from the analog pin
  MQ3.readSensor(); // Sensor will read PPM concentration using the model, a and b values set previously or from the setup
  MQ3.serialDebug(); // Will print the table on the serial port
  delay(500); //Sampling frequency
}


// #define MQ3pin 0

// float sensorValue;  //variable to store sensor value

// void setup() {
// 	Serial.begin(9600); // sets the serial port to 9600
// 	Serial.println("MQ3 warming up!");
// 	delay(20000); // allow the MQ3 to warm up
// }

// void loop() {
// 	sensorValue = analogRead(MQ3pin); // read analog input pin 0

// 	Serial.print("Sensor Value: ");
// 	Serial.println(sensorValue);
	
// 	delay(2000); // wait 2s for next reading
// }



// #include <LiquidCrystal.h>

// LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // Initialize the LCD library

// const int mq3Pin = A0; // Analog pin for MQ3 sensor
// const int threshold = 100; // Set your threshold value here
// const int blowDuration = 5; // Duration for blowing in seconds
// const int delayBeforeResult = 5; // Delay before displaying the result in seconds
// const int resultHoldDuration = 20; // Duration to hold the result on the screen in seconds
// const int preparingDuration = 10; // Duration for preparing in seconds

// void setup() {
//   lcd.begin(16, 2); // Initialize the LCD screen
//   pinMode(mq3Pin, INPUT); // Set MQ3 pin as input
//   Serial.begin(9600); // Initialize serial communication for debugging
// }

// void loop() {
//   // Preparing stage with progress bar
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Preparing...");
//   for (int i = 0; i < preparingDuration; i++) {
//     lcd.write(219); // Character for filling the progress bar
//     delay(preparingDuration * 1000 / 16); // 16 characters in total, evenly distributed across the duration
//   }

//   // Blowing stage with progress bar
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Blow for 5 sec");
  
//   // Wait until alcohol level rises indicating user blowing
//   unsigned long startTime = millis();
//   unsigned long elapsedTime = 0;
//   int initialSensorValue = analogRead(mq3Pin);
//   while (elapsedTime < 5000) {
//     int currentSensorValue = analogRead(mq3Pin);
//     if (abs(currentSensorValue - initialSensorValue) > threshold) {
//       elapsedTime = millis() - startTime;
//     }
//   }
  
//   // Display progress bar
//   lcd.setCursor(0, 1);
//   for (int i = 0; i < blowDuration; i++) {
//     lcd.write(219); // Character for filling the progress bar
//     delay(1000);
//   }
  
//   int alcoholLevel = measureAlcoholLevel();
  
//   // Display result
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Wait for result");
//   delay(delayBeforeResult * 1000); // Delay before displaying the result
  
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Alcohol Level:");
//   lcd.setCursor(0, 1);
//   lcd.print(alcoholLevel);
//   lcd.print("%");

//   delay(resultHoldDuration * 1000); // Hold the result for 20 seconds
// }

// int measureAlcoholLevel() {
//   // Measure alcohol level using MQ3 sensor
//   int sensorValue = analogRead(mq3Pin);
//   int alcoholLevel = map(sensorValue, 0, 1023, 0, 100); // Map the sensor value to percentage

//   return alcoholLevel;
// }
