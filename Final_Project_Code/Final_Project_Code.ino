// Include libraries
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// LCD VARIABLES
// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
bool repCountSelected = false;
bool startButtonSelected = false;

// GENERAL EXERCISE VARIABLES
int selectedRepCount;

// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1500) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 500)  return btnLEFT; 
 if (adc_key_in < 700)  return btnSELECT;   
 return btnNONE;  // when all others fail, return this...
}

    // DEBUG ANALOG FORCE READINGS (REMOVE WIRING AND CODE TO THESE LATER)
    // int forceDebugPin0 = A0; // Analog Pin 0 that we will use to debug our circuit 
    // int forceDebugPin1 = A1; // Analog Pin 1 that we will use to debug our circuit 
    // int forceDebugPin2 = A2; // Analog Pin 2 that we will use to debug our circuit 
    // int forceDebugPin3 = A3; // Analog Pin 3 that we will use to debug our circuit 

    // Digital force sensor reading, this is the result of an AND gate with A = 1, B = force sensor reading
    int heelSensor = 11; // interrupt event
    int ballSensor = 12;
    int lowerBoxSensor = 10; // interrupt event
    int higherBoxSensor = 13; 

    volatile int heelSensorVal = 0;
    volatile int ballSensorVal = 0;
    volatile int lowerBoxSensorVal = 0;
    volatile int higherBoxSensorVal = 0;

    void setup() {
      Serial.begin(9600);  //turn on serial communication

      pinMode(heelSensor, INPUT); 
      pinMode(ballSensor, INPUT);
      pinMode(lowerBoxSensor, INPUT);
      pinMode(higherBoxSensor, INPUT);
      lcd.begin(16,2);
      lcd.setCursor(0,0);
      lcd.print("Place foot");
      lcd.setCursor(0,1);
      lcd.print("on board!");
      delay(1000); // change longer for demo
      lcd.clear();

      lcd.setCursor(0,0);
      lcd.print("How many ");
      lcd.setCursor(0,1);
      lcd.print("reps?");
      delay(1000); // change longer for demo
      lcd.clear();

      lcd.setCursor(0,0);
      lcd.print("L: 5");
      lcd.setCursor(11,0);
      lcd.print("R: 10");

      // Wait for user to select rep number
      while(repCountSelected == false) {
        lcd_key = read_LCD_buttons();  // read the buttons
        switch (lcd_key)
        {
          case btnLEFT:
          {
            repCountSelected = true;
            selectedRepCount = 5;
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("L selected");
            Serial.println("Left selected, 5 reps");
            break;
          }
          case btnRIGHT:
          {
            repCountSelected = true;
            selectedRepCount = 10;
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("R selected");
            Serial.println("Right selected, 10 reps");
            break;
          }
        } 
      }

      delay(1000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Hit up to");
      lcd.setCursor(0,1);
      lcd.print("start!!");
      
      // Wait for user to select start
      while(startButtonSelected == false) {
        lcd_key = read_LCD_buttons();  // read the buttons
        switch (lcd_key)
        {         
          case btnUP:
          {
            startButtonSelected = true;
            lcd.clear();
            // START TIMER FOR THE WHOLE SESSION UNTIL THEY HAVE COMPLETED REPS
            // TODO:
            break;
          }
        }
      }

      // At the end of the setup loop, start the interrupt (not before though because we don't want to randomly trigger ISR)
      attachInterrupt(digitalPinToInterrupt(2), readFootPosition, CHANGE); // will sense changes from 1 to 0 OR 0 to 1 (required for our application)      
    }
    
    void loop() {
      // Write LED to low 
      // int blue0Val = digitalRead(blue0); // BOX SENSOR
      // int yellow1Val = digitalRead(yellow1);
      // int green2Val = digitalRead(green2);
      // int white3Val = digitalRead(white3); //  HEEL SENSOR

      // Read the analog value
      Serial.print("force reading (heelSensor): "); // Debug statement
      Serial.println(heelSensorVal); 
      // Serial.print("force reading (yellow1): "); // Debug statement
      // Serial.println(yellow1Val); 
      // Serial.print("force reading (green2): "); // Debug statement
      // Serial.println(green2Val); 
      Serial.print("force reading (lowerBoxSensor): "); // Debug statement
      Serial.println(lowerBoxSensorVal); 
      Serial.print("interrupt pin (2): "); // Debug statement
      int interruptPin = 2;
      int statusInterrupt = digitalRead(interruptPin);
      Serial.println(statusInterrupt); 

//       // From analog value reading, write digital value (mapping is subject to change) 
//       if(forceReading0 > 0) { 
// //        Serial.println("Force reading greater than 0"); 
//         digitalWrite(interruptDriverPin, HIGH);
//       }
//       else {
// //        Serial.println("Force reading less than or equal to 0"); 
//         digitalWrite(interruptDriverPin, LOW);
//       }
      // Serial.print("; increment number: "); // Debug statement
      // Serial.println(debugInterruptCounter, DEC); //print x to serial monitor
      delay(500);
    }

    // Interrupt service routine for interrupt 0
    void readFootPosition() {
      // Get a quick reading of all of the foot sensors to get an understanding of the state
      heelSensorVal = digitalRead(heelSensor); // BOX SENSOR
      // yellow1Val = digitalRead(yellow1);
      // green2Val = digitalRead(green2);
      lowerBoxSensorVal = digitalRead(lowerBoxSensor); //  HEEL SENSOR
    }
