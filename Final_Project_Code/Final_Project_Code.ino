////////////////////////////////////////////
// LCD VARIABLES
///////////////////////////////////////////
#include <LiquidCrystal.h>
// 4 data line LCD display
// rs = 8; register select pin controls where in the LCD's memory you are writing to
// en = 9; enable writing to the register to power the LCD
// 4 digital pins

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
bool holdTimeSelected = false;

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
volatile int interruptdebugcounter = 0;
// int forceDebugPin0 = A0; // Analog Pin 0 that we will use to debug our circuit 
// int forceDebugPin1 = A1; // Analog Pin 1 that we will use to debug our circuit 
// int forceDebugPin2 = A2; // Analog Pin 2 that we will use to debug our circuit 
// int forceDebugPin3 = A3; // Analog Pin 3 that we will use to debug our circuit 

////////////////////////////////////////////
// EXERCISE VARIABLES
///////////////////////////////////////////

int selectedRepCount;
unsigned long holdTime;
int repCounter = 0;
bool onTrackForRep = false;

int heelSensor = 11; // interruptable event
int ballSensor = 12;
int lowerBoxSensor = 10; // interruptable event
int higherBoxSensor = 13; 

String lastState = "invalid";

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

////////////////////////////////////////////
// TIMER VARIABLES
///////////////////////////////////////////
unsigned long sessionStartTime = 0;
unsigned long sessionEndTime = 0;
unsigned long sessionOverallTime = 0;

unsigned long repStartTime = 0;
unsigned long repEndTime = 0;
unsigned long repOverallTime = 0;

