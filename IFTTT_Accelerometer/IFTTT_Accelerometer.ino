#include <M5StickC.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

const char *ssid = "One+";
const char *password = "987654321";
const int BIG_BUTTON = 37;
volatile bool buttonPressed = false;

float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;

// Your Domain name with URL path or IP address with path
String iftttApiKey = "K4tibvQOfnxiGhnlpO-SR";
String event = "accel_readings";

void IRAM_ATTR buttonEvent() {
  buttonPressed = true;
}

void setup() {    
  Serial.begin(115200);
  M5.begin();
  M5.IMU.Init();
  M5.Lcd.setRotation(3);
  M5.Lcd.setCursor(0, 10);
  M5.Lcd.println("  X       Y       Z");
  pinMode(BIG_BUTTON, INPUT);
  attachInterrupt(BIG_BUTTON, buttonEvent, FALLING);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  M5.IMU.getAccelData(&accX,&accY,&accZ);

  if (buttonPressed) {
    M5.Lcd.setCursor(0, 30);
    M5.Lcd.printf(" %5.2f   %5.2f   %5.2f   ", accX, accY, accZ);
    buttonPressed = false;
    sendButtonTrigger();
    delay(500);
  }
}

void sendButtonTrigger() {
  // Send an HTTP GET request
  // Check WiFi connection status
  if (WiFi.status() == WL_CONNECTED) {
   String serverPath = "https://maker.ifttt.com/trigger/" + event + "/with/key/" + iftttApiKey + "?value1=" + accX + "&value2="+ accY + "&value3=" + accZ;
    String jsonBuffer = httpGETRequest(serverPath.c_str());
    Serial.println(jsonBuffer);
    JSONVar myObject = JSON.parse(jsonBuffer);
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}

String httpGETRequest(const char* serverName) {
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
