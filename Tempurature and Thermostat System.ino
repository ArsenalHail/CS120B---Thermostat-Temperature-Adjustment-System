#include <LiquidCrystal.h>
#include <DHT.h>
#define DHTPIN A0     
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int h = 30; //humidity
int f = 75; //temperature in fahrenheit
int user_h = 30; 
int user_f = 75; 
int power = 0;

  unsigned int h4 = 2;
  unsigned int l4 = 8;
  unsigned int h5 = 1;
  unsigned int l5 = 3;
  unsigned int h6 = 3;
  unsigned int l6 = 2;
  unsigned int h7 = 95;
  unsigned int l7 = 5;

int VRx = 0; //for stick
int VRy = 0;
int JS_BTN = 13;
int j_btn = 1;
int BLU = 8;
int YEL = 9;
int WHI = 10;
int RED = 11;

int top = 1;
unsigned int a = 0;
unsigned int b = 0;
unsigned int c = 0;
unsigned int d = 0;

typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
} task;

int delay_gcd;
const unsigned short tasksNum = 7;
task tasks[tasksNum];

enum SM1_States { SM1_INIT, onTop, onBottom, offDisplay}; //DISPLAY
int SM1_Tick(int state1) {
  static int clearCheck = 0;
  switch (state1) { //transitions
    case SM1_INIT:
      state1 = offDisplay;
      break;
    case offDisplay:
      if (power == 1) {
        state1 = onTop;
        lcd.display();
      }
      else {
        state1 = offDisplay;
      }
      break;
    case onTop:
      if (power == 0) {
        state1 = offDisplay;
      }
      else if (top == 1) {
        state1 = onTop;
      }
      else {
        clearCheck = 0;
        state1 = onBottom;
      }
      break;
    case onBottom:
      if (power == 0) {
        state1 = offDisplay;
      }
      else if (top == 1) {
        clearCheck = 0;
        state1 = onTop;
      }
      else {
        state1 = onBottom;
      }
      break;
  }
  switch (state1) { //actions
    case SM1_INIT:
      break;
    case onTop:
      //Serial.println("onTop");
      if (clearCheck == 0) {
        lcd.clear(); //ran clear check
        ++clearCheck;
      }
      lcd.setCursor(0, 0); lcd.print("Tmp:"); lcd.print(f); 
      lcd.setCursor(7, 0); lcd.print("^");
      lcd.setCursor(8, 0); lcd.print("STmp:"); lcd.print(user_f); 
      lcd.setCursor(0, 1); lcd.print("Hum:"); lcd.print(h); 
      lcd.setCursor(8, 1); lcd.print("SHum:"); lcd.print(user_h); 
      break;
    case onBottom:
      //Serial.println("onBottom");
      if (clearCheck == 0) {
        lcd.clear(); //ran clear check
        ++clearCheck;
      }
      lcd.setCursor(0, 0); lcd.print("Tmp:"); lcd.print(f); 
      lcd.setCursor(8, 0); lcd.print("STmp:"); lcd.print(user_f); 
      lcd.setCursor(0, 1); lcd.print("Hum:"); lcd.print(h); 
      lcd.setCursor(7, 1); lcd.print("v");
      lcd.setCursor(8, 1); lcd.print("SHum:"); lcd.print(user_h); 
      break;
    case offDisplay:
      //Serial.println("offDisplay");
      lcd.noDisplay();
      break;
  } 
  return state1;
}

enum SM2_States {SM2_INIT, off2, sample};  //DHT SENSOR
int SM2_Tick(int state2) {
  switch (state2) { //transitions
    case SM2_INIT:
      state2 = off2;
      break;
    case off2:
      if (power == 1) {
        state2 = sample;
      }
      else {
        state2 = off2;
      }
      break;
    case sample:
      if (power == 1) {
        state2 = sample;
      }
      else {
        state2 = off2;
      }
      break;
  }
  switch (state2) {
    case SM2_INIT:
      break;
    case off2: 
      break;
    case sample:
      h = dht.readHumidity();
      f = dht.readTemperature(true);
      /*Serial.print("Humidity: ");
        Serial.print(h);
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(f);
        Serial.println(" *F\t"); */
      break;
  }
  return state2;
}

