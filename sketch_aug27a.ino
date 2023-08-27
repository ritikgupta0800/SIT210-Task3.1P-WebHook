#include <SPI.h>
#include <WiFiNINA.h>
#include <EduIntro.h>

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "HOME";        // your network SSID (name)
char pass[] = "ShivCharan@784";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;             // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(80);

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";

String APIKey = "L8OXLOWSXMMC93I0";             // enter your channel's Write API Key

const int updateThingSpeakInterval = 20 * 1000; // 20 second interval at which to update ThingSpeak

// Variable Setup
long lastConnectionTime = 0;
boolean lastConnected = false;

// Initialize Arduino Ethernet Client
DHT11 dht11(D2);  

int C;   // temperature C readings are integers
float F; // temperature F readings are returned in float format
int H;   // humidity readings are integers
WiFiClient client;

void setup() {

  // Start Serial for debugging on the Serial Monitor

  Serial.begin(9600);

  while (!Serial) {

    ; // wait for serial port to connect. Needed for Leonardo only

  }

  // check for the presence of the shield:

  if (WiFi.status() == WL_NO_SHIELD) {

    Serial.println("WiFi shield not present");

    // don't continue:

    while (true);

  }

  // attempt to connect to Wifi network:

  while ( status != WL_CONNECTED) {

    Serial.print("Attempting to connect to SSID: ");

    Serial.println(ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:

    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:

    delay(10000);

  }

  // you're connected now, so print out the status:

  printWifiStatus();
}

void loop() {

dht11.update();

  C = dht11.readCelsius();     	// Reading the temperature in Celsius degrees and store in the C variable
  F = dht11.readFahrenheit();  	// Reading the temperature in Fahrenheit degrees and store in the F variable
  H = dht11.readHumidity();     // Reading the humidity index

  // Print the collected data in a row on the Serial Monitor
  Serial.print("H: ");
  Serial.print(H);
  Serial.print("\tC: ");
  Serial.print(C);
  Serial.print("\tF: ");
  Serial.println(F);

  delay(30000);                // Wait one second before get another temperature reading

  // Print Update Response to Serial Monitor

  if (client.available()) {

    char c = client.read();

    Serial.print(c);

  }

  // Disconnect from ThingSpeak

  if (!client.connected() && lastConnected) {

    Serial.println("...disconnected");

    Serial.println();

    client.stop();

  }

  // Update ThingSpeak

  if (!client.connected() && (millis() - lastConnectionTime > updateThingSpeakInterval)) {

    updateThingSpeak(String("field1=") + C + "&field2=" + H);


    Serial.println(C);

    Serial.println(H);

  }

  lastConnected = client.connected();
}

void updateThingSpeak(String tsData) {

  if (client.connect(thingSpeakAddress, 80)) {

    client.print("POST /update HTTP/1.1\n");

    client.print("Host: api.thingspeak.com\n");

    client.print("Connection: close\n");

    client.print("X-THINGSPEAKAPIKEY: " + APIKey + "\n");

    client.print("Content-Type: application/x-www-form-urlencoded\n");

    client.print("Content-Length: ");

    client.print(tsData.length());

    client.print("\n\n");

    client.print(tsData);

    lastConnectionTime = millis();

    if (client.connected()) {

      Serial.println("Connecting to ThingSpeak...");

      Serial.println();

    }

  }
}

void printWifiStatus() {

  // print the SSID of the network you're attached to:

  Serial.print("SSID: ");

  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:

  IPAddress ip = WiFi.localIP();

  Serial.print("IP Address: ");

  Serial.println(ip);

  // print the received signal strength:

  long rssi = WiFi.RSSI();

  Serial.print("signal strength (RSSI):");

  Serial.print(rssi);

  Serial.println(" dBm");
}