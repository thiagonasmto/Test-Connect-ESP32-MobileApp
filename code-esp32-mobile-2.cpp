#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Define o SSID e a senha da rede Wi-Fi
const char* ssid = "nome_da_rede";
const char* password = "senha_da_rede";

// Define o endereço IP do broker MQTT e a porta
const char* mqtt_server = "endereco_do_broker";
const int mqtt_port = 1883;

// Define o tópico MQTT para envio dos dados
const char* mqtt_topic = "topico/sensor";

// Define o pino do sensor DHT11
#define DHTPIN 27

// Define o tipo de sensor (DHT11 ou DHT22)
#define DHTTYPE DHT11

// Inicializa o objeto do sensor DHT
DHT dht(DHTPIN, DHTTYPE);

// Inicializa o objeto do cliente Wi-Fi e do cliente MQTT
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);

  // Conecta na rede Wi-Fi
  Serial.println();
  Serial.print("Conectando na rede ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado na rede Wi-Fi");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Trata as mensagens recebidas no tópico MQTT
}

void reconnect() {
  // Loop até que o cliente MQTT esteja conectado
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado ao broker MQTT");
      // Inscreve no tópico MQTT para receber mensagens
      client.subscribe("topico/acao");
    } else {
      Serial.print("Falha na conexão (");
      Serial.print(client.state());
      Serial.println(") - tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);

  // Conecta na rede Wi-Fi
  setup_wifi();

  // Configura o servidor MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Inicializa o sensor DHT
  dht.begin();
}

void loop() {
  // Verifica se o cliente MQTT está conectado
  if (!client.connected()) {
    reconnect();
  }
  // Envia os dados do sensor para o broker MQTT
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  String msg = "{\"temperatura\": " + String(temperature) + ", \"umidade\": " + String(humidity) + "}";
  char msgBuffer[50];
  msg.toCharArray(msgBuffer, 50);

  client.publish(mqtt
