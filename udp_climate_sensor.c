#include "DHT.h"
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#define DHTTYPE DHT22

const char* ssid = "name";
const char* password = "password";
const int DHTPin = 5;
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];
WiFiUDP Udp;
WiFiServer server(80);
DHT dht(DHTPin, DHTTYPE);
unsigned int localUdpPort = 4444;

void setup() {
  Udp.begin(localUdpPort);
  delay(10);
  dht.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  server.begin();
  delay(10000);
}

void loop() {
  WiFiClient client = server.available();
  
  if (client) {
    boolean blank_line = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (c == '\n' && blank_line) {
            float h = dht.readHumidity();
            float t = dht.readTemperature();
            float f = dht.readTemperature(true);
            if (isnan(h) || isnan(t) || isnan(f)) {
              strcpy(celsiusTemp,"Failed");
              strcpy(fahrenheitTemp, "Failed");
              strcpy(humidityTemp, "Failed");         
            }
            else{
            }
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head></head><body><h1>ESP8266 - Temperature and Humidity</h1><h3>Temperature in Celsius: ");
            client.println(t);
            client.println("&deg;C</h3><h3>Temperature in Fahrenheit: ");
            client.println(f);
            client.println("&deg;F</h3><h3>Humidity: ");
            client.println(h);
            client.println("%</h3><h3>");
            client.println("</body></html>");     
            String replyPacekt = String(round(t)) + String(round(h));
            Udp.beginPacket("10.0.0.20", 8888);
            Udp.write(replyPacekt.c_str());
            Udp.endPacket();
            break;
        }
        if (c == '\n') {
          blank_line = true;
        }
        else if (c != '\r') {
          blank_line = false;
        }
      }
    }  
    delay(1);
    client.stop();
  }
}
