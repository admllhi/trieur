#include <Arduino.h>
#include "rgb_lcd.h"
#include <ESP32Encoder.h>

int codeur;
int vitesse;
int codeurMemo = 0;
int erreur;
int commande;
int consigne;
int integ;


rgb_lcd lcd;
bool BP0;
bool BP1;
bool BP2;
int pot;
int cny;

int pos, tour,pos1;

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

void vTaskPeriodic(void *pvParameters)
{
  TickType_t xLastWakeTime;
  // Lecture du nombre de ticks quand la tâche commence
  xLastWakeTime = xTaskGetTickCount();
  while (1)
  {
    codeur = encoder.getCount();
    vitesse = codeur - codeurMemo;
    codeurMemo = codeur;
    erreur = consigne - vitesse;
    integ = integ + erreur;
    commande = 100 * erreur + 10 * integ;

    if (commande < -2047)
    {
      commande = -2047;
    }
    else if (commande > 2047)
    {
      commande = 2047;
    }
    if (commande > 0)
    {
      ledcWrite(1, commande);
      digitalWrite(26, 1);
    }
    else
    {
      ledcWrite(1, -commande);
      digitalWrite(26, 0);
    }
  }
    Serial.printf("%d \n", vitesse);
    // Endort la tâche pendant le temps restant par rapport au réveil,
    // ici 100ms, donc la tâche s'effectue ici toutes les 100ms.
    // xLastWakeTime sera mis à jour avec le nombre de ticks au prochain
    // réveil de la tâche.
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));


}

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
  xTaskCreate(vTaskPeriodic, "vTaskPeriodic", 10000, NULL, 2, NULL);
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

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.printf("pot : %d", pot);

  switch (etat)
  {
  case INIT:
    digitalWrite(25, 1);
    consigne = 2;
    while (cny < 3000)
    {
      cny = analogRead(36);

      Serial.printf("init %d \n", cny);
    }
    digitalWrite(25, 0);
    consigne = 0;
    integ  = 0;
    etat = ATTENTE;
    break;

  case ATTENTE:
    
    encoder.setCount(0);
    Serial.printf("BP0 = %d ", BP0);
    Serial.printf("BP1 = %d ", BP1);
    Serial.printf("Att \n");

    if (digitalRead(0) == LOW)
    {
      etat = ROT_HORAIRE;
    }
    else if (digitalRead(2) == LOW)
    {
      etat = ROT_ANTI_HORAIRE;
    }
    else
    {
      etat = ATTENTE;
    }
    break;

  case ROT_HORAIRE:
    Serial.printf("hor \n");
    pos = 0;
    encoder.setCount(0);
    pos1 = 0;
    while (pos > -800)
    {
      Serial.printf("dans le while \n");
      pos = ((int32_t)encoder.getCount());
      pos1 -= 100;
      while (pos >= pos1)
      {
        Serial.printf("dans le while 2\n");
        consigne = -2;
        digitalWrite(25, 1);
        pos = ((int32_t)encoder.getCount());
      }
      consigne = 0;
      integ = 0;
      digitalWrite(25, 0);
      delay(1000);
    }

    etat = ATTENTE;
    break;

  case ROT_ANTI_HORAIRE:
    Serial.printf("hor \n");
    pos = 0;
    encoder.setCount(0);
    pos1 = 0;
    while (pos < 800)
    {
      Serial.printf("dans le while \n");
      pos = ((int32_t)encoder.getCount());
      pos1 += 100;
      while (pos <= pos1)
      {
        Serial.printf("dans le while 2\n");
        consigne = 8;
        digitalWrite(25, 1);
        pos = ((int32_t)encoder.getCount());
      }
      consigne = 0;
      digitalWrite(25, 0);
      delay(1000);
    }

    etat = ATTENTE;
    break;}
}