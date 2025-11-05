#include <LiquidCrystal.h>
// #include <IRremote.hpp>
#include <Servo.h>
#include <math.h>

/* 
  10k potentiometer 
  Connect swipe pin of the potentiometer to the VO pin on the 16 x 2 LCD

  Lastly the Ground (Black) and power wires (Red) 
  from the Arduino to the following pins on the LCD display

  GND   = VSS, RW, and (K with 220 ohms) 
  POWER = VDD, A
 */

// #define PI (22.0 / 7.0)
#define K2 0.23 //0.25
#define Epsilon 1.0

#define IR_RECEIVE_PIN 7
#define PAUSE_BUTTON_PIN 2

const int rs = 4, en = 6, d4 = 10, d5 = 11, d6 = 12, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int analogPin1 = A0;
const int analogPin2 = A1;
const int servoPin = 9;
const int recvPin = 7;

Servo solarServo;

int angle = 90;
float wattage = 0;
bool paused = 0;

void setup() {
  Serial.begin(9600); // Reminder that the CPX and Arduino have to have the same baudrate.
  // Serial.println("Wattage\tLight1\tLight2\tAngle");

  pinMode(0, INPUT);
  pinMode(PAUSE_BUTTON_PIN, INPUT_PULLUP);

  lcd.begin(16, 2);

  solarServo.attach(servoPin);
  solarServo.write(angle);
  
  // IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
}

void getAnalogIn(int *l1, int *l2) {
  *l1 = analogRead(analogPin1);
  *l2 = analogRead(analogPin2);
}

void updateAngle(int l1, int l2) {
  int angularDifference = round(K2 * Epsilon * (l1 - l2));
  // int angularDifference = sign(l2 - l1) * round(K * Epsilon * (2/PI) * (PI/2 - atan(abs(l2 - l1))));
  // Serial.print("deltaAngle: ");
  // Serial.println(angularDifference);

  angle -= angularDifference;
  angle = constrain(angle, 0, 180);

  solarServo.write(angle);
}

void updateWattage(int l1, int l2) {
  float lux = map(max(l1, l2), 0, 1023, 0, 100000);
  wattage = (lux / 100000.0) * 0.015 * 0.2 * 1000; // .04 = 40cm2 area, 0.2 = 20% panel efficiency
}

void lcdWrite(int l1, int l2) {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("L1:");
  lcd.print(l1);

  lcd.setCursor(0, 1);
  lcd.print("L2:");
  lcd.print(l2);

  lcd.setCursor(12, 0);
  lcd.print(angle);
  lcd.write((byte)223);

  lcd.setCursor(11, 1);
  lcd.print(wattage, 2);
  lcd.print("W");

  // Serial.print("Angle: ");
  // Serial.println(angle);
  // Serial.print("Wattage: ");
  // Serial.println(wattage);
}

void loop() {
  
  if (digitalRead(PAUSE_BUTTON_PIN) == LOW || paused) {
    
  }

  Serial.println("-----------------------------------");

  int lightValue1 = 0;
  int lightValue2 = 0;

  getAnalogIn(&lightValue1, &lightValue2);
  // Serial.write("L1: ");
  // Serial.print(lightValue1);
  // Serial.write(" | L2: ");
  // Serial.println(lightValue2);

  updateAngle(lightValue1, lightValue2);
  updateWattage(lightValue1, lightValue2);

  lcdWrite(lightValue1, lightValue2);
  
  lcd.write("Updating from loop");

  Serial.print("Wattage:");
  Serial.print(wattage, 2);
  Serial.print(",");
  Serial.print("Light_Value_1:");
  Serial.print(lightValue1);
  Serial.print(",");
  Serial.print("Light_Value_2:");
  Serial.print(lightValue2);
  Serial.print(",");
  Serial.print("Angle:");
  Serial.print(angle);

  delay(200); // milliseconds
}