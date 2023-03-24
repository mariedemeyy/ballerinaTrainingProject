   /*     
    Simple Interrupt Example 1
    by: Jordan McConnell
    SparkFun Electronics
    created on 10/29/11
    */
    int ledPin = 13;  // LED is attached to digital pin 13

    // Force sensor code
    int forcePin0 = A0; // Analog Pin 0 
    int interruptDriverPin = 8; // Pin to 
    int x = 0;  // variable to be updated by the interrupt
    
    void setup() {                
      //enable interrupt 0 (pin 2) which is connected to a button
      //jump to the increment function on falling edge
      pinMode(ledPin, OUTPUT);
      pinMode(interruptDriverPin, OUTPUT);
      attachInterrupt(digitalPinToInterrupt(2), fallingEdge, FALLING);
      Serial.begin(9600);  //turn on serial communication
      Serial.println("");
      Serial.println(x, DEC);
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
      Serial.println(x, DEC); //print x to serial monitor
    }

    // Interrupt service routine for interrupt 0
    void fallingEdge() {
        x++;
        digitalWrite(ledPin, HIGH);
    }