enum SM3_States {SM3_INIT, idle, btn, up, down, left, right} ; //JOYSTICK
int SM3_Tick(int state3) {
  switch (state3) { //transitions
    case SM3_INIT:
      state3 = idle;
      break;
    case idle:
      VRx = analogRead(A2);
      VRy = analogRead(A1);
      j_btn = digitalRead(JS_BTN);
      Serial.print("x: "); Serial.print(VRx); Serial.print(" - y: "); Serial.print(VRy); 
      Serial.print("     "); Serial.println(j_btn);
      //state3 = idle; 
      if (j_btn == 0) {
        state3 = btn;
      }
      else if (VRy > 900) {
        state3 = up;
      }
      else if (VRy < 100) {
        state3 = down;
      }
      else if (VRx < 100) {
        state3 = left;
      }
      else if (VRx > 900) {
        state3 = right;
      }
      else {
        state3 = idle;
      }
      break;
    case btn:
      j_btn = digitalRead(JS_BTN);
      Serial.println(j_btn);
      if (j_btn == 1) {
        if (power == 1) {
          power = 0;
        }
        else {
          power = 1;
        } 
        state3 = idle;
      }
      break;
    case up:
      VRy = analogRead(A1);
      if (VRy <= 800) {
        state3 = idle;
      }
      else {
        state3 = up;
      }
      break;
    case down:
      VRy = analogRead(A1);
      if (VRy >= 200) {
        state3 = idle;
      }
      else {
        state3 = down;
      }
      break;
    case left:
      VRx = analogRead(A2);
      if (VRx >= 200) {
        state3 = idle;
      }
      else {
        state3 = left;
      }
      break;
    case right:
      VRx = analogRead(A2);
      if (VRx <= 800) {
        state3 = idle;
      }
      else {
        state3 = right;
      }
      break;
  }
  switch (state3) { //actions
    case SM3_INIT:
      break;
    case idle:
      //Serial.println("very chill.");
      break;
    case up:
      //Serial.println("up");
      top = 1;
      break;
    case down:
      //Serial.println("down");
      top = 0;
      break;
    case left:
      //Serial.println("left");
      if (top == 1 && power == 1) {
        --user_f;
      }
      else if (top == 0 && power == 1) {
        --user_h;
      }
      break;
    case right:
      //Serial.println("right");
      if (top == 1 && power == 1) {
        ++user_f;
      }
      else if (top == 0 && power == 1){
        ++user_h;
      }
      break;
  }
  return state3;
}

enum SM4_States {SM4_INIT, idle4, high4, low4}; //BLUE
int SM4_Tick(int state4) {

  switch (state4) {
    case SM4_INIT:
      state4 = idle4;
      break;
    case idle4:
      if (user_h > h && power == 1) {
        state4 = high4;
      }
      break;
    case high4:
      if (a >= h4) {
        state4 = low4;
        a = 0;
      }
      else {
        state4 = high4;
      }
      break;
    case low4:
      if (user_h == h || power == 0) {
        state4 = idle4;
        a = 0;
      }
      else if (a >= l4) {
        state4 = high4;
        a = 0;
      }
      else {
        state4 = low4;
      }
      break;
  }
  switch (state4) {
    case SM4_INIT:
      break;
    case idle4:
      digitalWrite(BLU, LOW); 
      break;
    case high4:
      digitalWrite(BLU, HIGH); 
      ++a;
      break;
    case low4:
      digitalWrite(BLU, LOW); 
      ++a;
      break;
  }
  return state4;
}

enum SM5_States {SM5_INIT, idle5, high5, low5}; //YELLOW
int SM5_Tick(int state5) {

  switch (state5) {
    case SM5_INIT:
      state5 = high5;
      break;
    case idle5:
      if (user_h < h && power == 1) {
        state5 = high5;
      }
      break;
    case high5:
      if (b >= h5) {
        state5 = low5;
        b = 0;
      }
      else {
        state5 = high5;
      }
      break;
    case low5:
      if (user_h == h || power == 0) {
        state5 = idle5;
        b = 0;
      }
      else if (b >= l5) {
        state5 = high5;
        b = 0;
      }
      else {
        state5 = low5;
      }
      break;;
  }
  switch (state5) {
    case SM5_INIT:
      break;
    case idle5:
      digitalWrite(YEL, LOW); 
      break;
    case high5:
      digitalWrite(YEL, HIGH); 
      //Serial.print("HIGH: "); Serial.println(b);
      ++b;
      break;
    case low5:
      digitalWrite(YEL, LOW); 
      //Serial.print("LOW: "); Serial.println(b);
      ++b;
      break;
  }
  return state5;
}

