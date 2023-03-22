#include <WiFi.h>
#include <PubSubClient.h>

// Configurações da rede WiFi
const char* ssid = "nome_da_rede_wifi";
const char* password = "senha_da_rede_wifi";

// Configurações do broker MQTT
const char* mqtt_server = "endereço_do_servidor_mqtt";
const int mqtt_port = 1883;
const char* mqtt_user = "usuário_do_servidor_mqtt";
const char* mqtt_password = "senha_do_servidor_mqtt";
const char* mqtt_topic = "tópico_para_publicação_e_inscrição";

// Instanciação do objeto WiFiClient
WiFiClient wifiClient;

// Instanciação do objeto PubSubClient
PubSubClient mqttClient(wifiClient);

void setup() {
  // Inicialização da comunicação serial
  Serial.begin(9600);

  // Conexão à rede WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Conexão ao broker MQTT
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(callback);
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32Client", mqtt_user, mqtt_password )) {
      Serial.println("Connected to MQTT broker");
      mqttClient.subscribe(mqtt_topic);
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  // Leitura de algum sensor conectado ao ESP32
  int sensorValue = analogRead(A0);

  // Publicação dos dados no tópico MQTT
  mqttClient.publish(mqtt_topic, String(sensorValue).c_str());

  // Aguarda por 1 segundo
  delay(1000);
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Recebe os dados enviados pelo aplicativo móvel
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("Message received: " + message);
}
