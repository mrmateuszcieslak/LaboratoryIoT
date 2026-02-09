//Opracował: Mateusz Cieślak nr indeksu 9202
//Biblioteki
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include <Adafruit_BMP280.h>
#include <PMS.h>

// ---- WiFi ----
const char* ssid = "Dom 2.4";
const char* password = "Mateuszek5566";

// ---- AWS IoT Core ----
const char* mqtt_server = "a5mhqyix4e6uw-ats.iot.eu-central-1.amazonaws.com"; // z Terraform output
const int mqtt_port = 8883;

// Certyfikaty 
const char* ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char* certificate_pem_crt = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUQaqnhAuWPkqpoWnJbxwBcAe6OZowDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI1MDgxOTE3MTEz
NVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOnFI74Jhs8akurFYs8z
feQ1iVoOYRJ0ANNBfEPhm07k5disnTKyKhpyq5J66BoA1IwGml+SXMwqCC3i2N9p
0qOfDgaIPyuSJk3Y9fLM9XHZaoRD6x2a7+x88ZbRKLeMbAWK/fK8agFK7VQOe0/q
MktXYQqujzLVmYWXqFe3iKwAp6I3CXE5a2gsM5ZS8vbKdruqcNLwha3cbNTdQZai
KZpeKEf8yf6DVcutDDj4TG9Q+qikVDcciEXLuC14ThOOHmKLacJQOD3tBf7m1hGp
U0iTAtc7kvgkjqweMt8oQOmU+KNZLu/oKi1BA1q9bIHq1KJa8N709kVWwZJo98yN
escCAwEAAaNgMF4wHwYDVR0jBBgwFoAUC2/Yf1x93PuWMX+zyI2IaLgz0nUwHQYD
VR0OBBYEFPyl2u37RYJW0EinqfHQD+T6v8jjMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAtEek0JUlkYyZIkxqoO39OqhVs
1ODv1y+CdlXIYF0AZvfLSP9vOo9wPNtY7Sx9nIrHisQ7OmOmflR18hg9S/ZNxLl2
GGVFjoLq05FRGhNNPpvmw8KKszGx/Z2sHBjiKFkGBlhej37NuzWzo2P0tneSSdCq
Jo8ARMZoSs3XObi1L+6xI4E9JfJk7zUXmS6gt9rAOyqx+psmYZ5Gb/WQY+uM82Gh
U6hvGzQkTp3rR6s62Ugss/G7ZZmIJ4zhewyGvb3FI2SZW1OBfSwdbiF7krfiabWH
1E45jhCqtFNkO65gIyYXa4S4El2c4HmnzSoA4kFlObKiYD+FALwosofibBVl
-----END CERTIFICATE-----
)EOF";

