#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Konfigurasi WiFi
const char *ssid = "Barulah Kamar";
const char *password = "gantipassword";

// Konfigurasi MQTT
const char *mqtt_server = "192.168.2.100";
const char *mqtt_topic = "kipas";

// Pin relay
const int relayPin = 16; // D0
const char *hostname = "IOT_KIPAS";

// Inisialisasi objek WiFi dan MQTT
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // Menghubungkan ke WiFi
  Serial.println();
  Serial.print("Menghubungkan ke ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  WiFi.hostname(hostname);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi terhubung");
  Serial.println("Alamat IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("] ");
  
  // Mengonversi payload menjadi string
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println(message);

  // Mengendalikan relay berdasarkan payload
  if (message.equals("on")) {
    digitalWrite(relayPin, HIGH);
    Serial.println("KIPAS ON");
  } else if (message.equals("off")) {
    digitalWrite(relayPin, LOW);
    Serial.println("KIPAS OFF");
  }
}

void reconnect() {
  // Loop sampai terhubung ke broker MQTT
  while (!client.connected()) {
    Serial.print("Mencoba koneksi ke MQTT...");
    
    // ID klien unik, bisa diganti sesuai keinginan
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Jika koneksi berhasil, subscribe ke topik yang diinginkan
    if (client.connect(clientId.c_str())) {
      Serial.println("terhubung");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 5 detik");
      
      // Menunggu 5 detik sebelum mencoba lagi
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Mengatur pin relay sebagai OUTPUT
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
