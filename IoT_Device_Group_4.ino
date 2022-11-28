#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <BH1750.h>
#include <Wire.h>

#ifndef WIFISSID
#define WIFISSID "Indihome@Tulip"
#define WIFIPASS "087775656333"
#endif

#ifndef INFLUXDB
#define INFLUXDB "103.226.138.13"
#define AUTHID "Upi_iotdev_4@UPI"
#define AUTHPASS "Upi12345"
#endif

// WiFi & MQTT //
const char* ssid = WIFISSID;
const char* password = WIFIPASS;
const char* mqttServer = INFLUXDB;
const char* mqttUser = AUTHID;
const char* mqttPassword = AUTHPASS;
const char* topic = "telemetry";
const int mqttPort = 1883;
unsigned long previousMillis = 0;
unsigned long interval = 30000;
WiFiClient espClient; 
PubSubClient client(espClient);

// Sensor //
BH1750 lightMeter;
DHT dht(14, DHT11);

void initWiFi()
  {
  // Inisialissi WiFi//
    Serial.println();
    Serial.print("Menghubungkan...");
    Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
    {
    delay(2000);
      Serial.print(".");
    }
    Serial.println();
    Serial.print("Terhubung, IP address: ");
    Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  }

void callback(char* topic, byte* payload, unsigned int length)
  {
  // callback //
    Serial.print("Data sampai ke topik: ");
    Serial.println(topic);
    Serial.print("Message: ");
    for (int i = 0; i , length; i++)
    {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
  }
  
void setup()
  {
  // Inisialisi Perintah //
    Serial.begin(115200);
      initWiFi();
      dht.begin();

  // Menghubungkan dan mengirim ke MQTT //
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected())
    {
      Serial.println("Menghubungkan dengan MQTT...");
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), mqttUser, mqttPassword))
        {
          Serial.println("Terhubung dengan MQTT!");
        }
        else
        {
          Serial.print("Gagal terhubung dengan MQTT, Status: ");
          Serial.println(client.state());
          Serial.println("Menghubungkan kembali dalam 5 detik");
            delay (5000);
        }
        client.subscribe(topic);
      }
      
  // Inisialisi sensor BH1750FVI //
    Wire.begin();
      lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
  }
  
void reconnect()
  {
  // Pemastian koneksi dengan MQTT //
    while (!client.connected())
    {
      Serial.println("Memastikan tetap terhubung dengan MQTT...");
      String clientId = "ESP8266Client-";
      clientId += String(random(0xffff), HEX);
      if (client.connect(clientId.c_str(), mqttUser, mqttPassword))
      {
        Serial.println("Terhubung dengan MQTT!");
      }
      else
      {
        Serial.print("Gagal terhubung dengan MQTT, Status: ");
        Serial.println(client.state());
        Serial.println("Menghubungkan kembali dalam 5 detik");
          delay (5000);
      }
        client.subscribe(topic);
    }
  }
    
void loop()
  {
  float temperature = dht.readTemperature();
    Serial.print("Kelembaban         : "); Serial.print(humidity); Serial.println(" %");
    Serial.print("Suhu/Temperature   : "); Serial.print(temperature); Serial.println(" °C");
    Serial.print("Intensitas Cahaya  : "); Serial.print(lightIntensity); Serial.println(" Lux");
      if (isnan(temperature) || isnan(humidity) || isnan(lightIntensity)){
    Serial.println(F("Gagal membaca data dari Sensor"));
      return;
    }
    else
    {
      
  // Serialisasi JSON dan Pengiriman ke Platform //
    DynamicJsonDocument doc(1024);
      JsonObject obj=doc.as<JsonObject>();
      doc["temp"] = temperature;
      doc["lux"] = lightIntensity;
        char jsonStr[60];
          serializeJson(doc,jsonStr);
      Serial.print(jsonStr);
      Serial.println();   
      Serial.print("\n");
        client.publish("telemetry",jsonStr); 
          delay(600000);
    }
  }
  }
