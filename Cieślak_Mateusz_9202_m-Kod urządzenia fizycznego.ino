//Opracował: Mateusz Cieślak nr indeksu 9202
//Biblioteki
#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_BMP280.h>
#include "DHT.h"
#include <SPI.h>
#include <PMS.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ST7796S_kbv.h"
#include "RTClib.h"
#include <time.h>

// Wi-Fi
const char* ssid     = "Dom 2.4";
const char* password = "Mateuszek5566";

// Kolory
#define BLACK   0x0000
#define WHITE   0xFFFF
#define TURKUS  0x07FF

// NTP
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

// TFT piny
#define TFT_CS   5
#define TFT_DC   33
#define TFT_RST  27
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_MISO 19

// Czujniki
#define DHTPIN   32
#define DHTTYPE  DHT22
#define SDA_PIN  21
#define SCL_PIN  22
#define RXD2     16
#define TXD2     17

// Obiekty
Adafruit_ST7796S_kbv tft = Adafruit_ST7796S_kbv(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST, TFT_MISO);
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp;
PMS pms(Serial1);
PMS::DATA data;
RTC_DS1307 rtc;

// Bufory czasu
char buf[20], buf1[20];
String lastClock = "";
float last_t = NAN, last_h = NAN, last_p = NAN;
int last_pm1 = -1, last_pm25 = -1, last_pm10 = -1;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("\nPołączono z Wi-Fi");
  Serial.println(WiFi.localIP());

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!rtc.begin()) {
    Serial.println("Brak RTC!");
    while (1);
  }

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    rtc.adjust(DateTime(
      timeinfo.tm_year + 1900,
      timeinfo.tm_mon + 1,
      timeinfo.tm_mday,
      timeinfo.tm_hour,
      timeinfo.tm_min,
      timeinfo.tm_sec
    ));
    Serial.println("RTC zsynchronizowany z NTP.");
  } else {
    Serial.println("Błąd pobierania czasu z NTP.");
  }

  tft.begin();
  tft.setRotation(1);
  dht.begin();
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);

  if (!bmp.begin(0x76)) {
    Serial.println("Brak BMP280!");
    while (1);
  }

  drawStaticLayout();
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  float p = bmp.readPressure() / 100.0F;

  if (pms.read(data)) {
    // odczyt udany
  }

  DateTime now = rtc.now();
  sprintf(buf, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  sprintf(buf1, "%02d/%02d/%04d", now.day(), now.month(), now.year());

  updateValues(t, h, p, data.PM_AE_UG_1_0, data.PM_AE_UG_2_5, data.PM_AE_UG_10_0);
  updateClock(buf1, buf);
  showNtpTime();

  delay(10000);
}

// ---------------------- Wyświetlanie ----------------------

void drawStaticLayout() {
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(80, 10);
  tft.println("Stacja Pogodowa IoT");

  tft.setCursor(10, 50);  tft.println("Parametr");
  tft.setCursor(200, 50); tft.println("Wartosc");
  tft.setCursor(360, 50); tft.println("Jednostka");

  for (int y = 40; y <= 260; y += 30)
    tft.drawLine(0, y, 480, y, WHITE);

  tft.drawLine(180, 40, 180, 270, WHITE);
  tft.drawLine(340, 40, 340, 270, WHITE);

  const char* labels[] = { "Temperatura", "Wilgotnosc", "Cisnienie", "PM1.0", "PM2.5", "PM10" };
  const char* units[]  = { "C", "%", "hPa", "ug/m3", "ug/m3", "ug/m3" };
  int startY = 80;

  for (int i = 0; i < 6; i++) {
    tft.setCursor(10, startY + i * 30);
    tft.setTextColor(WHITE);
    tft.print(labels[i]);
    tft.setCursor(360, startY + i * 30);
    tft.print(units[i]);
  }
}

void updateValues(float t, float h, float p, int pm1, int pm25, int pm10) {
  int startY = 80;
  int rowH = 30;

  tft.setTextSize(2);
  tft.setTextColor(TURKUS);

  if (t != last_t) {
    tft.fillRect(200, startY, 130, 24, BLACK);
    tft.setCursor(200, startY);
    tft.printf("%.1f", t);
    last_t = t;
  }

  if (h != last_h) {
    tft.fillRect(200, startY + rowH, 130, 24, BLACK);
    tft.setCursor(200, startY + rowH);
    tft.printf("%.1f", h);
    last_h = h;
  }

  if (p != last_p) {
    tft.fillRect(200, startY + 2 * rowH, 130, 24, BLACK);
    tft.setCursor(200, startY + 2 * rowH);
    tft.printf("%.1f", p);
    last_p = p;
  }

  if (pm1 != last_pm1) {
    tft.fillRect(200, startY + 3 * rowH, 130, 24, BLACK);
    tft.setCursor(200, startY + 3 * rowH);
    tft.print(pm1);
    last_pm1 = pm1;
  }

  if (pm25 != last_pm25) {
    tft.fillRect(200, startY + 4 * rowH, 130, 24, BLACK);
    tft.setCursor(200, startY + 4 * rowH);
    tft.print(pm25);
    last_pm25 = pm25;
  }

  if (pm10 != last_pm10) {
    tft.fillRect(200, startY + 5 * rowH, 130, 24, BLACK);
    tft.setCursor(200, startY + 5 * rowH);
    tft.print(pm10);
    last_pm10 = pm10;
  }
}

void updateClock(const char* dateStr, const char* timeStr) {
  String current = String(dateStr) + " " + String(timeStr);
  if (current != lastClock) {
    tft.setTextSize(2);  
    tft.setTextColor(WHITE);
    tft.fillRect(0, 275, 480, 28, BLACK); 
    tft.setCursor(10, 277);
    tft.print("RTC: ");
    tft.print(dateStr);
    tft.print(" ");
    tft.print(timeStr);
    lastClock = current;
  }
}

void showNtpTime() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    tft.setTextSize(2);
    tft.setTextColor(TURKUS);
    tft.fillRect(0, 300, 480, 28, BLACK);
    tft.setCursor(10, 302);
    tft.print("NTP: ");
    tft.printf("%02d/%02d/%04d %02d:%02d:%02d",
      timeinfo.tm_mday,
      timeinfo.tm_mon + 1,
      timeinfo.tm_year + 1900,
      timeinfo.tm_hour,
      timeinfo.tm_min,
      timeinfo.tm_sec
    );
  } else {
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    tft.fillRect(0, 300, 480, 28, BLACK);
    tft.setCursor(10, 302);
    tft.print("NTP: brak dostepu");
  }
}
