#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <PubSubClient.h>

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

// Configurações do MQTT
#define TOPICO_SUBSCRIBE    "/TEF/hosp340/cmd" 
const char* BROKER_MQTT = "46.17.108.113"; // Coloque o endereço do seu broker MQTT
int BROKER_PORT = 1883; // Porta do Broker MQTT
const char* TOPICO_PUBLISH = "/TEF/hosp340/attrs"; // Tópico MQTT de envio de informações para o Broker
const char* TOPICO_PUBLISH_2 = "/TEF/hosp340/attrs/l"; // Tópico MQTT de envio de informações de luminosidade para o Broker
const char* TOPICO_PUBLISH_3 = "/TEF/hosp340/attrs/h"; // Tópico MQTT de envio de informações de umidade para o Broker
const char* TOPICO_PUBLISH_4 = "/TEF/hosp340/attrs/t"; // Tópico MQTT de envio de informações de temperatura para o Broker
const char* TOPICO_PUBLISH_5 = "/TEF/hosp340/attrs/s";
const char* ID_MQTT = "fiware_340"; // Id MQTT (para identificação de sessão)

// Saídas Leds
int D4 = 2;       // Led builtin do ESP32 (led na placa)

// WIFI
const char* SSID = "Wokwi-GUEST"; // SSID / nome da rede WI-FI que deseja se conectar
const char* PASSWORD = ""; // Senha da rede WI-FI que deseja se conectar

//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient
char EstadoSaida = '0';  //variável que armazena o estado atual da saída
  
//Prototypes
void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi(); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);
void InitOutput(void);
 
/* 
 *  Implementações das funções
 */
void setup() 
{
    //inicializações:
    InitOutput();
    initSerial();
    initWiFi();
    initMQTT();
    MQTT.publish(TOPICO_PUBLISH, "s|off");

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


void initSerial() 
{
  Serial.begin(115200);
}

void initWiFi() 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
     
    reconectWiFi();
}

void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
    MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;
     
    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
    
    Serial.print("- Mensagem recebida: ");
    Serial.println(msg);
    
    //toma ação dependendo da string recebida:
    //verifica se deve colocar nivel alto de tensão na saída D0:
    //IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
    //enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
    if (msg.equals("hosp340@on|"))
    {
        digitalWrite(D4, HIGH);
        EstadoSaida = '1';
    }
 
    //verifica se deve colocar nivel alto de tensão na saída D0:
    if (msg.equals("hosp340@off|"))
    {
        digitalWrite(D4, LOW);
        EstadoSaida = '0';
    }
}

void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}

void reconectWiFi() 
{
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)
        return;
         
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
     
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
   
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}

void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
     
     reconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void EnviaEstadoOutputMQTT(void)
{
    if (EstadoSaida == '1')
    {
      MQTT.publish(TOPICO_PUBLISH, "s|on");
      Serial.println("- Led Ligado");
    }
    if (EstadoSaida == '0')
    {
      MQTT.publish(TOPICO_PUBLISH, "s|off");
      Serial.println("- Led Desligado");
    }
    Serial.println("- Estado do LED onboard enviado ao broker!");
    delay(1000);
}

void InitOutput(void)
{
    //IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
    //enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
    pinMode(D4, OUTPUT);
    digitalWrite(D4, HIGH);
    
    boolean toggle = false;

    for(int i = 0; i <= 10; i++)
    {
        toggle = !toggle;
        digitalWrite(D4,toggle);
        delay(200);           
    }

    digitalWrite(D4, LOW);
}

void loop() 
{   
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
      
      // Funções MQTT
      VerificaConexoesWiFIEMQTT();
      EnviaEstadoOutputMQTT();
      
      // Envie a mensagem para o servidor MQTT
      MQTT.publish(TOPICO_PUBLISH_5, "Tomou o remédio!");
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

  MQTT.loop();
}