unsigned long boxStartTime = 0;
unsigned long boxEndTime = 0;
unsigned long boxOverallTime = 0;

    void setup() {
      // Change clock prescaler from 64 to 256
      // Do this by changing the Clock Select Bits 2:0 in the Timer/Counter 0 Control
      // Register B
      // ALSO, use _BV function that is part of the avr-libc library - takes number as argument and converts
      // to appropriate bit mask
      TCCR0B |= _BV(CS02); // OR with 1 to get 1 
      TCCR0B &= ~_BV(CS01); // AND with 0 to get 0 
      TCCR0B &= ~_BV(CS00); // AND with 0 to get 0

      // setup the serial momitor
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
      delay(250); // DEMO: change longer for demo
      lcd.clear();

      // Ask user their desired box hold time
      lcd.setCursor(0,0);
      lcd.print("Desired hold");
      lcd.setCursor(0,1);
      lcd.print("time?");
      delay(250); // DEMO: change longer for demo
      lcd.clear();

      lcd.setCursor(0,0);
      lcd.print("UP: 2s");
      lcd.setCursor(8,0);
      lcd.print("DOWN: 4s");

      // Wait for user to select hold time
      while(holdTimeSelected == false) {
        lcd_key = read_LCD_buttons();  // read the buttons
        switch (lcd_key)
        {
          case btnUP:
          {
            holdTimeSelected = true;
            holdTime = 2000/4; // convert to actual wordly milliseconds
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("2s selected");
            Serial.println("Up selected, 2s");
            break;
          }
          case btnDOWN:
          {
            holdTimeSelected = true;
            holdTime = 4000/4; // convert to actual wordly milliseconds
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("4s selected");
            Serial.println("Down selected, 4s");
            break;
          }
        } 
      }
      // Ask user how many reps they want to complete
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("How many ");
      lcd.setCursor(0,1);
      lcd.print("reps?");
      delay(250); // DEMO: change longer for demo
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

      delay(250); // DEMO
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Hit select");
      lcd.setCursor(0,1);
      lcd.print("to start!!");
      
      // Wait for user to select start
      while(startButtonSelected == false) {
        lcd_key = read_LCD_buttons();  // read the buttons
        switch (lcd_key)
        {         
          case btnSELECT:
          {
            startButtonSelected = true;
            lcd.clear();
            
            // EVENT #1: OVERALL SESSION TIME (START: USER PRESSES UP, END: FLAT FOOT POSITION AFTER SELECTED REP COUNT)
            // start longest timer where more concurrent events will happen within
            sessionStartTime = millis();
            Serial.print("sessionStartTime = ");
            Serial.print(sessionStartTime*4); // multiplied to rescale back to normal time
            Serial.println(" ms");
            delay(1000);
            break;
          }
        }
      }

      // At the end of the setup loop, attach the interrupt (not before though because we don't want to randomly trigger ISR)
      attachInterrupt(digitalPinToInterrupt(interruptPin), interruptHandler, CHANGE); // will sense changes from 1 to 0 OR 0 to 1 (required for our application)      
    }
    
    void loop() {
      Serial.print("force reading (heelSensor): "); // Debug statement
      Serial.println(heelSensorVal); 
      Serial.print("force reading (ballSensorVal): "); // Debug statement
      Serial.println(ballSensorVal); 
      Serial.print("force reading (lowerBoxSensor): "); // Debug statement
      Serial.println(lowerBoxSensorVal); 
      Serial.print("force reading (higherBoxSensor): "); // Debug statement
      Serial.println(higherBoxSensorVal); 
      statusInterrupt = digitalRead(interruptPin);
      Serial.print("Interrupt pin (2): "); // Debug statement
      Serial.println(statusInterrupt); // Debug statement 
      // TODO: add some sort of logic that checks if its the very start of the session, and goes to read the pins using readFootPosition()
      if(heelSensorVal == HIGH) {
        Serial.println("CHAOS DEBUG");
      }
      if(heelSensorVal && ballSensorVal && !lowerBoxSensorVal && !higherBoxSensorVal) { // flat foot
        Serial.println("Flat");
        // If the last rep has been completed (TODO: write logic for that) get the time and print it
        
        if(lastState == "demi" && onTrackForRep) {
          // If the total number of reps have been reached (n-1), end the sessionTimer
          if(repCounter == selectedRepCount - 1) {
            // EVENT #1 END
            sessionEndTime = millis();
            Serial.print("sessionEndTime = ");
            Serial.print(sessionEndTime*4); // multiplied to rescale back to normal time
            Serial.println(" ms");

            // Get overall session time
            sessionOverallTime = sessionEndTime - sessionStartTime; 
            Serial.print("sessionOverallTime = ");
            Serial.print(sessionOverallTime*4); // multiplied to rescale back to normal time
            Serial.println(" ms");            
          }

          repCounter++;
          lcd.clear();
          lcd.setCursor(5,0);
          lcd.write("rep");
          lcd.setCursor(9,0);
          char numString[2];
          sprintf(numString, "%d", repCounter);
          lcd.write(numString);
          onTrackForRep = false;

        } else {
          onTrackForRep = false;
        }

        lastState = "flat";

      } else if (!heelSensorVal && ballSensorVal && !lowerBoxSensorVal && !higherBoxSensorVal) { // demi
        Serial.println("Demi-pointe");

        if(lastState == "flat") {
          onTrackForRep = true;
          // EVENT #2: REP TIME (START: DEMI TO END: FULL HOLD)
          repStartTime = millis();
          Serial.print("repStartTime = ");
          Serial.print(repStartTime*4); // multiplied to rescale back to normal time
          Serial.println(" ms");
        } else if(lastState == ("lower") || ("higher")) {
          onTrackForRep = true;
          // EVENT #3 END
          boxEndTime = millis();
          boxOverallTime = boxEndTime - boxStartTime;
          if((boxOverallTime*4) > holdTime) {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Successful");
            lcd.setCursor(0,1);
            lcd.print("hold!!");
          }
        } else {
          onTrackForRep = false;
        }
        
        lastState = "demi";

      } else if(!heelSensorVal && !ballSensorVal && lowerBoxSensorVal && !higherBoxSensorVal) { // lower box
        if(lastState == "demi") {
          onTrackForRep = true;
        } else if(lastState == "higher") {
          onTrackForRep = true;
        } else if(lastState == "full") {
          // EVENT #2: END
          repEndTime = millis();
          Serial.print("repEndTime = ");
          Serial.print(repEndTime*4); // multiplied to rescale back to normal time
          Serial.println(" ms");

          repOverallTime = repEndTime - repStartTime;
          Serial.print("repOverallTime = ");
          Serial.print(repOverallTime*4); // multiplied to rescale back to normal time
          Serial.println(" ms");
        } else {
          onTrackForRep = false;
        }

        Serial.println("Lower box - almost there!");
        lastState = "lower";

      } else if(!heelSensorVal && !ballSensorVal && lowerBoxSensorVal && higherBoxSensorVal) { // full box
        Serial.println("Full box");

        if(lastState == ("lower") || ("demi")) {
          onTrackForRep = true;
        } else if(lastState == "lower") {
          // EVENT #3 (START: FULL TO END: DEMI)
          boxStartTime = millis();
        } else {
          onTrackForRep = false;
        }

        lastState = "higher";

      } else {
        Serial.println("Invalid Position");
        lastState = "invalid";
      } 
      // Serial.print("interruptdebugCounter!!!");
      // lowerBoxSensorVal = digitalRead(lowerBoxSensor);
      // Serial.println(lowerBoxSensorVal);
      // Serial.println(interruptdebugcounter);
    }

    void interruptHandler() {
      // interruptdebugcounter++;
      readFootPosition();
    }

    void readFootPosition() {
      // Get a quick reading of all of the foot sensors to get an understanding of the state
      heelSensorVal = digitalRead(heelSensor);
      ballSensorVal = digitalRead(ballSensor);
      lowerBoxSensorVal = digitalRead(lowerBoxSensor);
      higherBoxSensorVal = digitalRead(higherBoxSensor);
    }