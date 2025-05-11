/*
 * dzzie@yahoo.com
 * http://sandsprite.com
 * free for any use
 * 
 * v1 video here:  https://youtu.be/68TlArt3PbY
 * v2 video:       https://youtu.be/6gox5O4asF4
 
 * this sketch is for a leonardo board, it gives you a bank of external buttons that will do
 * common tasks such as cut, copy, paste, select all with single button clicks. 
 * very simple, no resistors needed since we use the internal pullups on the arduino. debounce handled
 * by software delay.
 * 
 * this code also supports sending an adjustable delay mouse clicks, or mouse scroll wheel events
 * I use a 3 position switch select which (center is off). The time delay comes by
 * reading a potentiometer. Delay is currently mapped between 10ms and 3 seconds.
 * this features requires 3 more inputs. 
 * 
 * if it says keyboard.h not found, make sure you set the board to leanardo. You may also have to use a 
 * newer IDE (ex 1.6.9+)
 * 
 * if you have problems uploading, press the reset button down until it gets to the uploading phase
 
version 2 button configuration (button index not pin number)

 7   1
 8   2
 9   3

4   5   6

 dpdt switch for scroll/click mode
 pot 1 for scroll/click frequency
 pot 2 for profile selection - 4 profiles included, only affects buttons 4,7,8,9

1 = copy
2 = paste
3 = select all          
5 = single click (left)
6 = double click

profile 1: ollydbg
  4 = ctrl_G, paste (goto address in clipboard)
  7 = f7 step in
  8 = f8 step over
  9 = ctrl-f9 = execute till return (step out)

profile 2: vb6
  4 = not defined
  7 = f8 step into
  8 = shift f8 step over
  9 = ctrl shift f8 - step out

profile 3: visual studio
  4 = not defined
  7 = f11 step into
  8 = f10 step over
  9 = shift f11 - step out

profile 4: wingware
  4 = not defined
  7 = f7 step in
  8 = step over f6
  9 = step out f8

https://www.arduino.cc/en/Reference/KeyboardModifiers

 */

#include <Keyboard.h>
#include <Mouse.h>

const int copy            = 3;
const int paste           = 4;
const int selall          = 5; //changed to cut
const int button4         = 8; 
const int lClick          = 7; 
const int dblClick        = 6; 
const int button7         = 11;
const int button8         = 10;
const int button9         = 9;
const int scroll_selected = 13;
const int click_selected  = 12;
const int potPin          = A5;
const int profilePot      = A1;

enum profiles{ olly = 1, vb6 = 2, vs = 3, wing = 4};

void setup() { 
  
  pinMode(selall, INPUT_PULLUP);
  pinMode(copy, INPUT_PULLUP);
  pinMode(paste, INPUT_PULLUP);
  pinMode(lClick, INPUT_PULLUP);
  pinMode(dblClick, INPUT_PULLUP);

  pinMode(button4, INPUT_PULLUP);
  pinMode(button7, INPUT_PULLUP);
  pinMode(button8, INPUT_PULLUP);
  pinMode(button9, INPUT_PULLUP);
  
  pinMode(scroll_selected, INPUT_PULLUP);
  pinMode(click_selected, INPUT_PULLUP);
  pinMode(potPin, INPUT);
  pinMode(profilePot, INPUT);
  
  Serial.begin(9600);
  Mouse.begin();
  Keyboard.begin();
}

void SendCmd(char c){
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(c);
    delay(100);
    Keyboard.releaseAll(); 
    delay(150);
}

int readPot(){
      int val = analogRead(potPin);         // reads the value of the potentiometer (value between 0 and 1023) 
      //Serial.print("pot pin:"); Serial.println(val);
      return map(val, 0, 1023, 10, 3000);   // scale it to use it as a ms time delay (value between 10 and 3000) 
}

void checkDebugKeys(void){

    bool btnSpecial=(digitalRead(button4) == LOW);
    bool stepIn =   (digitalRead(button7) == LOW);
    bool stepOver = (digitalRead(button8) == LOW);
    bool stepOut =  (digitalRead(button9) == LOW);

    if(!stepIn && !stepOver && !stepOut) return;

    int val = analogRead(profilePot);

    //give it 3 even ranges to work with.. map(1,3) = dead 0, anywhere in between, dead max
    profiles profile = olly;
    if(val >= 250 && val < 500) profile = vb6;
    if(val >= 500 && val < 750) profile = vs;
    if(val >= 750) profile = wing;

/*
    Serial.print("profile:"); Serial.print(profile);
    Serial.print(" stepIn: "); Serial.print(stepIn);
    Serial.print(" stepOver: "); Serial.print(stepOver);
    Serial.print(" stepOut: "); Serial.println(stepOut);
*/

   if(profile == olly){
        if(btnSpecial){ SendCmd('g'); SendCmd('v'); /*press enter too? leave manual for data verify*/ }
        if(stepIn)    Keyboard.press(KEY_F7);
        if(stepOver)  Keyboard.press(KEY_F8);
        if(stepOut){  Keyboard.press(KEY_LEFT_CTRL); Keyboard.press(KEY_F9);}
   }

   if(profile == vb6){
        if(btnSpecial){SendCmd('x');}
        if(stepIn)   Keyboard.press(KEY_F8);
        if(stepOver){Keyboard.press(KEY_RIGHT_SHIFT); Keyboard.press(KEY_F8);}
        if(stepOut){ Keyboard.press(KEY_LEFT_CTRL); Keyboard.press(KEY_RIGHT_SHIFT); Keyboard.press(KEY_F8);}
   }

   if(profile == vs){
        if(btnSpecial){SendCmd('x');}
        if(stepIn)   Keyboard.press(KEY_F11);
        if(stepOver) Keyboard.press(KEY_F10);
        if(stepOut){ Keyboard.press(KEY_RIGHT_SHIFT); Keyboard.press(KEY_F11);}
   }

   if(profile == wing){
        if(btnSpecial){SendCmd('x');}
        if(stepIn)   Keyboard.press(KEY_F7);
        if(stepOver) Keyboard.press(KEY_F6);
        if(stepOut)  Keyboard.press(KEY_F8);
   }

   delay(100);
   Keyboard.releaseAll(); 
   delay(150);
     
}

void loop() {

    int ms = 0;

    checkDebugKeys(); 
    
    if (digitalRead(selall) == LOW) SendCmd('x');
    if (digitalRead(copy) == LOW)   SendCmd('c');
    if (digitalRead(paste) == LOW)  SendCmd('v');
    
    if (digitalRead(lClick) == LOW){ 
        //Mouse.click(); delay(200);
        //this allows for holding the mouse down for selection operations as well instead of just clicks...
        Mouse.press(); delay(200);
        while((digitalRead(lClick) == LOW)){delay(200);}
        Mouse.release();
    }    
        
    if (digitalRead(dblClick) == LOW){ Mouse.click(); delay(200); Mouse.click();delay(200);}
    //if (digitalRead(rClick) == LOW){ Mouse.click(MOUSE_RIGHT);  delay(200); }
    
    if (digitalRead(scroll_selected) == LOW){
        ms = readPot();
       // Serial.print("MouseScroll 1 delay:");
       // Serial.println(ms);
        Mouse.move(0, 0, -1);
        delay(ms);
    }

    if (digitalRead(click_selected) == LOW){
        ms = readPot();
        //Serial.print("Mouse Click delay:");
        //Serial.println(ms);
        Mouse.click();
        delay(ms);
    }
    

}
