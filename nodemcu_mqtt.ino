#include <ESP8266WiFi.h>        // Library to handle WiFi connections for ESP8266
#include <PubSubClient.h>       // MQTT client library

// WiFi credentials (replace with actual SSID and password)
const char* ssid = "DIR-615-E8AB";          // WiFi network name
const char* password = "Shadow@444";      // WiFi network password

// MQTT broker configuration
const char* mqtt_server = "broker.hivemq.com";   // Public MQTT broker URL
const int mqtt_port = 1883;                      // MQTT broker port (default is 1883)
const char* mqtt_topic = "efs/iotDisplay"; // Topic for  data

// Create WiFi and MQTT client instances
WiFiClient espClient;          // Handles network connection
PubSubClient client(espClient); // Handles MQTT connection using the network client

void setup() {
  Serial.begin(115200);        // Start serial communication at 115200 baud rate
  delay(10);                   // Small delay to stabilize

  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);         // Set WiFi to Station mode (connect to router)
  WiFi.begin(ssid, password);  // Begin WiFi connection using credentials

  // Wait until connected to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);                // Wait 500ms between connection attempts
    Serial.print(".");         // Print progress
  }
  Serial.println("\nWiFi connected"); // Connected successfully

  // Set MQTT server and port
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callBack);
}

//FOr receiving messages
void callBack(char* topic,byte* payload,unsigned int length){
  String message="";
  for(int i=0;i<length;i++){
    message+=(char)payload[i];
  }
  message+='\n';
  //Serial.println((const uint8_t*)message.c_str(),message.length());
  Serial.print(message);
}
// Reconnect function for MQTT
void reconnect() {
  // Attempt to reconnect until successful
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect with a unique client ID
    if (client.connect("ESP8266ClientFOREFS")) {
      Serial.println("connected");  // Connected to MQTT broker
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state()); // Print error code
      Serial.println(" retrying in 5 seconds...");

      // Wait 5 seconds (in smaller delays) to allow watchdog feeding
      for (int i = 0; i < 5000; i++) {
        delay(1);   // Delay 1ms
        yield();    // Feed the watchdog to prevent reboot
      }
    }
  }
}

void loop() {
    if (!client.connected()) {
    reconnect();  // Try to reconnect
  }
  client.loop();  // Maintain MQTT connection and process messages
}
