#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(443);

const char* ssid = ""; // SSID OF YOUR WiFi
const char* password = ""; // PASSWORD OF YOUR WiFi

IPAddress local_IP(192, 168, 0, 135); // ASSIGN IP FOR THE DEVICE
IPAddress gateway(192, 168, 0, 1);

IPAddress subnet(255, 255, 255, 0);

#define blue 5
#define red 0
#define green 4
#define fan 16

// Strings for request params
const char* input_r = "red";
const char* input_g = "green";
const char* input_b = "blue";

// Strings for rgb value
String rd = "254";
String grn = "254";
String blu = "254";

// States
bool ledState = true;
bool fanState = false;


static unsigned long nextSwitchTime = millis()+10000;

void offLed() {
  ledState = false;
  nextSwitchTime = millis()+120000;
  analogWrite(red, 0);
  analogWrite(green, 0);
  analogWrite(blue, 0);
}

void onLed() {
  ledState = true;
  toggleFan();
  analogWrite(red, rd.toInt());
  analogWrite(green, grn.toInt());
  analogWrite(blue, blu.toInt());
}

void setLedColor() {
  ledState = true;
  toggleFan();
  analogWrite(red, rd.toInt());
  analogWrite(green, grn.toInt());
  analogWrite(blue, blu.toInt());
}

void toggleFan() {
  if(fanState){
    fanState = false;
    digitalWrite(fan, LOW);
  } else {
    fanState = true;
    digitalWrite(fan, HIGH);
  }
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

int fileRequest(void * arg, const char *filename, uint8_t **buf) {
  Serial.printf("Reading file: %s\n", filename);
  if(SPIFFS.begin()) {
    Serial.println("SPIFFS has started");
  }
  File file = SPIFFS.open(filename, "r");
  if(file){
    Serial.printf("File found: %s\n", filename);
    size_t size = file.size();
    uint8_t * nbuf = (uint8_t*)malloc(size);
    if(nbuf){
      size = file.read(nbuf, size);
      file.close();
      *buf = nbuf;
      return size;
    }
    file.close();
  } else {
    Serial.println("File not found!");
    *buf = 0;
    return 0;
  }
}

void handleRgb(AsyncWebServerRequest *request) {
  if(request->hasParam("r") && request->hasParam("g") && request->hasParam("b")) {
    rd = request->getParam("r")->value();
    grn = request->getParam("g")->value();
    blu = request->getParam("b")->value();
    
    setLedColor();

    request->send(200, "text/plain", "OK");
  } else {
    request->send(409, "text/plain", "Request could not made due to params error.");
  }
}

void handleOn(AsyncWebServerRequest *request) {
  onLed();
  
  request->send(200, "text/plain", "OK");
}

void handleOff(AsyncWebServerRequest *request) {
  offLed();

  request->send(200, "text/plain", "OK");
}

void handleTest(AsyncWebServerRequest *request) {
  String message = "";
  
  if(request->hasParam("message")) {
    message = request->getParam("message")->value();
  }
  
  request->send(200, "text/plain", "Param: " + message);
}

void setup() {
  digitalWrite(2, LOW);
  pinMode(fan, OUTPUT);
  toggleFan();

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  
  WiFi.begin(ssid, password);
    
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    analogWrite(red, 254);
    delay(500);
    analogWrite(red, 0);
    delay(500);
    Serial.print(".");
  }

  analogWrite(red, 0);
  analogWrite(green, 254);
  
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello, world");
  });

  server.on("/test", HTTP_GET, handleTest);
  server.on("/on", HTTP_GET, handleOn);
  server.on("/off", HTTP_GET, handleOff);
  server.on("/rgb", HTTP_GET, handleRgb);

  server.onNotFound(notFound);
  server.onSslFileRequest(fileRequest, NULL);

  Serial.println( "Certs validation" );
  server.beginSecure("/Cert.pem", "/Key.pem", NULL);
  Serial.println( "HTTPs server has started" );

  delay(500);
  analogWrite(green, 0);
  ledState = false;
}

void loop() {
  if(fanState && !ledState) {
    if( nextSwitchTime < millis()) { 
        toggleFan();
    }
  }
  delay(150);
}
