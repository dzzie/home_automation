
#include <Servo.h>

#define maxRockets 12
bool debug = false;
Servo myservo; 

// rc receiver and speed control pins must all be ~PWM capable ports
int rc1_pin = 3; 
int rc2_pin = 6;
int rc5_pin = 5; 
int switch_servo_pin = 9; //air solinoid on as long as user keeps switch flipped now, no default delay (speed/testing hack?)
int drive_speed = 10;  

int steering_A = 7;
int steering_B = 8;

int drive_A = 12 ;
int drive_B = 13 ;

int pins[] = {A5, A4, A3};  //relay pins (6/7 burned out?)
int servoRow[] = {960, 1420, 1940, 2420}; //manually mapped values
int pinCnt = 3; //(sizeof(pins) / sizeof(int))-1;

int trigger = 0;
int lastVal = 0;
int lastRow = 0;

void mosfets_allLow(){ for (int i = 0; i < pinCnt; i++) digitalWrite(pins[i], LOW);}

void initMotor(int pinA, int pinB){
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
}

void setup() {
  
  myservo.attach(switch_servo_pin); 
  myservo.write(servoRow[0]);
  
  pinMode(rc1_pin, INPUT);
  pinMode(rc2_pin, INPUT);
  pinMode(rc5_pin, INPUT);

  pinMode(drive_speed, OUTPUT);
  
  initMotor(steering_A, steering_B);
  initMotor(drive_A, drive_B);
  
  for (int i = 0; i < pinCnt; i++) pinMode(pins[i], OUTPUT);
  mosfets_allLow();
  Serial.begin(9600);
  
}

void setRow(int index){
  
    int curRow = 0;
    
    if(index < 3){ //0,1,2
        myservo.write(servoRow[0]);
        curRow = 0; 
    }
    else if(index < 6){ //3,4,5
        myservo.write(servoRow[1]); 
        curRow = 1;
    }
    else if(index < 9){ //6,7,8
        myservo.write(servoRow[2]); 
        curRow = 2;
    }
    else{ //9,10,11
        myservo.write(servoRow[3]); 
        curRow = 3;
    }
    
    if( curRow != lastRow){
         if(debug){ Serial.print("new row: "); Serial.print(curRow);}
         lastRow = curRow;
         delay(500); //allow servo time for physical move to next position
    }
    
}

void shootRocket(int index){

     int deltaT = 0;
     int col = index % 3; //0 0, 1 1, 2 2, 3 0
     unsigned int trig_time = millis();
     
     digitalWrite(pins[col], HIGH);

     //wait while switch up (try to let user control the air time within reason)
     while(pulseIn(rc5_pin, HIGH) > 1600){;} 

     //switch now low, check timing
     deltaT = millis() - trig_time;
     
     if(deltaT < 80){
          //they just flicked the switch not enough time to open air valve...dont worry ill save you!
          if(debug){ Serial.print("deltaT to short delaying 500ms dt: "); Serial.print(deltaT);}
          delay(500 - deltaT);
     }
     
     digitalWrite(pins[col], LOW);
}

//speed control is optional to save pwm pins (not working anyway with these motors? (capicator?))
void handleMotor(int rcp, int pinA, int pinB, int pwmSpeedPin = 0, int powOverride = 0){

    int power = 0;
    
    if(rcp > 800 && rcp < 1400){
        power = powOverride != 0 ? powOverride : map(rcp, 980, 1400, 255, 50);
        if(power > 255) power = 255;
        if(power < 0) power = 0;
        if(pwmSpeedPin > 0) analogWrite(pwmSpeedPin, power);
        digitalWrite(pinA, HIGH);
        digitalWrite(pinB, LOW);
        if(debug){ Serial.print("Reverse "); Serial.print(power);}
    }else if(rcp >= 1600){
        power = powOverride != 0 ? powOverride : map(rcp, 1600, 2000, 50, 255);
        if(power > 255) power = 255;
        if(power < 0) power = 0;
        if(pwmSpeedPin > 0) analogWrite(pwmSpeedPin, power);
        digitalWrite(pinA, LOW);
        digitalWrite(pinB, HIGH);
        if(debug){Serial.print("Forward "); Serial.print(power);}
    }else{
        digitalWrite(pinA, LOW);
        digitalWrite(pinB, LOW);
        if(debug) Serial.print("Off");
    }

}

void loop() {

  int pw_rc1 = pulseIn(rc1_pin, HIGH);  //truck steering left right
  int pw_rc2 = pulseIn(rc2_pin, HIGH);  //truck throttle forward reverse
  int pw_rc5 = pulseIn(rc5_pin, HIGH);
  
   if(debug){ 
        Serial.print("pw_rc1: "); Serial.print(pw_rc1); 
        Serial.print("   pw_rc2: "); Serial.print(pw_rc2);
        Serial.print("   pw_rc5: "); Serial.print(pw_rc5);
        Serial.print("   steer: ");
   }
   
   handleMotor(pw_rc1, steering_A, steering_B);

   if(debug) Serial.print("   drive: ");
   handleMotor(pw_rc2, drive_A, drive_B, drive_speed);

   if(debug){ Serial.print("\n"); delay(500); }

   if (pw_rc5 > 1530) {
      if (trigger >= maxRockets) {
          if(debug) Serial.println("out of air rockets !");  
          mosfets_allLow();
      } else {
          if(debug){ Serial.print("air rocket launch #"); Serial.println(trigger);}
          shootRocket(trigger);
          trigger++;
          setRow(trigger);
      }
   }

   //delay(10);
}
