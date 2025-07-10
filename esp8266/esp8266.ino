#include <ESP8266WiFi.h>
#include <ESP8266Ping.h>
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

bool is_device_alive(char *ip_address);

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
        if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password, nullptr, 0, false, nullptr, clean_session)) {
            mqtt_client.subscribe(WOL_TOPIC, QoS);
            mqtt_client.subscribe(STATE_SUB_TOPIC, QoS);
        } else {
            char err_buf[128];
            espClient.getLastSSLError(err_buf, sizeof(err_buf));
            delay(5000);
        }
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
    char *payload_chr = new char[length+1];
    for (int i = 0; i < length; i++) {
        payload_chr[i] = (char) payload[i];
    }
    payload_chr[length] = '\0';

    if (strcmp(topic, WOL_TOPIC) == 0) {
        WOL.sendMagicPacket(payload_chr, WOL_PORT);
    }
    else if (strcmp(topic, STATE_SUB_TOPIC) == 0) {
        char msg[100];

        char *err = nullptr;
        if (is_device_alive(payload_chr, &err)) {
            snprintf(msg, sizeof(msg), "%s: [alive]", payload_chr);
        }
        else if (err == nullptr) {
            snprintf(msg, sizeof(msg), "%s: [dead]", payload_chr);
        }
        else {
            snprintf(msg, sizeof(msg), "%s: [error] - %s", payload_chr, err);
            delete[] err;
        }

        mqtt_client.publish(STATE_PUB_TOPIC, msg);
    }

    delete[] payload_chr;
}

bool is_device_alive(char *ip_address, char **err) {
    IPAddress ip;
    if (ip.fromString(ip_address)) {
        return Ping.ping(ip);
    }
    *err = new char[100];
    snprintf(*err, 100, "Invalid IP address: %s", ip_address);
    return false;
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
