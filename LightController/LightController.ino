#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define LIGHT_UP_PIN 27
#define LIGHT_DOWN_PIN 25
#define AIRCON_PIN 12
#define BUTTON_PIN 0

//char *ssid = "aterm-c832fc-g";
//char *password = "6d277841965f5";

char *ssid = "mcr-net-n";
char *password = "wifidenpa893";

const char *endpoint = "a33axmz8076amo-ats.iot.us-west-2.amazonaws.com";
const int port = 8883;
char *pubTopic = "$aws/things/LightController/shadow/update";
char *subTopic = "$aws/things/LightController/shadow/update/delta";

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

const char* certificate = "-----BEGIN CERTIFICATE-----\n" \
                          "MIIDWTCCAkGgAwIBAgIUIwPuhDGBZO3/LTZ3dI9iiusueg4wDQYJKoZIhvcNAQEL\n" \
                          "BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n" \
                          "SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE5MDIxMTA3NDgz\n" \
                          "MloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n" \
                          "ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKqSBChkAB5CkNx2oAAA\n" \
                          "QAm2ox80qCiwg9qB4EgL/I8kBmNvv76MRFWC/S79iaAhaWsF07oD6nnxn8Q335s8\n" \
                          "RQh/cOdHgvZiJcp1MJoxEstal7LSIX51YD9Whfvm8d/HjahNHt9abzaHz+WBRnwz\n" \
                          "6RMV7NqnbryjIIH2ZID7fTNY2Jn2lvxuzABzBv256YH07W1F9CLz6+RZ0N0bBZfY\n" \
                          "04Z40Od00Lh75ni8hfJDR8c/3vFBgOkMhvFVcUuYwx8U5CaVsCxHLmBG3UqGdS9v\n" \
                          "4XwUOWFZAGWMrTXdOSfE3C/UBk5LhY1y+xsAbiqflpNfRO/Z9rOQ4ell5ha8FhPA\n" \
                          "q5MCAwEAAaNgMF4wHwYDVR0jBBgwFoAUEOauAzKP4evgxnzfbRa0sWtkJsEwHQYD\n" \
                          "VR0OBBYEFEUYooC92fuZr4pBFRy8vngJSFDFMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n" \
                          "AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBGYD0aLyryZzeyPQO9sKN6p4al\n" \
                          "Ft9NL3AxWMP2UInD5xNEKa5pgUIn4QSbOhEMsAuDoM3bNEh74AJGNXa/TmiFFp1A\n" \
                          "fds//fTM6gRboeEYzJ6z47YWxCAoh8oPPjhNly7KS55kaTV+xD5DaBPpSa3WXmyv\n" \
                          "ej0V6UIMldkdy63E6zsBefV4TimpzcsPlJHM53lylpOZT+j1xWV7i5rcPBq7FUEq\n" \
                          "v7NZolzaojFHreshCISsms+JXgAm/yI6VPcln176iGdsqhZstMrfQo6y0eEZYiT6\n" \
                          "H7RnCP/ix84t+uHfygqvjMx/ASvlemS6gHaD0gXayaVTX9OT6j+kMeTfGtgk\n" \
                          "-----END CERTIFICATE-----\n";

const char* privateKey = "-----BEGIN RSA PRIVATE KEY-----\n" \
                         "MIIEpQIBAAKCAQEAqpIEKGQAHkKQ3HagAABACbajHzSoKLCD2oHgSAv8jyQGY2+/\n" \
                         "voxEVYL9Lv2JoCFpawXTugPqefGfxDffmzxFCH9w50eC9mIlynUwmjESy1qXstIh\n" \
                         "fnVgP1aF++bx38eNqE0e31pvNofP5YFGfDPpExXs2qduvKMggfZkgPt9M1jYmfaW\n" \
                         "/G7MAHMG/bnpgfTtbUX0IvPr5FnQ3RsFl9jThnjQ53TQuHvmeLyF8kNHxz/e8UGA\n" \
                         "6QyG8VVxS5jDHxTkJpWwLEcuYEbdSoZ1L2/hfBQ5YVkAZYytNd05J8TcL9QGTkuF\n" \
                         "jXL7GwBuKp+Wk19E79n2s5Dh6WXmFrwWE8CrkwIDAQABAoIBAQCUH1kc+jpgDM5f\n" \
                         "pWQsnAsAZGojn0aojyDxj/eEmWQpqc8iatjIUGbHG4liVSo+tMUm+1hyIRRt8TYc\n" \
                         "36h+1kfcqU8SiTdhXroSptIeFJ+NmI5D69bKSoPafPi9WECTZGfPz6kPrSkG4cnY\n" \
                         "M/tRwIKW4GNfPiccCU0hjyaYG80N2J6/JunbnLb+1ZZdLDnkCiN7KlQPjmuVmwqH\n" \
                         "aOQPGjOWlxa79lRgctrxJFx7mzwEgPDCZRlKXUx/PAGo/2gQJUNUFSgJ2cl8TWYk\n" \
                         "xtRLibGuF1DuZ2kxRSjoOPPq3h1SvIcehmgRbq7Fh5PPFPV1H1HdqjJJC0HUTb28\n" \
                         "44QCen4BAoGBANU0QUQ1u+4HPAA6ZEMLqB4lnMw1YB0+WO1Z59SV7SK+VFHPIcfr\n" \
                         "JGyDn5cBqZu2kwE/kcQcMaDC0q89/0to8jck24mljvpR0L3jEm8aTBtDxqjMWaTB\n" \
                         "ra8IxhuuUaVf/dtGS57jcuYVpgjchQ/sulc1Qi4anfIObM/FsqTpRoQDAoGBAMzO\n" \
                         "+d72vt/dYKySPSX+PBQh0GIuDVDTULQlC/zdXgo85zB9PZXoAqTDGLKg5Njncx8s\n" \
                         "J4QpYTzZnnZ2GKKbqYiMKSWYWBt8sI1LLqzUA+ddCiHv7la1AezFXz9IkqDkl4Ja\n" \
                         "ohftxDOW6+DrwIrHqEeiSNT2O/A3me7BvMoF2c0xAoGBALYJ4PQxgR2SlbuIH9VT\n" \
                         "HZ9n9AI0TLWLZ3h5PEW6IXh51Brp5WkCL95rYBcaN0wZzljRg09T11fxINy57cHg\n" \
                         "gHdvA/f6ISNAOh/3wuSUKolZDEVR/fR73iVGM4itBU9Tr7fKxISMpa0orNSCHSbk\n" \
                         "tJIDilc9KMDlaijZH1hJSYkrAoGBALXS0lY1bINF//t04MCIEkI1OtDO3Zdx4uXr\n" \
                         "NddJl6EzLHQJ29Dn5uDrK1P6LvC5maSFCJf2PJNPB7VXZsuFsl69QBJWS3Ironnt\n" \
                         "0wQwPT77hHFYlpl3nLvdBZAjF7mq9y8M03a26UZ7p/FImukaE7V0x3/5EoFSe2BR\n" \
                         "sw8PXqshAoGAZUtZKPO9yPR1jZkWOvpwQnDWllUn/Ebel2liypDiv9+Nb8elBxS7\n" \
                         "YNl6BV+5fSp1b9PIKzftKCkB9Uo7mX6o9Eom6I9YvkAge+Tyk02oNXu1rRu3P8r5\n" \
                         "73D6nZEtlvFZTEkswlXokeSTCC2N8tZnz+mjfbYHnsq9Zuz7AP8fatw=\n" \
                         "-----END RSA PRIVATE KEY-----\n";

