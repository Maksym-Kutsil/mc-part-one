#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

#define BUTTON_PIN 13
#define LED1 14
#define LED2 2
#define LED3 12

const char* ssid = "";
const char* password = "";

AsyncWebServer server(80);
bool running = false;
bool direction;

unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
unsigned long previousMillis3 = 0;
unsigned long previousMillisStop1 = 0;
unsigned long previousMillisStop2 = 0;
unsigned long previousMillisStop3 = 0;

const long interval = 500;

void connectToWiFi();
void setupWebServer();
void blinkLeds();
void stopLeds();
void handleStart(AsyncWebServerRequest *request);
void handleStop(AsyncWebServerRequest *request);
void initializePins();
void initializeEEPROM();

void setup() {
    Serial.begin(115200);
    initializeEEPROM();
    initializePins();
    
    connectToWiFi();
    setupWebServer();

    Serial.println("Server listening");
}

void loop() {
    if (running || digitalRead(BUTTON_PIN) == LOW) {
        blinkLeds();
    } else {
        stopLeds();
    }
}

void connectToWiFi() {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi connection failed!");
    }
}

void setupWebServer() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html",
            "<html><head><title>ESP8266 Control</title></head>"
            "<body style='height: 100vh; display: flex; flex-direction: column;gap: 40px; justify-content: center; align-items: center'>"
            "<h2>ESP8266 LED Control</h2>"
            "<div style='display:flex; gap: 50px'>"
            "<button onclick='sendRequest(\"/start\")' style='padding:10px 20px; font-size:20px;margin: 30px;background-color: red;color: white'>Start</button>"
            "<button onclick='sendRequest(\"/stop\")' style='padding:10px 20px; font-size:20px;margin: 30px;background-color: red;color: white'>Stop</button>"
            "</div>"
            "<script>"
            "function sendRequest(url) {"
            "  var xhr = new XMLHttpRequest();"
            "  xhr.open('GET', url, true);"
            "  xhr.send();"
            "} "
            "</script>"
            "</body></html>");
    });

    server.on("/start", HTTP_GET, handleStart);
    server.on("/stop", HTTP_GET, handleStop);

    server.begin();
}

void handleStart(AsyncWebServerRequest *request) {
    Serial.println("Start pressed!");
    running = true;
    direction = !direction;
    EEPROM.write(0, direction);
    EEPROM.commit();
    request->send(200, "text/plain", "Started");
}

void handleStop(AsyncWebServerRequest *request) {
    Serial.println("Stop pressed!");
    running = false;
    stopLeds();
    request->send(200, "text/plain", "Stopped");
}

void initializePins() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);

    stopLeds();
}

void initializeEEPROM() {
    EEPROM.begin(1);
    direction = EEPROM.read(0);
}

void stopLeds() {
    unsigned long currentMillis = millis();
    static int currentLed = LED1; 

    if (currentMillis - previousMillisStop1 >= interval) {
        previousMillisStop1 = currentMillis;

        digitalWrite(LED1, LOW);
        digitalWrite(LED2, LOW);
        digitalWrite(LED3, LOW);

        digitalWrite(currentLed, HIGH);

        if (currentLed == LED1) {
            currentLed = LED2;
        } else if (currentLed == LED2) {
            currentLed = LED3;
        } else if (currentLed == LED3) {
            currentLed = LED1;
        }
    }
}

void blinkLeds() {
    unsigned long currentMillis = millis();
    static int currentLed = LED3; 

    if (currentMillis - previousMillis1 >= interval) {
        previousMillis1 = currentMillis;

        digitalWrite(LED1, LOW);
        digitalWrite(LED2, LOW);
        digitalWrite(LED3, LOW);

        digitalWrite(currentLed, HIGH);

        if (currentLed == LED3) {
            currentLed = LED2;
        } else if (currentLed == LED2) {
            currentLed = LED1;
        } else if (currentLed == LED1) {
            currentLed = LED3;
        }
    }
}