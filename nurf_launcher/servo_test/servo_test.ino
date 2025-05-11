/*
    servo on pin 9
    read servo pos from serial and output to servo for testing
*/

#include <Servo.h>

#define firmware_ver  "v0.1 " __DATE__ 

int incPin = 8;
//int potPin = A0;  // analog pin used to connect the potentiometer
int tranPin = 11;  //Transistor base pin
                   //thanks Gary! - http://www.modsbyus.com/how-to-properly-detachturn-off-a-servo-with-arduino/

Servo myservo; 
int lastVal = 0;
int val = 0;

void setup()   {   
  
  Serial.begin(9600);
  
  pinMode(incPin, INPUT_PULLUP);
  //pinMode(potPin, INPUT);
  //pinMode(tranPin, OUTPUT); 
  
  myservo.attach(9); 

   
  //myservo.write( servoPos() );   //set servo startup position..wont jump if still at last pos set on pot on power down..
  //powerDevices(true);            //now turn on the servo and dht22
  
}

/*
void powerDevices(bool on){
  digitalWrite(tranPin, on ? HIGH : LOW); 
  powerOn  = on;
}

int servoPos(){
      int val = analogRead(potPin);        // reads the value of the potentiometer (value between 0 and 1023) 
      return map(val, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180) 
}
*/

int readPos(){
  
      int v1 = 0;
             
      if (Serial.available()){       
          v1 = Serial.parseInt();
          while( Serial.available() ) Serial.read(); //clear any remaining chars from buffer...
      }

      return v1;
}

void loop() {

   int val = readPos();

   /*if(val == 0){
      val = 500;
      myservo.write(val); 
   }
   //this way we can control it at the device and let it slowly increment while we monitor without having to hook up a pot
   if(digitalRead(incPin) == LOW ) val += 20;

  if(val > 2600){
      Serial.print("complete\n"); 
      exit(0);
  }*/

   if(val > 0 &&  (abs(val - lastVal) > 2)){ //pot changed refresh display..
       lastVal = val;
       //if(!powerOn) powerDevices(true);
       Serial.print("Pos: "); Serial.println(val);
       myservo.write(val); 
       delay(300);
   }
   

   delay(10);

}
