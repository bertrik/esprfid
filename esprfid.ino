#include <stdint.h>

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiManager.h>
#include <PubSubClient.h>

#include <MFRC522.h>

// TODO give these a meaningful value
#define PIN_MFRC_SS  D8
#define PIN_MFRC_RST D3

static WiFiManager wifiManager;
static WiFiClient wifiClient;
static MFRC522 mfrc522(PIN_MFRC_SS, PIN_MFRC_RST);

void setup(void)
{
    // initialize serial port
    Serial.begin(115200);
    Serial.println("RFID reader\n");

    // initialise card reader
    SPI.begin();
    mfrc522.PCD_Init();

    // connect to wifi
    Serial.println("Starting WIFI manager ...");
    wifiManager.autoConnect("ESP-RFID");
}

static int post_uid(const char *url, int len, const unsigned char *uid)
{
    // create UID string
    char payload[128];
    strcpy(payload, "");
    for (int i = 0; i < len; i++) {
        char buf[8];
        sprintf(buf, "%02X", uid[i]);
        strcat(payload, buf);
    }
    
    // send it over HTTP
    HTTPClient http;
    http.begin(url);
    int result = http.POST((uint8_t *)payload, strlen(payload));
    http.end();
    
    return result;
}

void loop(void)
{
    if (mfrc522.PICC_IsNewCardPresent()) {
        if (mfrc522.PICC_ReadCardSerial()) {
            Serial.println("Detected card!");
            // send the UID
            int result = post_uid("http://server.com/api", mfrc522.uid.size, mfrc522.uid.uidByte);
            Serial.print("Result: ");
            Serial.println(result, DEC);
        }
    }
}

