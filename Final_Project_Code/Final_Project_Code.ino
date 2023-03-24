   /*     
    Simple Interrupt Example 1
    by: Jordan McConnell
    SparkFun Electronics
    created on 10/29/11
    */
    // int ledPin = 13;  // LED is attached to digital pin 13

    // DEBUG ANALOG FORCE READINGS (REMOVE WIRING AND CODE TO THESE LATER)
    int forceDebugPin0 = A0; // Analog Pin 0 that we will use to debug our circuit 
    int forceDebugPin1 = A1; // Analog Pin 1 that we will use to debug our circuit 
    int forceDebugPin2 = A2; // Analog Pin 2 that we will use to debug our circuit 
    int forceDebugPin3 = A3; // Analog Pin 3 that we will use to debug our circuit 
    
    // DEBUG INTERRUPT COUNT 
    int debugInterruptCounter = 0;  // variable to be updated by the interrupt
  
    // int interruptDriverPin = 8; // Pin to 

    // DIGITAL FORCE READING PINS (THIS WILL CONNECT FROM THE "Y" IN OUR QUAD 2-INPUT AND)
    int forcePin0 = 9; // Digital Pin 9
    int forcePin1 = 10; // Digital Pin 10
    int forcePin2 = 11; // Digital Pin 11
    int forcePin3 = 12; // Digital Pin 12

    void setup() {   
      pinMode(forcePin0, INPUT);             
      // pinMode(ledPin, OUTPUT);

      // BASICALLY 4 FORCE SENSOR READINGS... but only 1 interrupt handler should be used.
      // How do we go from 4 digital pins --> 1. 
      // ex: 1 0 1 1 = 1
      // ex: 0 0 0 1 = 1
      attachInterrupt(digitalPinToInterrupt(2), fallingEdge, FALLING); // 1 to 0 
      Serial.begin(9600);  //turn on serial communication
      
      // DEBUG SERIAL STATEMENTS
      Serial.println("");
      Serial.println(debugInterruptCounter, DEC);
    }
    
    void loop() {
      // Write LED to low 
      digitalWrite(ledPin, LOW);

      // Read the analog value
      int forceReading0 = analogRead(forcePin0);
      Serial.print("force reading: "); // Debug statement
      Serial.print(forceReading0); 

      // From analog value reading, write digital value (mapping is subject to change) 
      if(forceReading0 > 0) { 
//        Serial.println("Force reading greater than 0"); 
        digitalWrite(interruptDriverPin, HIGH);
      }
      else {
//        Serial.println("Force reading less than or equal to 0"); 
        digitalWrite(interruptDriverPin, LOW);
      }
      delay(500);
      Serial.print("; increment number: "); // Debug statement
      Serial.println(debugInterruptCounter, DEC); //print x to serial monitor
    }

    // Interrupt service routine for interrupt 0
    void fallingEdge() {
        debugInterruptCounter++;
        digitalWrite(ledPin, HIGH);
    }
