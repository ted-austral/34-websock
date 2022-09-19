/*
 *  Project 34-websock - main.cpp
 *      Show that using WebSockets is better than using normal Web servers
 *      in near real time systems
 *
 *  Austral - IntroComp - 2022 - EAM
 */

#include <Arduino.h>


#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

#include "wifi_ruts.h"
#include "webpage.h"

/*
 *  Constants taken from platformio.ini
 *      LED         LED GPIO
 *      SERIAL_BAUD Serial Port Baud Rate
 */

// constants setting PWM properties

static const int freq = 5000;
static const int ledChannel = 0;
static const int resolution = 12;

//  Object creation

WebServer server;
WebSocketsServer webSocket = WebSocketsServer(81);

/*
 *  Private functions
 */

/*
 *  webSocketEvent:
 *      Events Callback
 */

static void
webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    uint16_t brightness;
    size_t i;

    if (type == WStype_TEXT)
    {
        if (payload[0] == '#')
        {
            brightness = (uint16_t)strtol((const char *)&payload[1], NULL, 10);
            ledcWrite(ledChannel, brightness);
            Serial.print("brightness= ");
            Serial.println(brightness);
        }
        else
        {
            for (i = 0; i < length; i++)
                Serial.print((char)payload[i]);
            Serial.println();
        }
    }
}

/*
 *  Public functions
 */

void
setup(void)
{
    Serial.begin(SERIAL_BAUD);

    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(LED, ledChannel);

    wifi_connect();

    server.on("/", []() { server.send_P(200, "text/html", webpage); });
    server.begin();
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void
loop(void)
{
    char c[1];

    webSocket.loop();
    server.handleClient();
    if (Serial.available() > 0)
    {
        c[0] = Serial.read();
        webSocket.broadcastTXT(c, sizeof(c));
    }
}


