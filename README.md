# WoL_with_ESP8266_and_MQTT

**WoL_with_ESP8266_and_MQTT** is a small project designed to <u>completely remotely</u> **power on** and **check the status** of a computer on the local network using an ESP8266 microcontroller and the MQTT protocol.

---

## 🎯 Project Goal

The goal of this project is to enable:

- **Remote wake-up** of a PC via Wake-on-LAN (WoL),
- **Status checking** (whether the PC is online or offline),
- Managing these actions remotely via a simple command-line interface on a remote machine.

---

## 🛠️ How It Works

### ESP8266:

- Connects to Wi-Fi and a secure MQTT broker using TLS *(MQTT broker is served on emqx.com)*.
- Subscribes to two MQTT topics:
  - `home/esp8266/wol` – for receiving Wake-on-LAN commands (sends a WoL magic packet),
  - `home/esp8266/dev-state` – for checking the device state (pings a given IP).
- Publishes the result of the status check to a separate MQTT topic (`remote/dev-state`).

### Remote Machine (Python client):

- Connects to the same MQTT broker over TLS.
- Allows the user to:
  - Send a WoL packet (based on MAC address) which turns on the device,
  - Check if a computer/server inside LAN is online (based on IP address).
- Displays the server response received via MQTT.

---

## 🔒 Security

This project uses:
- TLS/SSL encrypted connections,
- CA certificate validation for the MQTT broker,
- Username and password authentication.
- Topic authorization for users (on emqx platform)

---

## 💡 Example Usage

1. Start the ESP8266 – it connects to Wi-Fi and the MQTT broker.
2. Run the Python script on your computer.
3. Choose an option from the CLI menu:
   - `poweron` – sends a WoL signal to the server,
   - `state` – checks whether the server is online,
   - `quit` – exits the program.

---

## ⚙️ Requirements

- ESP8266 with the following libraries:
  - `ESP8266WiFi`
  - `ESP8266Ping`
  - `PubSubClient`
  - `WiFiUdp`
  - `WakeOnLan`
  - `BearSSL`
- A TLS-enabled MQTT broker (e.g., Mosquitto with SSL)
- Python 3 with `paho-mqtt`
- MQTT CA certificate
- Wake-on-LAN enabled in BIOS and OS of the target machine

---

## 📁 Project Structure

```
WoL_with_ESP8266_and_MQTT/
├── esp8266/           # ESP8266 source code
│   ├── esp8266.ino    # main code
│   ├── config.h       # Configuration
│   └── ca_cert.h      # CA certificate
├── remote_client/     # Python client
│   ├── main.py        # main code
|   ├── .env           # Configuration (not versioned)
|   └── emqxsl-ca.crt  # CA certificate
└── README.md
```

---

## 🔐 .env File Example (inside remote_client)

The `.env` file should contain all sensitive and environment-specific configuration:

```
MQTT_BROKER=your.broker.address
MQTT_PORT=8883
MQTT_USERNAME=your_username
MQTT_PASSWORD=your_password
CA_CERT=./your_ca_cert.crt
MQTT_SUB_TOPIC=remote/dev-state
MQTT_STATE_PUB_TOPIC=home/esp8266/dev-state
MQTT_WOL_PUB_TOPIC=home/esp8266/wol
MAC_ADDRESS=XX:XX:XX:XX:XX:XX
IP_ADDRESS=192.168.0.X
QOS=1
CLEAN_SESSION=1
```

---


## 🧠 Author & Motivation

This is a hobby/educational project created to simplify self-hosted server management.

---
