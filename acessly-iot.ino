
#include <WiFi.h>                //biblioteca para conectar na rede WiFi
#include <PubSubClient.h>        //biblioteca cliente MQTT para enviar/receber mensagens
#include <ArduinoJson.h>         //biblioteca para monstar e manipular estruturas JSON
#include <DHTesp.h>              //biblioteca para sensor de temperatura e umidade (DHT22)
#include <LiquidCrystal_I2C.h>   //biblioteca para display LCD via I2C
#include <Wire.h>  

#define WIFI_SSID "Wokwi-GUEST"             //nome da rede WiFi simulada no Wokwi
#define WIFI_PASSWORD ""                    //sem senha na rede do Wokwi
#define MQTT_SERVER "broker.hivemq.com"     //broker MQTT público
#define MQTT_PORT 1883                      //porta padrão para MQTT
#define MQTT_TOPIC_BASE "/acessly/empresa"  //tópico base das mensagens MQTT
#define EMPRESA_ID "001"                    //identificador deste dispositivo/empresa

//Sensores
#define DHT_PIN 15         // SDA do DHT22
#define LDR_PIN 34         // AO do LDR
#define POT_PIN 35         // SIG do potenciômetro
#define BUTTON_PIN 32      //pino digital do botão

//Atuadores
#define LED_GREEN 25       // LED verde
#define LED_YELLOW 26      // LED amarelo
#define LED_RED 27         // LED vermelho
#define LCD_SDA 21         // SDA do LCD
#define LCD_SCL 22         // SCL do LCD

//Instanciação de objetos das bibliotecas
DHTesp dht;               //objeto para sensor DHT22
WiFiClient espClient;     //cliente WiFi
PubSubClient mqtt(espClient);   //cliente MQTT
LiquidCrystal_I2C lcd(0x27, 16, 2);   //endereço I2C padrão do LCD

//Variáveis para armazenar leituras dos sensores
float temperature = 0;            //temperatura atual (°C)
float humidity = 0;               //umidade relativa atual (%)
int lightLevel = 0;               //nível de luminosidade (0-4095)
int noiseLevel = 0;               //nível de ruído simulado (0-4095)
bool obstructionDetected = false; //flag: caminho obstruído?
int accessibilityScore = 100;     //score de acessibilidade (0-100%)

//Limiares para validar ambiente adequado
const int MIN_LIGHT = 200;
const int MAX_NOISE = 600;
const float MIN_TEMP = 20.0;
const float MAX_TEMP = 24.0;

const int MQTT_BUFFER_SIZE = 1024;  //aumenta buffer para 1KB

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n================================");
  Serial.println("   ACESSLY MONITOR - ESP32");
  Serial.println("================================");
  
  Serial.println("[1/5] Inicializando hardware...");
  dht.setup(DHT_PIN, DHTesp::DHT22);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  Serial.println("[2/5] Inicializando LCD...");
  Wire.begin(LCD_SDA, LCD_SCL);  // ← ESSENCIAL!
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Acessly Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");

  Serial.println("[3/5] Conectando WiFi...");
  connectWiFi();
  
  Serial.println("[4/5] Configurando MQTT...");
  mqtt.setBufferSize(MQTT_BUFFER_SIZE);
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  
  Serial.println("[4.5/5] Conectando ao broker MQTT...");
  delay(1000);
  connectMQTT();
  
  Serial.println("[5/5] Setup completo!");
  Serial.println("================================\n");
  
  delay(2000);
  lcd.clear();
}

void loop() {

  //Verifica e reconecta WiFi se necessário
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado! Reconectando...");
    connectWiFi();
  }
  
  //Verifica e conecta MQTT se necessário
  if (!mqtt.connected()) {
    Serial.println("MQTT desconectado! Conectando...");
    connectMQTT();
  }
  
  mqtt.loop();  //mantém cliente MQTT ativo

  //Ciclo de monitoramento
  readSensors();                     //faz leitura dos sensores
  calculateAccessibilityScore();     //calcula score conforme limites
  updateIndicators();                //atualiza LEDs e LCD conforme score/status
  publishData();                     //monta JSON e publica no MQTT
  
  delay(5000);  //aguarda 5s até próxima leitura
}

//Conecta na rede WiFi, mostrando progresso no LCD
void connectWiFi() {
  Serial.println("Iniciando conexao WiFi...");
  lcd.setCursor(0, 1);
  lcd.print("WiFi Connect...");
  
  //Configura modo Station e desconecta conexões anteriores
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.print("SSID: ");
  Serial.println(WIFI_SSID);
  
  //Inicia conexão
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  //Aguarda conexão com timeout de 15 segundos
  int tentativas = 0;
  while (WiFi.status() != WL_CONNECTED && tentativas < 30) {
    delay(500);
    Serial.print(".");
    tentativas++;
  }
  Serial.println();
  
  //Verifica resultado da conexão
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    
    //Exibe no LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi OK");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
  } else {
    Serial.println("ERRO: WiFi nao conectou!");
    Serial.print("Status WiFi: ");
    Serial.println(WiFi.status());
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi ERRO");
    delay(5000);
  }
}

