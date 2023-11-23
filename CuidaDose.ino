#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

#define SDA_PIN 21
#define SCL_PIN 22
#define BUTTON_PIN 18
#define SERVO_PIN 14
#define RED_LED_PIN 16
#define GREEN_LED_PIN 17
#define BUZZER_PIN 15  // Pino do buzzer

LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C e tamanho do display
Servo myservo;

int buttonState = HIGH; // HIGH quando não está pressionado
int lastButtonState = HIGH;
unsigned long startTime = 0;
bool timerRunning = false;
unsigned long buzzerStartTime = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  myservo.attach(SERVO_PIN);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Inicializa o display
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Verifica o estado do botão
  buttonState = digitalRead(BUTTON_PIN);

  // Verifica se o botão foi pressionado
  if (buttonState == LOW && lastButtonState == HIGH) {
    // Inicia o temporizador
    startTimer();
  }

  // Verifica se o temporizador está em execução
  if (timerRunning) {
    // Calcula o tempo decorrido
    unsigned long elapsedTime = millis() - startTime;

    // Atualiza o display com o tempo restante
    updateDisplay(elapsedTime);

    // Verifica se o temporizador chegou ao fim
    if (elapsedTime >= 10000) {
      // Desliga temporizador
      stopTimer();

      // Exibe a mensagem no display
      lcd.clear();
      lcd.print("Hora de tomar");
      lcd.setCursor(0, 1);
      lcd.print("o remedio");

      // Aciona o buzzer e move o servo
      activateGreenComponents();
    }
  }

  // Verifica se o buzzer está ativo
  if (buzzerStartTime > 0) {
    unsigned long buzzerElapsedTime = millis() - buzzerStartTime;
    if (buzzerElapsedTime >= 5000) {
      // Desliga o buzzer após 5 segundos
      digitalWrite(BUZZER_PIN, LOW);
      buzzerStartTime = 0;
    }
  }

  // Atualiza o estado do botão
  lastButtonState = buttonState;
}

void startTimer() {
  startTime = millis();
  timerRunning = true;
  lcd.clear();
  lcd.print("Tempo restante:");
}

void stopTimer() {
  timerRunning = false;
  lcd.clear();
}

void updateDisplay(unsigned long elapsedTime) {
  int remainingTime = 10 - elapsedTime / 1000;
  lcd.setCursor(0, 1);
  lcd.print("Faltam: ");
  lcd.print(remainingTime);
}

void activateGreenComponents() {
  digitalWrite(GREEN_LED_PIN, HIGH);  // Acende o LED verde

  // Aciona o buzzer simulado
  digitalWrite(BUZZER_PIN, HIGH);
  buzzerStartTime = millis();  // Registra o início do tempo para controlar a duração

  // Move o servo para a posição de 180 graus (ajuste conforme necessário)
  myservo.write(180);

  // Aguarde um curto período de tempo para permitir que o servo alcance a posição desejada
  delay(500);

  // Desliga o LED verde
  digitalWrite(GREEN_LED_PIN, LOW);
}
