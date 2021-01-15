#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

#define REST_PORT 8080

String ssid = WiFi.macAddress() + "_network"; //unique network per esp
String password = "espdefault";

ESP8266WebServer server(REST_PORT);

void setup()
{
    Serial.begin(115200);
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFi.softAP(ssid, password);
    // Serial.print("Connecting");
    // while (WiFi.status() != WL_CONNECTED)
    // {
    //     delay(500);
    //     Serial.print(".");
    // }
    // Serial.println("");
    Serial.print("IP: ");
    Serial.print(WiFi.softAPIP());
    routing();
    server.begin();
}

void loop()
{
    server.handleClient();
}

void routing()
{
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", "ESP8266 REST Server\n");
    });
    server.on("/rgb", HTTP_POST, []() {
        if (server.hasArg("plain") == false)
        {
            server.send(400, "text/plain", "Body not received\n");
            return;
        }

        StaticJsonDocument<64> doc;

        String body = server.arg("plain");
        deserializeJson(doc, body);
        Serial.println(body);
        uint8 r = doc["r"];
        uint8 g = doc["g"];
        uint8 b = doc["b"];
        Serial.println((String) "r:" + r + ", g:" + g + ", b:" + b);
        server.send(200, "text/plain", "RGB successfully posted\n");
    });
}