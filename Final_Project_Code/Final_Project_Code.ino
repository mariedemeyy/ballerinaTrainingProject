////////////////////////////////////////////
// LCD VARIABLES
///////////////////////////////////////////
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

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

////////////////////////////////////////////
// DEBUG VARIABLES (REMOVE LATER)
///////////////////////////////////////////

// int forceDebugPin0 = A0; // Analog Pin 0 that we will use to debug our circuit 
// int forceDebugPin1 = A1; // Analog Pin 1 that we will use to debug our circuit 
// int forceDebugPin2 = A2; // Analog Pin 2 that we will use to debug our circuit 
// int forceDebugPin3 = A3; // Analog Pin 3 that we will use to debug our circuit 

////////////////////////////////////////////
// EXERCISE VARIABLES
///////////////////////////////////////////

int selectedRepCount;

int heelSensor = 11; // interruptable event
int ballSensor = 12;
int lowerBoxSensor = 10; // interruptable event
int higherBoxSensor = 13; 

// These variables are modified in the ISR, declare as volatile to make sure they are updated correctly
volatile int heelSensorVal = 0;
volatile int ballSensorVal = 0;
volatile int lowerBoxSensorVal = 0;
volatile int higherBoxSensorVal = 0;

////////////////////////////////////////////
// ISR VARIABLES
///////////////////////////////////////////
int interruptPin = 2;
int statusInterrupt = 0;

    void setup() {
      Serial.begin(9600);

      pinMode(heelSensor, INPUT); 
      pinMode(ballSensor, INPUT);
      pinMode(lowerBoxSensor, INPUT);
      pinMode(higherBoxSensor, INPUT);

      // LCD display code to guide user through their exercises
      lcd.begin(16,2);
      lcd.setCursor(0,0);
      lcd.print("Place foot");
      lcd.setCursor(0,1);
      lcd.print("on board!");
      delay(1000); // DEMO: change longer for demo
      lcd.clear();

      lcd.setCursor(0,0);
      lcd.print("How many ");
      lcd.setCursor(0,1);
      lcd.print("reps?");
      delay(1000); // DEMO: change longer for demo
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

      delay(1000); // DEMO
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

      // At the end of the setup loop, attach the interrupt (not before though because we don't want to randomly trigger ISR)
      attachInterrupt(interruptPin, interruptHandler, CHANGE); // will sense changes from 1 to 0 OR 0 to 1 (required for our application)      
    }
    
    void loop() {
      Serial.print("force reading (heelSensor): "); // Debug statement
      Serial.println(heelSensorVal); 
      Serial.print("force reading (lowerBoxSensor): "); // Debug statement
      Serial.println(lowerBoxSensorVal); 

      statusInterrupt = digitalRead(interruptPin);
      Serial.print("Interrupt pin (2): "); // Debug statement
      Serial.println(statusInterrupt); // Debug statement 
      // TODO: add some sort of logic that checks if its the very start of the session, and goes to read the pins using readFootPosition()
  
      if(heelSensorVal && ballSensorVal && !lowerBoxSensorVal && !higherBoxSensorVal) { // flat foot
        Serial.println("Flat");

      } else if (!heelSensorVal && ballSensorVal && !lowerBoxSensorVal && !higherBoxSensorVal) { // demi
        Serial.println("Demi-pointe");

      } else if(!heelSensorVal && !ballSensorVal && lowerBoxSensorVal && !higherBoxSensorVal) { // lower box
        Serial.println("Lower box - almost there!");

      } else if(!heelSensorVal && !ballSensorVal && lowerBoxSensorVal && higherBoxSensorVal) { // full box
        Serial.println("Full box");

      } else {
        Serial.println("Keep Trying :)");

      } 
    }
    
    void interruptHandler() {
      readFootPosition();
    }

    void readFootPosition() {
      // Get a quick reading of all of the foot sensors to get an understanding of the state
      heelSensorVal = digitalRead(heelSensor);
      ballSensorVal = digitalRead(ballSensor);
      lowerBoxSensorVal = digitalRead(lowerBoxSensor);
      higherBoxSensorVal = digitalRead(higherBoxSensor);
    }

    void 
