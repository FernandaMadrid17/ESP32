#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Configurar el sensor de humedad y temperatura DHT11 o DHT22
#define DHTPIN 15
#define DHTTYPE DHT22
DHT sensor(DHTPIN, DHTTYPE);

// Comunicar el access point virtual de la plataforma
const char* WIFI_SSID = "Wokwi-GUEST";
const char* WIFI_PASS = "";

const char* MQTT_CLIENT = "ESP32_Fersita"; // debe tener un nombre único
const char* MQTT_BROKER = "broker.hivemq.com";
const char* MQTT_USER = "";
const char* MQTT_PASSWORD = "";
const char* MQTT_TOPIC = "Clima_fersita";
const int MQTT_PORT = 1883;

float temp = 0;
float hum = 0;

WiFiClient espClient;
PubSubClient client(espClient);

String clima = "";

void conectarWiFi() {
  Serial.print("Conectando a la Red Wifi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println(" Conectado!");
}

void conectarMQTT() {
  Serial.print("Conectando al Broker MQTT...");
  client.setServer(MQTT_BROKER, MQTT_PORT);

  while (!client.connected()) {
    bool ok;
    if (strlen(MQTT_USER) > 0) {
      ok = client.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASSWORD);
    } else {
      ok = client.connect(MQTT_CLIENT);
    }
    if (ok) {
      Serial.println("conectado!");
    } else {
      Serial.print("Fallo, rc=");
      Serial.print(client.state());
      Serial.println(" Reintentando en 1s");
      delay(1000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  sensor.begin();      // "prendemos" el sensor de Hum/Tem
  conectarWiFi();       // llamamos los procedimientos de conexión wifi
  conectarMQTT();        // y comunicación MQTT
}

void loop() {
  if (!client.connected()) {
    conectarMQTT();
  }
  client.loop(); // mantenemos la comunicación activa

  Serial.print("Midiendo la Temperatura y Humedad... ");
  temp = sensor.readTemperature(); // leemos el sensor
  hum = sensor.readHumidity();

  // Verificamos que el sensor funcione
  if (isnan(temp) || isnan(hum)) {
    Serial.println("Error leyendo el sensor DHT22");
    delay(1000);
    return;
  }

  StaticJsonDocument<128> doc;
  doc["temp"] = temp;
  doc["hum"] = hum;

  char message[128];
  serializeJson(doc, message);
  String messageStr = String(message);

  // Solo enviamos el mensaje cuando hay cambios en los sensores
  if (messageStr != clima) {
    Serial.println("Actualizado!");
    Serial.print("Enviando el topic MQTT ");
    Serial.print(MQTT_TOPIC);
    Serial.print(": ");
    Serial.println(messageStr);
    client.publish(MQTT_TOPIC, message);
    clima = messageStr;
  } else {
    Serial.println("Sin cambios en el clima");
  }

  delay(1000); // Este delay se puede modificar
}