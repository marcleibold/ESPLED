// Webserver

// LED
#include <Adafruit_NeoPixel.h>

uint8 defaultLEDPin = 12; // D6 - GPIO12
uint16 defaultLEDCount = 10; // Later configured via /configure route

uint16 ledCount = defaultLEDCount;
uint8 ledPin = defaultLEDPin;

Adafruit_NeoPixel strip(defaultLEDCount, defaultLEDPin);

void setup()
{
    Serial.begin(115200);

    strip.begin();
    strip.show(); // initialize to off
}

void loop()
{
}

// void routing()
// {
//     server.on("/rgb", HTTP_POST, []() {
//         if (server.hasArg("plain") == false) {
//             server.send(400);
//             return;
//         }

//         StaticJsonDocument<81> doc;

//         String body = server.arg("plain");
//         deserializeJson(doc, body);
//         uint8 r = doc["r"];
//         uint8 g = doc["g"];
//         uint8 b = doc["b"];
//         uint8 brightness = doc["brightness"];
//         changeColor(r, g, b, brightness);
//         server.send(200);
//     });

//     server.on("/configure", HTTP_PUT, []() {
//         if (server.hasArg("plain") == false) {
//             server.send(400);
//             return;
//         }

//         StaticJsonDocument<48> doc;

//         String body = server.arg("plain");
//         deserializeJson(doc, body);

//         ledCount = doc["ledCount"];
//         ledPin = doc["ledPin"];

//         if (ledCount > 0 && ledCount != defaultLEDCount) {
//             strip.updateLength(ledCount);
//         }
//         if (ledPin > 0 && ledPin != defaultLEDPin) {
//             strip.setPin(ledPin);
//         }
//         server.send(201);
//     });
// }

void changeColor(uint8 r, uint8 g, uint8 b, uint8 brightness)
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