/*
  S88.cpp
*/

#include <Arduino.h>
#include <S88.h>

S88::S88(QueueHandle_t s88Queue)
    : mClockPin(S88_CLOCK_PIN),
      mPSPin(S88_LOAD_PIN),
      mDataPin(S88_DATA_PIN),
      mNbModule(S88_MODULE_COUNT),
      mNbInputPerModule(S88_INPUTS_PER_MODULE),
      s88Queue(s88Queue) {}

void S88::setup()
{
  pinMode(mClockPin, OUTPUT);
  pinMode(mPSPin, OUTPUT);
  pinMode(mDataPin, INPUT_PULLDOWN);

  digitalWrite(mClockPin, LOW);
  digitalWrite(mPSPin, LOW);
}

// Méthode delay pour durée inférieures au ms
void S88::delay_us(uint32_t delay)
{
  uint64_t startDelayTime = esp_timer_get_time();
  while (esp_timer_get_time() < (startDelayTime + delay))
  {
  }
}

void S88::readS88()
{
  // Initialiser la lecture
  digitalWrite(mPSPin, LOW);
  digitalWrite(mClockPin, LOW);
  delay_us(18);
  // Charger les états des entrées des modules
  digitalWrite(mPSPin, HIGH);
  delay_us(18);
  digitalWrite(mPSPin, LOW);
  delay_us(18);
  // Lire les bits des données (un bit par impulsion d'horloge)
  uint16_t count = 0;
  for (uint8_t i = 0; i < mNbModule; i++)
  {
    for (uint8_t j = 0; j < mNbInputPerModule; j++)
    {
      byte data = digitalRead(mDataPin); // Lire le bit courant
      if (s88data[count] != data)
      {
        message.module = i;
        message.sensor = j;
        message.value = data;
        xQueueSend(s88Queue, (void *)&message, (TickType_t)1 != pdPASS); // Attendre 1 tick
        s88data[count] = data;
      }
      count++;
      digitalWrite(mClockPin, HIGH); // Envoyer une impulsion d'horloge
      delay_us(18);
      digitalWrite(mClockPin, LOW); // Remettre l'horloge à bas
      delay_us(18);
    }
  }
}
