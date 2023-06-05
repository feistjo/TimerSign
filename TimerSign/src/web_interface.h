#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <WiFi.h>

#include "timer.h"

class WebInterface
{
public:
    WebInterface(char *ssid, char *password, Timer &timer) : kSsid{ssid}, kPassword{password}, timer_{timer} {}

    void Initialize()
    {
        WiFi.softAP(kSsid, kPassword);

        initSPIFFS();
        initWebSocket();
        initWebServer();
    }

    void notifyClients()
    {
        static StaticJsonDocument<10000> doc;
        doc["remaining_time"] = timer_.GetTimeRemaining();
        doc["timer"] = timer_.GetTime();

        static char data[10000];
        size_t len = serializeJson(doc, data);
        ws.textAll(data, len);
    }

private:
    const char *kSsid;
    const char *kPassword;

    Timer &timer_;

    AsyncWebServer server{80};
    AsyncWebSocket ws{"/ws"};

    void initWebServer()
    {
        server.on("/", [this](AsyncWebServerRequest *request) { this->onRootRequest(request); });
        server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
        server.begin();
    }

    void initSPIFFS()
    {
        if (!SPIFFS.begin())
        {
            Serial.println("Cannot mount SPIFFS volume...");
            while (1)
            {
            }
        }
    }

    void onRootRequest(AsyncWebServerRequest *request) { request->send(SPIFFS, "/index.html", "text/html"); }

    void initWebSocket()
    {
        ws.onEvent([this](AsyncWebSocket *server,
                          AsyncWebSocketClient *client,
                          AwsEventType type,
                          void *arg,
                          uint8_t *data,
                          size_t len) { this->onEvent(server, client, type, arg, data, len); });
        server.addHandler(&ws);
    }

    void onEvent(
        AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
    {
        switch (type)
        {
            case WS_EVT_CONNECT:
                Serial.printf(
                    "WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
                break;
            case WS_EVT_DISCONNECT:
                Serial.printf("WebSocket client #%u disconnected\n", client->id());
                break;
            case WS_EVT_DATA:
                handleWebSocketMessage(arg, data, len);
            case WS_EVT_PONG:
            case WS_EVT_ERROR:
                break;
        }
    }

    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
        {
            const uint8_t size = JSON_OBJECT_SIZE(10000);
            StaticJsonDocument<size> json;
            DeserializationError err = deserializeJson(json, data);
            if (err)
            {
                Serial.print(F("deserializeJson() failed with code "));
                Serial.println(err.c_str());
                return;
            }

            if (json.containsKey("new_time"))
            {
                timer_.SetTime(json["new_time"]);
                timer_.Reset();
            }
            else
            {
                timer_.Reset();
            }
        }
    }
};