WiFiClientSecure httpsClient;
PubSubClient mqttClient(httpsClient);

char pubMessage[128];
char lightUpState[5] = "OFF";
char lightDownState[5] = "OFF";
char airconState[5] = "OFF";

void setup() {
  Serial.begin(115200);

  ledcSetup(0, 50, 10);  // 0ch 50 Hz 10bit resolution
  ledcAttachPin(LIGHT_UP_PIN, 0); // 27pin, 0ch

  ledcSetup(0, 50, 10);  // 0ch 50 Hz 10bit resolution
  ledcAttachPin(LIGHT_DOWN_PIN, 1); // 25pin, 1ch

  ledcSetup(0, 50, 10);  // 0ch 50 Hz 10bit resolution
  ledcAttachPin(AIRCON_PIN, 2); // 0pin, 2ch

  pinMode(BUTTON_PIN, INPUT);

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
    if (mqttClient.connect("ESP323254")) {
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
  else if (state.containsKey("aircon")) {
    strcpy(airconState, state["aircon"]);
    Serial.print("aircon -> ");
    Serial.println(airconState);
    switchAircon(airconState);
  }
  sendStatus(true);
}

void switchLight(char* state, char* deviceName) {
  int ch;
  if(!strcmp(deviceName, "lightUp")) ch = 0;
  else if(!strcmp(deviceName, "lightDown")) ch = 1;
  
  if (!strcmp(state, "OFF")) {
    ledcWrite(ch, 70);
    delay(150);
    ledcWrite(ch, 0);
  } else if (!strcmp(state, "ON")) {
    ledcWrite(ch, 95);
    delay(150);
    ledcWrite(ch, 0);
  }
}


void switchAircon(char* state){
  int ch = 2;
  if(!strcmp(state, "OFF")){
    ledcWrite(ch, 67);
    delay(150);
    ledcWrite(ch, 0);
  }else{
    ledcWrite(ch, 103);
    delay(150);
    ledcWrite(ch, 0);
  }
}

void sendStatus(bool isReported) {
  if(isReported){
    sprintf(pubMessage, "{\"state\": {\"reported\": {\"lightUp\": \"%s\",\"lightDown\": \"%s\",\"aircon\": \"%s\"}}}", lightUpState, lightDownState, airconState);
  }else{
    char next[5];
    if(!strcmp(lightUpState, "OFF")){
      sprintf(next, "ON");
    }else{
      sprintf(next, "OFF");
    }
    sprintf(pubMessage, "{\"state\": {\"desired\": {\"lightUp\": \"%s\",\"lightDown\": \"%s\",\"aircon\": \"%s\"}}}", (char*)next, next, next);
  }
  
  Serial.print("Publishing message to topic ");
  Serial.println(pubTopic);
  Serial.println(pubMessage);
  mqttClient.publish(pubTopic, pubMessage);
  Serial.println("Published.\n");
}

void mqttLoop() {
  if(WiFi.status() != WL_CONNECTED){
    connectWiFi();
  }
  if (!mqttClient.connected()) {
    connectAWSIoT();
  }
  mqttClient.loop();
}
unsigned long beforeTime = millis();
void loop() {
  mqttLoop();
  if(digitalRead(BUTTON_PIN) == LOW && millis() - beforeTime > 5000){
    sendStatus(false);
    beforeTime = millis();
  }
}
