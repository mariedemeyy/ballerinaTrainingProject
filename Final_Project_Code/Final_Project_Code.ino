   /*     
    Simple Interrupt Example 1
    by: Jordan McConnell
    SparkFun Electronics
    created on 10/29/11
    */
    // int ledPin = 13;  // LED is attached to digital pin 13

    // DEBUG ANALOG FORCE READINGS (REMOVE WIRING AND CODE TO THESE LATER)
    // int forceDebugPin0 = A0; // Analog Pin 0 that we will use to debug our circuit 
    // int forceDebugPin1 = A1; // Analog Pin 1 that we will use to debug our circuit 
    // int forceDebugPin2 = A2; // Analog Pin 2 that we will use to debug our circuit 
    // int forceDebugPin3 = A3; // Analog Pin 3 that we will use to debug our circuit 
    
    // DEBUG INTERRUPT COUNT 
    volatile int debugInterruptCounter = 0;  // variable to be updated by the interrupt
  
    // int interruptDriverPin = 8; // Pin to 

    // DIGITAL FORCE READING PINS (THIS WILL CONNECT FROM THE "Y" IN OUR QUAD 2-INPUT AND)
    int blue0 = 9; // Digital Pin 9 for 0
    int yellow1 = 10; // Digital Pin 10 for 1
    int green2 = 11; // Digital Pin 11 for 2
    int white3 = 12; // Digital Pin 12 for 3

    volatile int blue0Val = 0; // BOX SENSOR
    volatile int yellow1Val = 0;
    volatile int green2Val = 0;
    volatile int white3Val = 0; //  HEEL SENSOR

    void setup() {   
      pinMode(blue0, INPUT); 
      pinMode(yellow1, INPUT);
      pinMode(green2, INPUT);
      pinMode(white3, INPUT);            
      // pinMode(ledPin, OUTPUT);

      // OLD IDEA:
      // BASICALLY 4 FORCE SENSOR READINGS... but only 1 interrupt handler should be used.
      // How do we go from 4 digital pins --> 1. 
      // ex: 1 0 1 1 = 1
      // ex: 0 0 0 1 = 1
      // Proved to be too difficult because of having more than 1 "button press" at a time, and not being able to detect new
      // "press" (or technically release in our project) when other buttons are "pressed"

      // NEW IDEA: 
      // 1 sensor for the heel, 1 sensor for the platform = combine these with an OR gate to detect which of the two sensors are on
      // Reason for these sensor locations is to mimic only button being pressed at a time (ie: if you are doing movement correct and as
      // the device intends, you start on heel (platform sensor off), progress to demi point (both sensors off), then final state where
      // the platform sensor is on (heel sensor off))

      
      attachInterrupt(digitalPinToInterrupt(2), readFootPosition, CHANGE); // will sense changes from 1 to 0 OR 0 to 1 (required for our application)
      Serial.begin(9600);  //turn on serial communication
      
      // DEBUG SERIAL STATEMENTS
      Serial.println("");
      Serial.println(debugInterruptCounter, DEC);
    }
    
    void loop() {
      // Write LED to low 
      // int blue0Val = digitalRead(blue0); // BOX SENSOR
      // int yellow1Val = digitalRead(yellow1);
      // int green2Val = digitalRead(green2);
      // int white3Val = digitalRead(white3); //  HEEL SENSOR

      // Read the analog value
      Serial.print("force reading (blue0): "); // Debug statement
      Serial.println(blue0Val); 
      Serial.print("force reading (yellow1): "); // Debug statement
      Serial.println(yellow1Val); 
      Serial.print("force reading (green2): "); // Debug statement
      Serial.println(green2Val); 
      Serial.print("force reading (white3): "); // Debug statement
      Serial.println(white3Val); 
      Serial.print("interrupt pin (2)"); // Debug statement
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
      Serial.print("debugInterrupCounter: "); // Debug statement
      Serial.println(debugInterruptCounter, DEC); //print x to serial monitor
      delay(1000);
      // Serial.print("; increment number: "); // Debug statement
      // Serial.println(debugInterruptCounter, DEC); //print x to serial monitor
    }

    // Interrupt service routine for interrupt 0
    void readFootPosition() {
      // Remove debug counter later
      debugInterruptCounter++;

      // Get a quick reading of all of the foot sensors to get an understanding of the state
      blue0Val = digitalRead(blue0); // BOX SENSOR
      yellow1Val = digitalRead(yellow1);
      green2Val = digitalRead(green2);
      white3Val = digitalRead(white3); //  HEEL SENSOR
    }
