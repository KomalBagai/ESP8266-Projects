//node mcu
//d1(node)--d4(nano) sda
//d2(node)--d5(nano) scl

#include <time.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <SoftwareSerial.h>
const char*  ssid = "";
const char* password = "";

const char* host = "";
const int httpsPort = 443;
char par;
// Root certificate used by api.github.com.
// Defined in "CACert" tab.
extern const unsigned char caCert[] PROGMEM;
extern const unsigned int caCertLen;
unsigned long t2 = 0;
unsigned long t1 = 0;
WiFiClientSecure client;
SoftwareSerial nodeSerial(D6, D5); //rx, tx
void setup() {
  Serial.begin(115200);
  nodeSerial.begin(9600);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Synchronize time useing SNTP. This is necessary to verify that
  // the TLS certificates offered by the server are currently valid.
  Serial.print("Setting time using SNTP");
  configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));

  // Load root certificate in DER format into WiFiClientSecure object
  bool res = client.setCACert_P(caCert, caCertLen);
  if (!res) {
    Serial.println("Failed to load root CA certificate!");
    while (true) {
      yield();
    }
  }
}

void loop() {
  //delay(3200);
  //nodeSerial.begin(9600);
  char par;
  nodeSerial.write("s");
  delay(100);
  Serial.println("Looking for message from Arduino");
  String url = "data_string";
  if (!nodeSerial.available())
    Serial.println("no value received");
  while (nodeSerial.available() > 0) {
    par = nodeSerial.read();
    url.concat(par);  //join string
    if (par == '\0')
      break;
  }
  //  nodeSerial.end();

  Serial.print("requesting URL: ");
  Serial.println(url);

  // Connect to remote server
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  // Verify validity of server's certificate
  if (client.verifyCertChain(host)) {
    Serial.println("Server certificate verified");
  } else {
    Serial.println("ERROR: certificate verification failed!");
    //   return;
  }


  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');

  Serial.println("reply was:");
  // Serial.println("==========");
  Serial.println(line);
  delay(3200);
  //nodeSerial.flush();
  Serial.println("==========");
  Serial.println();
}