const char* private_pem_key = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEA6cUjvgmGzxqS6sVizzN95DWJWg5hEnQA00F8Q+GbTuTl2Kyd
MrIqGnKrknroGgDUjAaaX5JczCoILeLY32nSo58OBog/K5ImTdj18sz1cdlqhEPr
HZrv7HzxltEot4xsBYr98rxqAUrtVA57T+oyS1dhCq6PMtWZhZeoV7eIrACnojcJ
cTlraCwzllLy9sp2u6pw0vCFrdxs1N1BlqIpml4oR/zJ/oNVy60MOPhMb1D6qKRU
NxyIRcu4LXhOE44eYotpwlA4Pe0F/ubWEalTSJMC1zuS+CSOrB4y3yhA6ZT4o1ku
7+gqLUEDWr1sgerUolrw3vT2RVbBkmj3zI16xwIDAQABAoIBACdJcFZRt4w6xRIb
lh2IY9HkhW7TF9FIGZNnYADwe29o3I/rgoewlY7YUqlNapBkL1RRD8ScDmhpb/J2
O0hgunuxEWRbpXInH7TZj4v8kgtOJK6R04OoNEYkQV29TQB74vOThlYzAAWzl+sW
e/bUPxuWVcK7z2kVqLVg5XPIbnmpWET71Ts1PnvOHS6gxHpUf+iHTsNimfr6GJk6
5Ngt43SndZZ1hLFuf3U9HKItAwSlV4iWvxY5b9g5/uc3L46dg4BR52cMAz1twdBp
Zs2SUApiONoMJUEfuIrqI5b7nuglQuepQl2guLWfqqjrdu39wkRxaaauyAAp8wBm
CTgtG5ECgYEA+W5Uk9OnawNReJsUN5GwduWkEiH6DlkW7nk1VqQVAnES5dKFo/X9
QYdcxJFt4v5qO7pJpSS+IjChcT5+ok3TZqv57QqSyK64ymBg9SnbprwSqYmcdXqL
h5TYPYtVA9KLsFIi/sCXGATTG6KkE2Jwy8PP2TRxOf4JfX2KQkrrE6MCgYEA7+05
Gtmem0MLhJ8oKRO6UzVi8MgUyjnap5tmgoOqH1bIZSO3gVtVqpAqfVDx6Lp7FFP3
fVr32TDIHka9RHAfiy7jftWBOreroJViJhnX3RB4RyrTnUxkgYViKeyBJXr6VgJd
+tgtFSvoNDjREMGmv5orX/8cDh5xCIZFu0QITo0CgYA/n8oJLyr8XAtTFVOx3QLD
QU55pZsQP99ekyJs6EvHpKoqX2nhaeTg6zNZnLMH/NeCRsF0U4wAUqFLrqJEqk9r
WCDXOyQ39oGCddUllDjILh8gic6UFN80hV4nTLwxzmn9XuY4T037PXfQC8uIekrF
XzxMblkXbtpi3TwALvYJuwKBgQCIhChb+LtgiLODELRfOxRnyBa1/oKl1mltuOSJ
GrWwA0cfFUWS2w9Rb5pdl7Mtwv3JQUWvFBW2hSU45PNIf6vTHCjhwEY5zMxhIN6p
40aKSJbHl7y04gTGFckXJbUOSH0sd0hWNK7iPwtuu21p5WYl22kWxCeIeRXDzfLb
bQh0+QKBgQDI8zIXdPykyVqVOoy+jGaquI0NQPYY4+ZP/0VEo9LdH2HNjFYxXIex
YIpw1LQ7yVrTWSY/z4t/djLZltVWxAc4s7dQ0MSoH70u2LQG69Tdjj3Kw8UxPueO
1VwVhLCNDBckYXtInxtRqqOvXgJIiHZthi8HJ/SZbAiu+40bE5ilww==
-----END RSA PRIVATE KEY-----
)EOF";

// ---- MQTT client ----
WiFiClientSecure net;
PubSubClient client(net);

// ---- Wykorzystane Czujniki ----
// DHT22
#define DHTPIN 32
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
#define RXD2     16
#define TXD2     17

// BMP280
Adafruit_BMP280 bmp;

// PMS7003
PMS pms(Serial1);
PMS::DATA data;
//Funkcja łącząca z AWS
void connectAWS() {
  net.setCACert(ca_cert);
  net.setCertificate(certificate_pem_crt);
  net.setPrivateKey(private_pem_key);

  client.setServer(mqtt_server, mqtt_port);

  Serial.print("Łączenie z AWS IoT Core...");
  while (!client.connect("esp32-weather-001")) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" Połączono!");
}

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2); // PMS7003 UART

  // WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nPołączono z WiFi!");

  // Czujniki
  dht.begin();
  if (!bmp.begin(0x76)) {
    Serial.println("Nie wykryto BMP280!");
    while (1);
  }

  // Połączenie z AWS
  connectAWS();
}

void loop() {
  client.loop();

  // Pobranie danych z czujników
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  float pressure = bmp.readPressure() / 100.0F;

  float pm1_0 = 0.0, pm25 = 0.0, pm10 = 0.0;

  if (pms.read(data)) {
    pm1_0 = data.PM_AE_UG_1_0;
    pm25  = data.PM_AE_UG_2_5;
    pm10  = data.PM_AE_UG_10_0;
  }

    Serial.print("PM1.0 = ");
    Serial.print(pm1_0);
    Serial.print(", PM2.5 = ");
    Serial.print(pm25);
    Serial.print(", PM10 = ");
    Serial.println(pm10);


  // JSON payload
  StaticJsonDocument<256> doc;
  doc["device_id"]   = "esp32-weather-001";
  doc["temperature"] = temp;
  doc["humidity"]    = hum;
  doc["pressure"]    = pressure;
  doc["pm1_0"]       = pm1_0;
  doc["pm2_5"]       = pm25;
  doc["pm10"]        = pm10;
  doc["timestamp"]   = (long)time(nullptr);

  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);

  if (client.publish("devices/esp32-weather-001/telemetry", jsonBuffer)) {
    Serial.println("Wysłano: ");
    Serial.println(jsonBuffer);
  } else {
    Serial.println("Błąd wysyłania MQTT");
  }

  delay(10000);
}
