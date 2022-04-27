#include "./config.h"
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

uint8_t defaultLEDPin = 12; // D6 - GPIO12
uint16_t defaultLEDCount = 10; // Later configured via led/config topic
char* client_name = "bett";
uint16_t ledCount = defaultLEDCount;
uint8_t ledPin = defaultLEDPin;
char* mqtt_prefix = strcat("led/", client_name);
char* mqtt_rgb_topic = strcat(mqtt_prefix, "/rgb"); // led/name/rgb
char* mqtt_config_topic = strcat(mqtt_prefix, "/config"); // led/name/config
char* mqtt_connect_topic = "led/connect";

Adafruit_NeoPixel strip(defaultLEDCount, defaultLEDPin);
WiFiClient espClient;
PubSubClient client(espClient);
void setup()
{
    Serial.begin(115200);

    strip.begin();
    strip.show(); // initialize to off

    // WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    //connecting to the MQTT broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "ESP8266Client-";
        client_id += client_name;
        Serial.print("Attempting MQTT connection...");
        if (client.connect(client_id.c_str())) {
            Serial.println("connected");
            client.subscribe(mqtt_rgb_topic);
            client.subscribe(mqtt_config_topic);
            client.publish(mqtt_connect_topic, client_name);
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void configCallback(byte* payload)
{
    StaticJsonDocument<32> doc;
    DeserializationError error = deserializeJson(doc, (char*)payload);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }

    if (doc.containsKey("ledCount")) {
        ledCount = doc["ledCount"];
    }
    if (doc.containsKey("ledPin")) {
        ledPin = doc["ledPin"];
    }

    strip.show();
}

void rgbCallback(byte* payload)
{
    StaticJsonDocument<64> doc;
    DeserializationError error = deserializeJson(doc, (char*)payload);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }
    changeColor(doc["r"], doc["g"], doc["b"], doc["a"]);
}

void callback(char* topic, byte* payload, unsigned int length)
{
    if (strcmp(topic, mqtt_rgb_topic) == 0) {
        Serial.println("rgb");
        rgbCallback(payload);
    } else if (strcmp(topic, mqtt_config_topic) == 0) {
        Serial.println("configCallback");
        configCallback(payload);
    }
}

void loop()
{
    client.loop();
}

void changeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness)
{
    if ((r == 0 && g == 0 && b == 0) || brightness == 0) {
        strip.clear(); // off
        strip.show();
    } else {
        uint32 color = strip.Color(r, g, b);
        strip.fill(color, 0, strip.numPixels());
        strip.setBrightness(brightness);
        strip.show();
    }
}