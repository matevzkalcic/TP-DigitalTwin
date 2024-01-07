#include <Adafruit_BMP280.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>


const char* ssid = "SSID";
const char* password = "GESLO";
const char* serverName = "IP-Arduino";

ESP8266WebServer server(80);
Adafruit_BMP280 bmp;

void handleRoot() {
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure();
  int altitude = bmp.readAltitude();
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");


  String response = "Temperature: " + String(temperature) + " °C, Pressure: " + String(pressure) + " Pa, Altitude: " + String(altitude) + " meters";
  server.send(200, "text/plain", response);
}

void handlePost() {
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure();
  int altitude = bmp.readAltitude();
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");


  String data = "temperature=" + String(temperature) + "&pressure=" + String(pressure) + "&altitude=" + String(altitude);

  WiFiClient client;
  if (client.connect(serverName, 80)) {
    client.println("POST /post HTTP/1.1");
    client.println("Host: " + String(serverName));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);
    delay(10);
    client.stop();
  }

  server.send(200, "text/plain", "Data received: " + data);

  // You can process or store the data as needed
  // Example: Send data to another server, save to a file, etc.
}

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to WiFi. IP Address: ");
  Serial.println(WiFi.localIP());

  // Set up BMP280 sensor
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  // Set up HTTP server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/post", HTTP_POST, handlePost);

  // Start server
  server.begin();
}

void loop() {
  server.handleClient(); // Handle incoming client requests

  // Additional loop code here if needed
}