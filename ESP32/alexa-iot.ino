#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define LIGHT_UP_PIN 27
#define LIGHT_DOWN_PIN 25
#define BUTTON_PIN 0

char *ssid = "YOUR WIFI SSID"; //ssidを設定
char *password = "YOUR WIFI PASSWORD";//wifiのパスワードを設定


const char *endpoint = "**********.iot.us-west-2.amazonaws.com"; //変更してね
const int port = 8883;
char *pubTopic = "$aws/things/************/shadow/update"; //変更してね（今回の例ではpowerPotchinThings)
char *subTopic = "$aws/things/************/shadow/update/delta"; //上と同じように変更
//共通なので変更なし
const char* rootCA = "-----BEGIN CERTIFICATE-----\n" \
                     "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
                     "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
                     "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
                     "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
                     "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
                     "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
                     "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
                     "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
                     "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
                     "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
                     "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
                     "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
                     "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
                     "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
                     "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
                     "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
                     "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
                     "rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
                     "-----END CERTIFICATE-----\n";
//ダウンロードした証明書を\nを消さないように頑張って貼り付けてください。(******-certificate.pem.crt)
const char* certificate = "-----BEGIN CERTIFICATE-----\n" \
                          "MIIDWjCCAkKgAwIBAgIVAKVg/gqHy/ckw3eQUm/hD0dBgjw1MA0GCSqGSIb3DQEB\n" \
                          "CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "-----END CERTIFICATE-----\n";
//ダウンロードした証明書を\nを消さないように頑張って貼り付けてください。(*******-private.pem.key)
const char* privateKey = "-----BEGIN RSA PRIVATE KEY-----\n" \
                          "MIIEpQIBAAKCAQEA3sLzIsXSdxl9uZFBxCGGzs3mg0ydzr35nVloloi9hcLCqNS4\n" \
                          "rveD/iTirYYCe+uY8cdJgJDxNbNtI64Izdm0C1nZf+G/WoTiIsOa25VmA2CtXJEY\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "\n" \
                          "-----END RSA PRIVATE KEY-----\n";

WiFiClientSecure httpsClient;
PubSubClient mqttClient(httpsClient);

char pubMessage[128];
char lightUpState[5] = "OFF";
char lightDownState[5] = "OFF";

void setup() {
  Serial.begin(115200);

  ledcSetup(0, 50, 10);  // 0ch 50 Hz 10bit resolution
  ledcAttachPin(LIGHT_UP_PIN, 0); // 27pin, 0ch

  ledcSetup(0, 50, 10);  // 0ch 50 Hz 10bit resolution
  ledcAttachPin(LIGHT_DOWN_PIN, 1); // 25pin, 1ch

  pinMode(BUTTON_PIN, INPUT);

  uint8_t mac3[6];
  esp_read_mac(mac3, ESP_MAC_WIFI_STA);
  Serial.printf("[Wi-Fi] Mac Address = %02X:%02X:%02X:%02X:%02X:%02X\r\n", mac3[0], mac3[1], mac3[2], mac3[3], mac3[4], mac3[5]);

  connectWiFi();
  
  // Configure MQTT Client
  httpsClient.setCACert(rootCA);
  httpsClient.setCertificate(certificate);
  httpsClient.setPrivateKey(privateKey);
  mqttClient.setServer(endpoint, port);
  mqttClient.setCallback(mqttCallback);

  connectAWSIoT();

  sendStatus(true); 
}

void connectAWSIoT() {
  while (!mqttClient.connected()) {
    if (mqttClient.connect("ESP32")) {
      Serial.println("Connected.");
      int qos = 0;
      mqttClient.subscribe(subTopic, qos);
      Serial.println("Subscribed.");
    } else {
      Serial.print("Failed. Error state=");
      Serial.print(mqttClient.state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void connectWiFi(){
    // Start WiFi
  Serial.println("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected.");
}

void mqttCallback (char* topic, byte* payload, unsigned int length) {
  const size_t capacity = 3 * JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + 200;
  DynamicJsonBuffer jsonBuffer(capacity);
  char msg[500];
  Serial.print("Received. topic=");
  Serial.println(topic);
  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
  }
  JsonObject& root = jsonBuffer.parseObject(msg);
  JsonObject& state = root["state"];
  if (state.containsKey("lightUp")) {
    strcpy(lightUpState, state["lightUp"]);
    Serial.print("lightUp -> ");
    Serial.println(lightUpState);
    switchLight(lightUpState, "lightUp");
  }
  else if (state.containsKey("lightDown")) {
    strcpy(lightDownState, state["lightDown"]);
    Serial.print("lightDown -> ");
    Serial.println(lightDownState);
    switchLight(lightDownState, "lightDown");
  }

  sendStatus(true);
}

void switchLight(char* state, char* deviceName) {
  int ch;
  if(!strcmp(deviceName, "lightUp")) ch = 0;
  else if(!strcmp(deviceName, "lightDown")) ch = 1;
  
  if (!strcmp(state, "OFF")) {
    if(ch == 1){
      ledcWrite(ch, 80);
    }else{
      ledcWrite(ch, 80);
    }
    delay(150);
    ledcWrite(ch, 0);
  } else if (!strcmp(state, "ON")) {
    if(ch==1){
      ledcWrite(ch, 105);
    }else{
      ledcWrite(ch, 105);
    }
    delay(150);
    ledcWrite(ch, 0);
  }
}


void sendStatus(bool isReported) {
  if(isReported){
    sprintf(pubMessage, "{\"state\": {\"reported\": {\"lightUp\": \"%s\",\"lightDown\": \"%s\"}}}", lightUpState, lightDownState);
  }else{
    char next[5];
    if(!strcmp(lightUpState, "OFF")){
      sprintf(next, "ON");
    }else{
      sprintf(next, "OFF");
    }
    sprintf(pubMessage, "{\"state\": {\"desired\": {\"lightUp\": \"%s\",\"lightDown\": \"%s\"}}}", (char*)next, next, next);
  }
  
  Serial.print("Publishing message to topic ");
  Serial.println(pubTopic);
  Serial.println(pubMessage);
  mqttClient.publish(pubTopic, pubMessage);
  Serial.println("Published.\n");
}

void mqttLoop() {
  if (!mqttClient.connected()) {
    if(WiFi.status() != WL_CONNECTED){
      connectWiFi();
    }
    connectAWSIoT();
  }
  mqttClient.loop();
}
unsigned long beforeTime = millis();
void loop() {
  mqttLoop();
  if(digitalRead(BUTTON_PIN) == LOW && millis() - beforeTime > 3000){
    sendStatus(false);
    beforeTime = millis();
  }
}
