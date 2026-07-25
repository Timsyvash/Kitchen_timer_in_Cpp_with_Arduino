#include <Arduino.h>

const short buttonPin = 2;  // Пін кнопки
const short buzzerPin = 12; // Пін пищалки (бузера)

unsigned long last_time = 0;
unsigned long targetTime = 0;
unsigned long interval = 60UL * 1000UL; // Час таймера: 1 хвилина

bool running = false;
bool lastButtonState = HIGH;

short fr;

void setup()
{
  // Ініціалізація Serial порту (швидкість 9600 бод)
  Serial.begin(9600);
  while (!Serial)
  {
    ; // Чекаємо на підключення порту (актуально для Leonardo/Micro)
  }
  Serial.println("--- TIMER STARTED ---");

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  randomSeed(analogRead(0));
  fr = 400 + random(600);

  Serial.print("Generated sound frequency: ");
  Serial.print(fr); // Виправлено .concat на .print
  Serial.println(" Hz");
}

void loop()
{
  bool currentButtonState = digitalRead(buttonPin);

  // Перевірка натискання кнопки (зміна стану з HIGH на LOW)
  if (lastButtonState == HIGH && currentButtonState == LOW)
  {
    Serial.println("Button press fixation...");

    // Перевірка антидребезгу
    if (millis() - last_time >= 300)
    {
      if (!running)
      {
        // Запуск таймера
        targetTime = millis() + interval;
        running = true;
        noTone(buzzerPin);

        Serial.print("Timer STARTED on ");
        Serial.print(interval / 1000); // Виправлено .concat на .print
        Serial.println(" sec.");
      }
      else
      {
        // Скидання таймера
        running = false;
        noTone(buzzerPin);
        Serial.println("Timer RESET by user.");
      }
    }
    else
    {
      Serial.println("Ignoring due to contact bounce.");
    }

    // Оновлюємо час останнього натискання ЗАМЕЖАМИ умови if(running),
    // щоб антидребезг працював коректно в обох режимах
    last_time = millis();
  }
  lastButtonState = currentButtonState;

  // Щосекундний вивід залишку часу (щоб бачити, що Arduino не зависла)
  static unsigned long lastTick = 0;
  if (running && (millis() - lastTick >= 1000))
  {
    lastTick = millis();
    long secondsLeft = (targetTime - millis()) / 1000;
    if (secondsLeft >= 0)
    {
      Serial.print("Time left: ");
      Serial.print(secondsLeft); // Виправлено .concat на .print
      Serial.println(" sec.");
    }
  }

  // Відлік часу закінчився
  if (running && millis() >= targetTime)
  {
    running = false;
    Serial.println("IT'S TIME TO GO OUT! I'm turning on the beeper...");

    // Звуковий сигнал тричі
    for (int i = 0; i < 3; i++)
    {
      Serial.print("Гудок ");
      Serial.println(i + 1);
      tone(buzzerPin, fr);
      delay(300);
      noTone(buzzerPin);
      delay(300);
    }
    Serial.println("The signal is complete. The timer is ready to start again..");
  }
}
