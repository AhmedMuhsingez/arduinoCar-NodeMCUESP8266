#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <FS.h> 

//Include Lib for Arduino to Nodemcu
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

SoftwareSerial nodemcu(D6, D5);

//Timer to run Arduino code every 5 seconds
unsigned long previousMillis = 0;
unsigned long currentMillis;
const unsigned long period = 10000;  


ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80
String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);
const int led = 2;
#define IN1 15
#define IN2 13
#define IN3 2
#define IN4 0
void handleRoot();              // function prototypes for HTTP handlers
void handleLED();
void handleNotFound();



void handleForward() {
	digitalWrite(IN1, HIGH);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, HIGH); server.send(303);
}void handleBackward() {
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH);
	digitalWrite(IN3, HIGH);
	digitalWrite(IN4, LOW); server.send(303);
}void handleRight() {
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, HIGH); server.send(303);
}void handleLeft() {
	digitalWrite(IN1, HIGH);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, HIGH);
	digitalWrite(IN4, LOW); server.send(303);
}void handleStop() {
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, LOW); server.send(303);
}
void handlemove() {
	digitalWrite(IN1, HIGH);
	digitalWrite(IN2, HIGH);
	digitalWrite(IN3, HIGH);
	digitalWrite(IN4, HIGH); server.send(303);
}

  StaticJsonDocument<128> doc;
  void handleDistance() {
  
  	DeserializationError error = deserializeJson(doc, nodemcu); 

	server.sendHeader("Access-Control-Allow-Origin", "*");
 	server.send(1200, "application/json", doc.as<String>());
};


void setup(void) {
	Serial.begin(9600);         // Start the Serial communication to send messages to the computer
	delay(10);
	nodemcu.begin(9600);

	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);
	pinMode(IN3, OUTPUT);
	pinMode(IN4, OUTPUT);
	   
	// add Wi-Fi networks you want to connect to
	wifiMulti.addAP("DESKTOP", "o!98K333");

	wifiMulti.addAP("Ahmed's phone", "dpzd3185");
	wifiMulti.addAP("666E_2.4GHz", "7dDUv4x9CF");
	wifiMulti.addAP("DESKTOP", "o!98K333");

	wifiMulti.addAP("TurkTelekom_T67E3", "atwm@atwm");


	Serial.println("Connecting ...");
	int i = 0;
	while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
		delay(250);
		Serial.print('.');
	}
	// Serial.println(' ');
	Serial.print("Connected to ");
	Serial.println(WiFi.SSID());              // Tell us what network we're connected to
	Serial.print("IP address:\t");
	Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer

	if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
		Serial.println("mDNS responder started");
	}
	else {
		Serial.println("Error setting up MDNS responder!");
	}

	SPIFFS.begin();
	server.on("/forward", HTTP_POST, handleForward);  // Call the 'handleLED' function when a POST request is made to URI "/LED"
	server.on("/backward", HTTP_POST, handleBackward);
	server.on("/right", HTTP_POST, handleRight);
	server.on("/left", HTTP_POST, handleLeft);
	server.on("/stop", HTTP_POST, handleStop);

	server.on("/distance", HTTP_GET, handleDistance);

	server.onNotFound([]() {                              // If the client requests any URI
		if (!handleFileRead(server.uri()))                  // send it if it exists
			server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
	});
	server.begin();                           // Actually start the server
}



void loop(void) {
	MDNS.update();
	server.handleClient();          
  	DeserializationError error = deserializeJson(doc, nodemcu); 
	if (error) {
	return;
  }

}



String getContentType(String filename) { // convert the file extension to the MIME type
	if (filename.endsWith(".html")) return "text/html";
	else if (filename.endsWith(".css")) return "text/css";
	else if (filename.endsWith(".js")) return "application/javascript";
	else if (filename.endsWith(".ico")) return "image/x-icon";
	return "text/plain";
}

bool handleFileRead(String path)  // send the right file to the client (if it exists)
{
	Serial.println("handleFileRead: " + path);
	if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
	String contentType = getContentType(path);            // Get the MIME type
	if (SPIFFS.exists(path)) {                            // If the file exists
		File file = SPIFFS.open(path, "r");                 // Open it
		size_t sent = server.streamFile(file, contentType); // And send it to the client
		file.close();                                       // Then close the file again
		return true;
	}
	Serial.println("\tFile Not Found");
	return false;                                         // If the file doesn't exist, return false
}

