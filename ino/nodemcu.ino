#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

/* LED Button code */
int buttonState = 0;      // variable for reading the pushbutton state
bool clicked = false;   // pushbutton clicked 

const int buttonPin = D1; // the number of the pushbutton pin
int speakerPin = D8;
const int ledPins[6] = {D2, D3, D4, D5, D6, D7};    // leds connected
const int ledPinsSize = 6;  // save the number cause there is no direct way to get the length in C

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.printf("[WSc] Disconnected!\n");
        break;
    case WStype_CONNECTED:
        Serial.printf("[WSc] Connected to url: %s\n", payload);
        // send message to server when Connected
        webSocket.sendTXT("Connected-NodeMCU");
        break;
    case WStype_TEXT:

        Serial.printf("[WSc] get text: %s\n", payload);
        clicked = false;

        char *msg = ((char *) payload);
        int position = *msg - '0';  // convert char to int
        int ledPin = ledPins[position]; // which desk summond Ravi

        if (position > 8) {
            break;
        }

        analogWrite (speakerPin, 255);
        delay (200);
        analogWrite (speakerPin, 0);
        delay (50);
        analogWrite (speakerPin, 255);
        delay (150);
        analogWrite (speakerPin, 0);
        delay (50);
        analogWrite (speakerPin, 255);
        delay (100);
        analogWrite (speakerPin, 0);
        delay (100);

        digitalWrite(ledPin, HIGH);
        delay(300);                // wait for a second
        digitalWrite(ledPin, LOW); // turn the LED off by making the voltage LOW
        delay(300);

        digitalWrite(ledPin, HIGH);
        delay(3000);                // wait for a second
        digitalWrite(ledPin, LOW); // turn the LED off by making the voltage LOW
        delay(300);

        break;
    case WStype_BIN:
        Serial.printf("[WSc] get binary length: %u\n", length);
        hexdump(payload, length);

        // send data to server
        // webSocket.sendBIN(payload, length);
        break;
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    pinMode(buttonPin, INPUT);
    pinMode (speakerPin, OUTPUT);

    for (int i = 0; i < ledPinsSize; i++) {
        pinMode(ledPins[i], OUTPUT);        
    }
    
    Serial.println();
    Serial.println();
    Serial.println();

    for (uint8_t t = 4; t > 0; t--)
    {
        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    WiFiMulti.addAP("WIFI_SSID", "WIFI_PASSWORD");

    while (WiFiMulti.run() != WL_CONNECTED)
    {
        delay(100);
    }

    webSocket.begin("192.168.100.69", 3000);
    webSocket.onEvent(webSocketEvent);
}

void loop()
{
    webSocket.loop();
    buttonState = digitalRead(buttonPin);

    // check if the pushbutton is pressed.
    // if it is, the buttonState is HIGH:
    if (buttonState == HIGH)
    {
        if (!clicked)
        {
            webSocket.sendTXT("resloved");  // Ravi is comming
            clicked = true;
        }
    }
    else
    {
        // do nothing
    }
}
