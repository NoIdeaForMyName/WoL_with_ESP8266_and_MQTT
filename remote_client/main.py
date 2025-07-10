import paho.mqtt.client as mqtt
import os
from dotenv import load_dotenv
import ssl

load_dotenv()

MQTT_BROKER = os.getenv("MQTT_BROKER")
MQTT_PORT = int(os.getenv("MQTT_PORT"))
MQTT_SUB_TOPIC = os.getenv("MQTT_SUB_TOPIC")
MQTT_PUB_TOPIC = os.getenv("MQTT_PUB_TOPIC")
MQTT_USERNAME = os.getenv("MQTT_USERNAME")
MQTT_PASSWORD = os.getenv("MQTT_PASSWORD")
CA_CERT = os.getenv("CA_CERT")
QOS_PUB = int(os.getenv("QOS_PUB"))
QOS_SUB = int(os.getenv("QOS_SUB"))
CLEAN_SESSION = bool(os.getenv("CLEAN_SESSION"))

MAC_ADDRESS = os.getenv("MAC_ADDRESS")

server_state = "unknown"


# Callback: on connect
def on_connect(client: mqtt.Client, userdata, flags, rc, properties):
    print("Connected with result code: " + str(rc))
    client.subscribe(MQTT_SUB_TOPIC, qos=QOS_SUB)

# Callback: on message
def on_message(client, userdata, msg):
    global server_state
    if msg.topic == MQTT_SUB_TOPIC:
        server_state = msg.payload.decode()
    print_server_state()

def print_server_state():
    print("Server state:", server_state)

def send_wol_signal(client: mqtt.Client):
    client.publish(MQTT_PUB_TOPIC, MAC_ADDRESS, QOS_PUB)

def main():
    # Client initialization
    client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2, clean_session=CLEAN_SESSION)

    # TLS
    client.tls_set(ca_certs=CA_CERT, tls_version=ssl.PROTOCOL_TLSv1_2)
    client.tls_insecure_set(False)

    # Authentication
    client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)

    # Callbacks register
    client.on_connect = on_connect
    client.on_message = on_message

    # Connection
    client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)

    client.loop_start()

    menu = True
    while menu:
        print("\nSelect option from menu below:")
        print("poweron - power on server")
        print("state - check server state")
        print("quit - quit program")
        opt = input("...")
        if opt == "poweron":
            send_wol_signal(client)
        elif opt == "state":
            print_server_state()
        elif opt == "quit":
            menu = False
        else:
            print("Unknown option!")

    client.loop_stop()


if __name__ == '__main__':
    main()
