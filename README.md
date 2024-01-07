# TP-DigitalTwin
Projekt pri predmetu Telekomunikacijski protokoli


## Opis projekta
Projekt vključuje Arduino z WiFi shieldom in priključenim BMP280 senzorjem. Arduino deluje kot HTTP strežnik, ki omogoča oddajanje podatkov preko Wi-Fi povezave. Ti podatki vključujejo informacije, ki jih zaznava BMP280 senzor.

Unity 3D je uporabljen kot prejemnik teh podatkov, ki jih nato vizualizira in prikaže. Skupno gledano, projekt omogoča komunikacijo med Arduino in Unity prek Wi-Fi povezave, kar omogoča oddaljen nadzor in vizualizacijo podatkov, zbranih s senzorja. Projekt je enostavna izvedba digitalnega dvojčka.

## Izvedba
### 1. Povezava senzorja BMP280 na Arduino z ESP8266 shieldom:
![FGTF2I8ILSAWHK8](https://github.com/matevzkalcic/TP-DigitalTwin/assets/74674279/f4a1b40b-6ee3-4f19-854a-18d3cd8c91c9)
V mojem primeru (brez breadboarda): 
![410555180_405914115120292_3040707616192304531_n](https://github.com/matevzkalcic/TP-DigitalTwin/assets/74674279/9644acea-0105-49b4-a35c-dc9a664bd5e4)

Povezava pinov na senzorju s pini na Arduinotu:
<pre>
Senzor BMP280:              Arduino:
 3Vo           ---->         3.3V
 GND           ---->         GND
 SCK           ---->         SCL
 SDI           ---->         SDA
</pre>

### 2. Programiranje Arduinota:
Arduino nam deluje kot http strežnik, na katerega se pošilja rezultate iz senzorja. 

```ruby
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
```
Vključene knjižnice: 
- Adafruit_BMP280.h: Knjižnica za BMP280 senzor.
- ESP8266WiFi.h: Knjižnica za upravljanje Wi-Fi povezave na mikrokrmilniku ESP8266.
- ESP8266WebServer.h: Knjižnica za implementacijo HTTP strežnika na ESP8266.
- Wire.h: Knjižnica za komunikacijo po I2C protokolu.
- Adafruit_Sensor.h: Skupna knjižnica za senzorje Adafruit.

Rezultat na serverju:

![image](https://github.com/matevzkalcic/TP-DigitalTwin/assets/74674279/2f7e8295-247b-4f9e-83f3-08eb749e4fef)


### 3. Vzpostavljanje povezave med Arduinotom in programom Unity 3D:
Prvo je bilo treba v Unity 3D ustvariti okolje (v tem primeru je to moja soba): 
![image](https://github.com/matevzkalcic/TP-DigitalTwin/assets/74674279/67a2bd18-6607-4f5a-86f8-13fb29e89067)

V Unity večina programiranja poteka prek jezika C#, zato sem kodo za HTTP GET zahtevek sprogramiral prav v tem jeziku:

```ruby
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.UI;
using System.Collections;

public class HttpGetRequest : MonoBehaviour
{
    public Text sensorDataText;
    public string arduinoIPAddress = "your-arduino-ip-address";

    void Start()
    {
        StartCoroutine(GetSensorData());
    }

    IEnumerator GetSensorData()
    {
        UnityWebRequest www = UnityWebRequest.Get("http://" + arduinoIPAddress);
        yield return www.SendWebRequest();

        if (www.isNetworkError || www.isHttpError)
        {
            Debug.Log(www.error);
        }
        else
        {
            // Show the sensor data in the Unity UI Text component
            sensorDataText.text = www.downloadHandler.text;
        }
    }
}
```
Uporabljene knjižnice:

- UnityEngine: Osnovna knjižnica za delo z Unity.
- UnityEngine.Networking: Knjižnica za omrežno komunikacijo v Unity.
- UnityEngine.UI: Knjižnica za upravljanje z uporabniškim vmesnikom v Unity.
- System.Collections: Knjižnica za upravljanje s kolekcijami v C#.


To skripto je nato treba "prilepiti" na element v Unity-u, in nato se na tem elementu skripta izvaja.
![image](https://github.com/matevzkalcic/TP-DigitalTwin/assets/74674279/1b48a704-4bd4-485d-992b-ba24a83566e6)

Ko nastaviš IP naslov in v Unity-u zaženes sceno (Play), ti skripta izpiše rezultate iz senzorja.
![image](https://github.com/matevzkalcic/TP-DigitalTwin/assets/74674279/414246de-e840-4fd8-8d3c-ad07fbed075a)



