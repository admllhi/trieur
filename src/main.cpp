#include <Arduino.h>
#include "rgb_lcd.h"

rgb_lcd lcd;
bool BP0;
bool BP1;
bool BP2;
int pot;

int pwm = 1;
int freq = 25000;
int res = 11;

void setup() {
  // Initialise la liaison avec le terminal
  Serial.begin(115200);

  // Initialise l'écran LCD
  Wire1.setPins(15, 5);
  lcd.begin(16, 2, LCD_5x8DOTS, Wire1);
  lcd.printf("Trieur de balles");
  pinMode(0, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);

  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);

  ledcSetup(pwm, freq, res);
  ledcAttachPin(27, pwm);
}

void loop() {
  BP0 = digitalRead(0);
  BP1 = digitalRead(2);
  BP2 = digitalRead(12);
  pot = analogRead(33);



  Serial.printf("BP0 = %d ",BP0 );
  Serial.printf("BP1 = %d ",BP1);
  Serial.printf("BP2 = %d ",BP2);
  Serial.printf("pot = %d \n",pot);

  ledcWrite(pwm, pot/2);
  digitalWrite(25, BP0);
  digitalWrite(26, BP1);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.printf("pot : %d", pot);

}
