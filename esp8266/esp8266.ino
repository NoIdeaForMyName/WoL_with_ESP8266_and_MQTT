#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include <time.h>
#include <string.h>

#include "ca_cert.h"
#include "config.h"


// WiFi and MQTT client initialization
BearSSL::WiFiClientSecure espClient;
PubSubClient mqtt_client(espClient);

WiFiUDP UDP;
WakeOnLan WOL(UDP);

// Function declarations
void connectToWiFi();

void connectToMQTT();

void syncTime();

void mqttCallback(char *topic, byte *payload, unsigned int length);

void setup() {
    connectToWiFi();
    syncTime();  // X.509 validation requires synchronization time
    mqtt_client.setServer(mqtt_broker, mqtt_port);
    mqtt_client.setCallback(mqttCallback);
    connectToMQTT();

    WOL.setRepeat(3, 100);
    WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask()); 
}

void connectToWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
    }
}

void syncTime() {
    configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);
    while (time(nullptr) < 8 * 3600 * 2) {
        delay(1000);
    }
    struct tm timeinfo;
}

void connectToMQTT() {
    BearSSL::X509List serverTrustedCA(ca_cert);
    espClient.setTrustAnchors(&serverTrustedCA);
    while (!mqtt_client.connected()) {
        String client_id = "esp8266-client-" + String(WiFi.macAddress());
        if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            mqtt_client.subscribe(WOL_TOPIC);
        } else {
            char err_buf[128];
            espClient.getLastSSLError(err_buf, sizeof(err_buf));
            delay(5000);
        }
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
    if (strcmp(topic, WOL_TOPIC) == 0) {
        char *mac_address = new char[length+1];
        for (int i = 0; i < length; i++) {
            mac_address[i] = (char) payload[i];
        }
        mac_address[length] = '\0';

        WOL.sendMagicPacket(mac_address, WOL_PORT);
        delete[] mac_address;
    }
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        connectToWiFi();
        syncTime();
    }

    if (!mqtt_client.connected()) {
        connectToMQTT();
    }

    mqtt_client.loop();
}
