#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

#define REST_PORT 8080
#define MAX_RECONNECT 20

String defaultSSID = WiFi.macAddress() + "_network"; // unique network per esp
String defaultPassword = "espdefault";

ESP8266WebServer server(REST_PORT);

void setup()
{
    Serial.begin(115200);
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFi.softAP(defaultSSID, defaultPassword);
    WiFi.begin();
    Serial.print("Connecting");
    uint8 retries = 0;
    while ((WiFi.status() != WL_CONNECTED) && retries < MAX_RECONNECT) {
        delay(500);
        Serial.print(".");
        retries++;
    }
    Serial.println("");
    if (!(WiFi.localIP().toString() == NULL)) {
        Serial.println("WiFi IP: " + WiFi.localIP().toString());
    }
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());
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

    server.on("/connect", HTTP_PUT, []() {
        if (server.hasArg("plain") == false) {
            server.send(400);
            return;
        }

        StaticJsonDocument<139> doc;
        String body = server.arg("plain");
        Serial.println(body);
        deserializeJson(doc, body);
        String ssid = doc["ssid"];
        String pass = doc["pass"];
        Serial.println(ssid);
        Serial.println(pass);
        if (ssid == NULL && pass == NULL) {
            server.send(400);
            return;
        }
        Serial.println("In Progress");
        // WiFi.softAPdisconnect(true);
        WiFi.begin(ssid, pass);
        uint8 retries = 0;
        while ((WiFi.status() != WL_CONNECTED) && retries < MAX_RECONNECT) { //go back to AP Mode
            delay(500);
            Serial.print(".");
            retries++;
        }
        Serial.println("");
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Connection failed, reverting");
            server.send(400);
            WiFi.disconnect();
            Serial.println("Back to normal.");
        } else {
            Serial.print("Connected to: ");
            Serial.println(ssid);
            server.send(201, "text/plain", "Connected to " + ssid + " as " + WiFi.localIP().toString() + "\n");
            Serial.println("Done.");
        }
    });

    server.on("/rgb", HTTP_POST, []() {
        if (server.hasArg("plain") == false) {
            server.send(400);
            return;
        }

        StaticJsonDocument<64> doc;

        String body = server.arg("plain");
        deserializeJson(doc, body);
        Serial.println(body);
        uint8 r = doc["r"];
        uint8 g = doc["g"];
        uint8 b = doc["b"];
        changeColor(r, g, b);
        server.send(200);
    });
}

void changeColor(uint8 r, uint8 g, uint8 b)
{
    if (r == 0 && g == 0 && b == 0) {
        //off
    } else {
    }
}