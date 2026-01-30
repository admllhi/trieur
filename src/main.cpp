#include <Arduino.h>
#include "rgb_lcd.h"
#include <ESP32Encoder.h>

rgb_lcd lcd;
bool BP0;
bool BP1;
bool BP2;
int pot;
int cny;

int pwm = 1;
int freq = 25000;
int res = 11;

ESP32Encoder encoder;

enum Etat
{
  INIT,
  ATTENTE,
  ROT_HORAIRE,
  ROT_ANTI_HORAIRE
};

Etat etat = INIT;

void setup()
{
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

  pinMode(36, INPUT);

  ledcSetup(pwm, freq, res);
  ledcAttachPin(27, pwm);

  encoder.attachFullQuad(23, 19);
  encoder.clearCount();
  Serial.println("Encoder Start = " + String((int32_t)encoder.getCount()));
}

void loop()
{
  BP0 = digitalRead(0);
  BP1 = digitalRead(2);
  BP2 = digitalRead(12);
  pot = analogRead(33);
  cny = analogRead(36);

  // Serial.printf("BP0 = %d ",BP0 );
  // Serial.printf("BP1 = %d ",BP1);
  // Serial.printf("BP2 = %d ",BP2);
  // Serial.printf("pot = %d ",pot);
  // Serial.printf("cny = %d ",cny);
  // Serial.printf("enc = %d \n",(int32_t)encoder.getCount());

  ledcWrite(pwm, pot / 2);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.printf("pot : %d", pot);

  switch (etat)
  {

  case INIT:
    digitalWrite(25, 1);
    digitalWrite(26, 1);
    ledcWrite(pwm, 1300);
    while (cny < 2000)
    {
      cny = analogRead(36);

      Serial.printf("init %d \n", cny);
    }
    etat = ATTENTE;
    break;

  case ATTENTE:
    digitalWrite(25, 0);
    encoder.setCount(0);
    Serial.printf("Att \n");
    int pos ((int32_t)encoder.getCount());


    if (digitalRead(BP0) == LOW)
    {
      etat = ROT_HORAIRE;
    }
    else if (digitalRead(BP1) == LOW)
    {
      etat = ROT_ANTI_HORAIRE;
    }
    break;

  case ROT_HORAIRE:
    Serial.printf("hor \n");
    digitalWrite(25, 1);
    digitalWrite(26, 0);
    etat = ATTENTE;
    break;

  case ROT_ANTI_HORAIRE:
    Serial.printf("ANTI-hor \n");
    digitalWrite(25, 1);
    digitalWrite(26, 1);
    etat = ATTENTE;
    break;
  }
}
