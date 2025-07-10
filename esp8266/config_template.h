// CHANGE NAME TO config.h

#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials
const char *ssid = "WIFI_SSID";              // Replace with your WiFi name
const char *password = "WIFI_PASSWORD";   // Replace with your WiFi password

// MQTT Broker settings
const int mqtt_port = 8883;  // MQTT port (TLS)
const char *mqtt_broker = "broker.emqx.io";  // EMQX broker endpoint
const char *mqtt_username = "emqx";  // MQTT username for authentication
const char *mqtt_password = "public";  // MQTT password for authentication
const int QoS = 1;
const bool clean_session = true;

// NTP Server settings
const char *ntp_server = "pool.ntp.org";     // Default NTP server
const long gmt_offset_sec = 0;            // GMT offset in seconds (adjust for your time zone)
const int daylight_offset_sec = 0;        // Daylight saving time offset in seconds

const char *WOL_TOPIC = "testtopic/#";
const int WOL_PORT = 9;

#endif