//Conecta ao broker MQTT com retry e publica status online
void connectMQTT() {
  Serial.println("\n=== Conectando MQTT ===");
  
  int tentativas = 0;
  while (!mqtt.connected() && tentativas < 5) {
    tentativas++;
    
    //Gera clienteId único (evita conflitos)
    String clientId = "AcesslyESP32-" + String(random(0xffff), HEX);
    Serial.print("Tentativa ");
    Serial.print(tentativas);
    Serial.print("/5 - Client ID: ");
    Serial.println(clientId);
    Serial.print("Conectando a ");
    Serial.print(MQTT_SERVER);
    Serial.print(":");
    Serial.print(MQTT_PORT);
    Serial.print("... ");
    
    //Tenta conexão
    if (mqtt.connect(clientId.c_str())) {
      Serial.println("CONECTADO!");
      
      // Publica status 'online' em tópico de status
      String topic = String(MQTT_TOPIC_BASE) + "/" + EMPRESA_ID + "/status";
      bool pub = mqtt.publish(topic.c_str(), "online");
      
      if (pub) {
        Serial.println("✓ Status 'online' publicado!");
      } else {
        Serial.println("✗ Falha ao publicar status");
      }
      
      Serial.println("========================\n");
      return;  //sai da função após conectar
      
    } else {
      Serial.println("FALHOU!");
      Serial.print("Código de erro: ");
      Serial.println(mqtt.state());
      
      if (tentativas < 5) {
        Serial.println("Aguardando 3s para tentar novamente...");
        delay(3000);
      }
    }
  }
  
  //Se não conseguiu conectar após 5 tentativas
  if (!mqtt.connected()) {
    Serial.println("✗✗✗ ERRO CRÍTICO: Não foi possível conectar ao MQTT!");
    Serial.println("Verifique:");
    Serial.println("  - Broker está online?");
    Serial.println("  - WiFi está funcionando?");
    Serial.println("========================\n");
  }
}

//Faz leitura de todos os sensores
void readSensors() {
  temperature = dht.getTemperature();       //temperatura ambiente
  humidity = dht.getHumidity();             //umidade relativa 
  lightLevel = analogRead(LDR_PIN);         //leitura do sensor de luz
  noiseLevel = analogRead(POT_PIN);         //leitura do ruído via potenciômetro
  obstructionDetected = (digitalRead(BUTTON_PIN) == LOW);   //botão pressionado indica obstrução
}

//Calcula o score geral de acessibilidade com base nos limites definidos
void calculateAccessibilityScore() {
  accessibilityScore = 100;        //começa em 100
  if (lightLevel < MIN_LIGHT) accessibilityScore -= 25;   //penaliza se pouca luz
  if (noiseLevel > MAX_NOISE) accessibilityScore -= 20;   //penaliza ruído alto
  if (temperature < MIN_TEMP || temperature > MAX_TEMP) accessibilityScore -= 15;   //penaliza conforto técnico
  if (obstructionDetected) accessibilityScore -= 40;      //penaliza fortemente se há obstrução
  if (accessibilityScore < 0) accessibilityScore = 0;     //não deixa passar de zero
}

//Atualiza LEDs (status visual) e mostra score/status no LCD
void updateIndicators() {
  if (accessibilityScore >= 80) {           //score bom: LED verde
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
  } else if (accessibilityScore >= 50) {    //score intermediário: LED amarelo
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, LOW);
  } else {                                  //score ruim: LED vermelho
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, HIGH);
  }

  //Atualiza display LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.printf("Score: %d%%", accessibilityScore);    //exibe % de score
  lcd.setCursor(0, 1);

  if (accessibilityScore >= 80) lcd.print("Status: OK");
  else if (accessibilityScore >= 50) lcd.print("Status: ALERTA");
  else lcd.print("Status: CRITICO");
}

//Monta mensagem em JSON e publica no tópico/data via MQTT
void publishData() {

  //Cria documento JSON com tamanho de 512 bytes
  StaticJsonDocument<512> doc;

  //Metadados
  doc["empresa_id"] = EMPRESA_ID;
  doc["timestamp"] = millis() / 1000;   //timestamp em segundos

  //Objetos "sensores" com leituras dos sensores
  JsonObject sensors = doc.createNestedObject("sensors");
  sensors["temperature"] = temperature;
  sensors["humidity"] = humidity;
  sensors["light_level"] = lightLevel;
  sensors["noise_level"] = noiseLevel;
  sensors["obstruction"] = obstructionDetected;

  //Objeto "acessibility" com score e status
  JsonObject accessibility = doc.createNestedObject("accessibility");
  accessibility["score"] = accessibilityScore;

  //Define status textual baseado no score
  accessibility["status"] = (accessibilityScore >= 80) ? "OK" : 
                            (accessibilityScore >= 50) ? "WARNING" : "CRITICAL";

  //Objeto "compliance" com verificação individual de cada critério
  JsonObject compliance = accessibility.createNestedObject("compliance");
  compliance["lighting"] = (lightLevel >= MIN_LIGHT);
  compliance["noise"] = (noiseLevel <= MAX_NOISE);
  compliance["temperature"] = (temperature >= MIN_TEMP && temperature <= MAX_TEMP);
  compliance["path_clear"] = !obstructionDetected;

  //Serializa JSON para string
  String payload;
  serializeJson(doc, payload);

  //Define tópico de publicação
  String topic = String(MQTT_TOPIC_BASE) + "/" + EMPRESA_ID + "/data";
  
  //Logs de debug
  Serial.println("\n--- PUBLICANDO MQTT ---");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(payload);
  
  //Publica no broker MQTT
  bool publicado = mqtt.publish(topic.c_str(), payload.c_str());
  
  //Verifica resultado da publicação
  if (publicado) {
    Serial.println("✓ Publicado com sucesso!");
  } else {
    Serial.println("✗ ERRO ao publicar!");
    Serial.print("Estado MQTT: ");
    Serial.println(mqtt.state());
  }
  Serial.println("----------------------\n");
}