enum SM6_States {SM6_INIT, idle6, high6, low6}; //WHITE
int SM6_Tick(int state6) {

  switch (state6) {
    case SM6_INIT:
      state6 = high6;
      break;
    case idle6:
      if (user_f < f && power == 1) {
        state6 = high6;
      }
      break;
    case high6:
      if (c >= h6) {
        state6 = low6;
        c = 0;
      }
      else {
        state6 = high6;
      }
      break;
    case low6:
      if (user_f == f || power == 0) {
        state6 = idle6;
        c = 0;
      }
      else if (c >= l6) {
        state6 = high6;
        c = 0;
      }
      else {
        state6 = low6;
      }
      break;
  }
  switch (state6) {
    case SM6_INIT:
      break;
    case idle6:
      digitalWrite(WHI, LOW); 
      break;
    case high6:
      digitalWrite(WHI, HIGH); 
      ++c;
      break;
    case low6:
      digitalWrite(WHI, LOW); 
      ++c;
      break;
  }
  return state6;
}

enum SM7_States {SM7_INIT, idle7, high7, low7}; //RED
int SM7_Tick(int state7) {

  switch (state7) {
    case SM7_INIT:
      state7 = high7;
      break;
    case idle7:
      if (user_f > f && power == 1) {
        state7 = high7;
      }
      break;
    case high7:
      if (d >= h7) {
        state7 = low7;
        d = 0;
      }
      else {
        state7 = high7;
      }
      break;
    case low7:
      if (user_f == f || power == 0) {
        state7 = idle7;
        d = 0;
      }
      else if (d >= l7) {
        state7 = high7;
        d = 0;
      }
      else {
        state7 = low7;
      }
      break;
  }
  switch (state7) {
    case SM7_INIT:
      break;
    case idle7:
      digitalWrite(RED, LOW); 
      break;
    case high7:
      digitalWrite(RED, HIGH); 
      ++d;
      break;
    case low7:
      digitalWrite(RED, LOW); 
      ++d;
      break;
  }
  return state7;
}

void setup() {
  unsigned char i = 0;
  tasks[i].state = SM1_INIT;
  tasks[i].period = 100; 
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM1_Tick;
  i++;
  tasks[i].state = SM2_INIT;
  tasks[i].period = 500;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM2_Tick;
  i++;
  tasks[i].state = SM3_INIT;
  tasks[i].period = 300;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM3_Tick;
  i++;
  tasks[i].state = SM4_INIT;
  tasks[i].period = 20; //set to 20 ***
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM4_Tick;
  i++;
  tasks[i].state = SM5_INIT;
  tasks[i].period = 100; //set to 100 ***
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM5_Tick;
  i++;
  tasks[i].state = SM6_INIT;
  tasks[i].period = 60; //set to 60 ***
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM6_Tick;
  i++;
  tasks[i].state = SM7_INIT;
  tasks[i].period = 1; //set to 1 ***
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &SM7_Tick;
  
  lcd.begin(16, 2);

  pinMode(JS_BTN, INPUT_PULLUP);
  
  //pinMode(BLU, OUTPUT);
  //pinMode(YEL, OUTPUT);
  //pinMode(WHI, OUTPUT);
  //pinMode(RED, OUTPUT);

  delay_gcd = 1000; // GCD
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  unsigned char i;
  for (i = 0; i < tasksNum; ++i) {
    if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
      tasks[i].state = tasks[i].TickFct(tasks[i].state);
      tasks[i].elapsedTime = millis(); // Last time this task was ran
    }
  }
  
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  //lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  //lcd.print(millis() / 1000);
  //delay(1000);
  //float h = dht.readHumidity();
  //float t = dht.readTemperature();
  //float f = dht.readTemperature(true);

  //if (isnan(h) || isnan(t) || isnan(f)) {
    //Serial.println("Failed to read from DHT sensor!");
    //return;
  //}

  //float hif = dht.computeHeatIndex(f, h);
  //float hic = dht.computeHeatIndex(t, h, false);
  /*
  
  */
}
