
int rc_pin = 3; // rc receiver on ~PWM capable port
int pins[] = {2, 5, 6, 7, 8};
int pinCnt = (sizeof(pins) / sizeof(int))-1;
int trigger = 0;
bool warned = false;
int lastVal = 0;

void allLow(){ for (int i = 0; i <= pinCnt; i++) digitalWrite(pins[i], LOW);}

void setup() {
  pinMode(rc_pin, INPUT);
  for (int i = 0; i <= pinCnt; i++) pinMode(pins[i], OUTPUT);
  allLow();
  Serial.begin(9600);
}

void loop() {

  int rc5 = pulseIn(rc_pin, HIGH, 25000);

  if( abs(lastVal - rc5) > 40){ //anti spam
      //Serial.print("rc5: "); Serial.print (rc5);
      lastVal = rc5 ;
  }
 
  if (rc5 == 0) {
      if(!warned){
          Serial.println("no signal waiting...");
          allLow();
          warned = true;
      }
  }
  else if (rc5 > 1530) {
      warned = false;
      if (trigger > pinCnt) {
          Serial.println("out of air rockets !");
          allLow();
          exit(0);
      } else {
          Serial.print("air rocket launch #"); Serial.println(trigger);
          digitalWrite(pins[trigger], HIGH);
          delay(1000);
          digitalWrite(pins[trigger], LOW);
          trigger++;
      }
  }

  delay(10);
}
