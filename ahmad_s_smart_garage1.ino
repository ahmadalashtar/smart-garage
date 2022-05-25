#include <Adafruit_LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 3; //four columns


char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte pin_rows[ROW_NUM] = {11,10, 9, 8}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {7,6,5}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );
const String password = "1"; // change your password here
String keypadinput = "";
#define maxPassDigits 32


Adafruit_LiquidCrystal lcd_1(0);

#define maxSlots 6
int slots = 0;
int stage = 0;
char nkey;

int pos = 0;
Servo servo;
#define servo_pin 13
#define servo_degree 190


#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin1 1 //attach pin D3 Arduino to pin Trig of HC-SR04
#define trigPin2 3
#define trigPin3 4
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
#define carToSensor 30

int motorPin = 12;


void setup() {
  pinMode(motorPin, OUTPUT);
  Serial.begin(9600);
  lcd_1.begin(16, 2);
  keypadinput.reserve(maxPassDigits);
  servo.attach(servo_pin);
  pinMode(trigPin1, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(trigPin2, OUTPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
}

void loop() {

  switch (stage) {
    case 0:
      clear();
      display(0,0,"Press # to start");
      stage+=1;

      break;
    case 1:
      nkey = keypad.getKey();
      if (nkey){ 
        start(nkey);  
      }
      break;
    case 2:
      if (slots > 0) { 
        stage+=2;
      }else{
        clear();
        display(0,0,"Number of occup-");
        display(0,1,"ied slots:");
        stage+=1;

      }
      
      break;
    case 3:
      nkey = keypad.getKey();
      if (nkey){ 
        enterSlots(nkey);  
        }
      break;
    case 4:
      clear();
      display(0,0,"Enter Password:");
      keypadinput = "";
      stage+=1;


      break;
    case 5:
      nkey = keypad.getKey();
      if (nkey){ enterPassword(nkey);  }
      break;
    case 6:
      openTheGate();
      stage+=1;
      break;
    case 7:
      clear();
      display(0,0,"Getting car into");
      display(0,1,"place");
      stage+=1;
      break;
    case 8:
      digitalWrite(motorPin, HIGH);
      stage+=1;
      break;
      case 9:
      activateSensor();
      break;
      case 10:
      clear();
      display(0,0,"Closing the Gate");
      break;
      case 11:
        CloseTheGate();
        stage = 0;
        break;
  }

}

void display(int column, int line, String content) {
  lcd_1.setCursor(column, line);
  lcd_1.print(content);
  lcd_1.setBacklight(1);
  //delay(500); // Wait for 500 millisecond(s)
}
void clear(){
  lcd_1.clear();
}
void start(char key){
  if(key == '#') {
    stage+=1;
  } 
}
void enterSlots(char key){
  if(key == '*') {
    keypadinput = "";
    stage-=1;
  } 
  else if(key == '#') {
    if( keypadinput.toInt() < maxSlots ) {
      clear();
      slots = keypadinput.toInt()+1;
      stage+=1;
    } else {
      clear();
      display(0,0,"No Free Slots");
      delay(1500);
      keypadinput = "";
      stage-=1;
    }

    keypadinput = ""; // clear input password
  } 
  else {
  keypadinput += key; // append new character to input password string
  display(10,1,keypadinput);
  }
}
void enterPassword(char key) {
    

    if(key == '*') {
      stage-=1;

    } 
    else if(key == '#') {
      if(password == keypadinput) {
        clear();
        display(0,0,"Correct Password");
        display(0,1,"Opening the Gate");
        stage+=1;
        
      } else {
        clear();
        display(0,0,"Incorrect Passw-");
        display(0,1,"ord, press *");
      }

      keypadinput = ""; // clear input password
    } 
    else {
      keypadinput += key; // append new character to input password string
      display(0,1,keypadinput);
    }
}
void openTheGate(){
  for (; pos <= servo_degree; pos += 1) {
    // tell servo to go to position in variable 'pos'
    servo.write(pos);
    // wait 15 ms for servo to reach the position
    delay(15); // Wait for 15 millisecond(s)
  }
}
void activateSensor(){
  if (slots<3){
    //activate sensor 1
    digitalWrite(trigPin1, LOW);

    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin1, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    if (distance <= carToSensor){
      digitalWrite(motorPin, LOW);
      stage += 1;
    }

  } else if (slots <5){
    //activate sensor 2
    digitalWrite(trigPin2, LOW);

    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    if (distance <= carToSensor){
      digitalWrite(motorPin, LOW);
      stage += 1;    }
  } else {
    // activate sensor 3
    digitalWrite(trigPin3, LOW);

    delayMicroseconds(2);
    // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
    digitalWrite(trigPin3, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin3, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
    if (distance <= carToSensor){
      
      digitalWrite(motorPin, LOW);
      stage += 1;
          }
  }
}
void CloseTheGate(){
  for (; pos > 0; pos -= 1) {
    // tell servo to go to position in variable 'pos'
    servo.write(pos);
    // wait 15 ms for servo to reach the position
    delay(15); // Wait for 15 millisecond(s)
  }
}