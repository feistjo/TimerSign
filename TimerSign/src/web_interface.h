#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "timer.h"
#include "wifi_auth.h"

const char *test_root_ca =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIEsTCCA5mgAwIBAgIQCKWiRs1LXIyD1wK0u6tTSTANBgkqhkiG9w0BAQsFADBh\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
    "QTAeFw0xNzExMDYxMjIzMzNaFw0yNzExMDYxMjIzMzNaMF4xCzAJBgNVBAYTAlVT\n"
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
    "b20xHTAbBgNVBAMTFFJhcGlkU1NMIFJTQSBDQSAyMDE4MIIBIjANBgkqhkiG9w0B\n"
    "AQEFAAOCAQ8AMIIBCgKCAQEA5S2oihEo9nnpezoziDtx4WWLLCll/e0t1EYemE5n\n"
    "+MgP5viaHLy+VpHP+ndX5D18INIuuAV8wFq26KF5U0WNIZiQp6mLtIWjUeWDPA28\n"
    "OeyhTlj9TLk2beytbtFU6ypbpWUltmvY5V8ngspC7nFRNCjpfnDED2kRyJzO8yoK\n"
    "MFz4J4JE8N7NA1uJwUEFMUvHLs0scLoPZkKcewIRm1RV2AxmFQxJkdf7YN9Pckki\n"
    "f2Xgm3b48BZn0zf0qXsSeGu84ua9gwzjzI7tbTBjayTpT+/XpWuBVv6fvarI6bik\n"
    "KB859OSGQuw73XXgeuFwEPHTIRoUtkzu3/EQ+LtwznkkdQIDAQABo4IBZjCCAWIw\n"
    "HQYDVR0OBBYEFFPKF1n8a8ADIS8aruSqqByCVtp1MB8GA1UdIwQYMBaAFAPeUDVW\n"
    "0Uy7ZvCj4hsbw5eyPdFVMA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEF\n"
    "BQcDAQYIKwYBBQUHAwIwEgYDVR0TAQH/BAgwBgEB/wIBADA0BggrBgEFBQcBAQQo\n"
    "MCYwJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBCBgNVHR8E\n"
    "OzA5MDegNaAzhjFodHRwOi8vY3JsMy5kaWdpY2VydC5jb20vRGlnaUNlcnRHbG9i\n"
    "YWxSb290Q0EuY3JsMGMGA1UdIARcMFowNwYJYIZIAYb9bAECMCowKAYIKwYBBQUH\n"
    "AgEWHGh0dHBzOi8vd3d3LmRpZ2ljZXJ0LmNvbS9DUFMwCwYJYIZIAYb9bAEBMAgG\n"
    "BmeBDAECATAIBgZngQwBAgIwDQYJKoZIhvcNAQELBQADggEBAH4jx/LKNW5ZklFc\n"
    "YWs8Ejbm0nyzKeZC2KOVYR7P8gevKyslWm4Xo4BSzKr235FsJ4aFt6yAiv1eY0tZ\n"
    "/ZN18bOGSGStoEc/JE4ocIzr8P5Mg11kRYHbmgYnr1Rxeki5mSeb39DGxTpJD4kG\n"
    "hs5lXNoo4conUiiJwKaqH7vh2baryd8pMISag83JUqyVGc2tWPpO0329/CWq2kry\n"
    "qv66OSMjwulUz0dXf4OHQasR7CNfIr+4KScc6ABlQ5RDF86PGeE6kdwSQkFiB/cQ\n"
    "ysNyq0jEDQTkfa2pjmuWtMCNbBnhFXBYejfubIhaUbEv2FOQB3dCav+FPg5eEveX\n"
    "TVyMnGo=\n"
    "-----END CERTIFICATE-----\n";

class WebInterface
{
public:
    WebInterface(/* char *ssid, char *password,  */ Timer &timer)
        : /* kSsid{ssid}, kPassword{password}, */ timer_{timer}
    {
    }

    void Initialize()
    {
        // WiFi.softAP(kSsid, kPassword);

        WiFi.disconnect(true);  // disconnect form wifi to set new wifi connection
        WiFi.mode(WIFI_STA);    // init wifi mode
        /* esp_wifi_set_ps(WIFI_PS_NONE);
        /* esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)username,
                                           strlen(username));    */                    // provide identity
        /* esp_wifi_sta_wpa2_ent_set_username((uint8_t *)username, strlen(username));  // provide username
        esp_wifi_sta_wpa2_ent_set_password((uint8_t *)password, strlen(password));  // provide password
        esp_wifi_sta_wpa2_ent_enable();
        WiFi.enableSTA(true);
        WiFi.begin(ssid);  // connect to wifi  */
        WiFi.begin(ssid, WPA2_AUTH_PEAP, anonymous_identity, username, password);
        uint8_t counter = 0;
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
            counter++;
            if (counter >= 60)
            {  // after 30 seconds timeout - reset board (on unsucessful connection)
                ESP.restart();
            }
        }
        // client_.setCACert(test_root_ca);
        //  client.setCertificate(test_client_cert); // for client verification - certificate
        //  client.setPrivateKey(test_client_key);  // for client verification - private key
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address set: ");
        Serial.println(WiFi.localIP());  // print LAN IP

        initSPIFFS();
        initWebServer();
        initWebSocket();
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

    void MaintainConnection()
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            // Serial.println("Connected to WiFi.");
            return;
        }
        if (WiFi.status() != WL_CONNECTED)
        {  // if we lost connection, retry
            Serial.println("Disconnected from WiFi. Reconnecting...");
            WiFi.begin(ssid, WPA2_AUTH_PEAP, anonymous_identity, username, password);
        }
        uint8_t counter = 0;
        while (WiFi.status() != WL_CONNECTED)
        {  // during lost connection, print dots
            delay(500);
            Serial.print(".");
            counter++;
            if (counter >= 60)
            {  // 30 seconds timeout - reset board
                ESP.restart();
            }
        }
    }

private:
    /* const char *kSsid;
    const char *kPassword; */

    Timer &timer_;

    WiFiClientSecure client_;

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
            const uint8_t size = JSON_OBJECT_SIZE(1000);
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
