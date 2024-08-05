#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include "index.h"

#define CMD_STOP 0
#define CMD_FORWARD 1
#define CMD_BACKWARD 2
#define CMD_LEFT 4
#define CMD_RIGHT 8

#include <Encoder.h>

const int l1_pin = A1;
const int l2_pin = A0;

const int r1_pin = A2;
const int r2_pin = D3;

const int el_1 = D4;
const int el_2 = D5;

const int er_1 = D7;
const int er_2 = D8;

Encoder l_enc(el_1, el_2);
Encoder r_enc(er_1, er_2);


const char* ssid = "Rescue Aid Society";     // CHANGE IT
const char* password = "rescuerollers";  // CHANGE IT

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);  // WebSocket server on port 81


void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      //Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        //Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      }
      break;
    case WStype_TEXT:
      ////Serial.printf("[%u] Received text: %s\n", num, payload);
      String angle = String((char*)payload);
      int command = angle.toInt();
      //Serial.print("command: ");
      //Serial.println(command);

      switch (command) {
        case CMD_STOP:
          //Serial.println("Stop");
          stop();
          break;
        case CMD_FORWARD:
          //Serial.println("Move Forward");
          forward();
          break;
        case CMD_BACKWARD:
          //Serial.println("Move Backward");
          backward();
          break;
        case CMD_LEFT:
          //Serial.println("Turn Left");
          left();
          break;
        case CMD_RIGHT:
          //Serial.println("Turn Right");
          right();
          break;
        default:
          break;
          //Serial.println("Unknown command");
      }

      break;
  }
}

void setup() {
  //Serial.begin(9600);

  init_motor(l1_pin, l2_pin, l_enc);
  init_motor(r1_pin, r2_pin, r_enc);
  delay(1);
  
  // Connect to Wi-Fi
  int attempts = 0;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    attempts++;
    //Serial.print("Connecting to WiFi (Attempt ");
    //Serial.print(attempts);
    //Serial.println(") ...");
  }
  //Serial.println("Connected to WiFi");

  // Initialize WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // Serve a basic HTML page with JavaScript to create the WebSocket connection
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    //Serial.println("Web Server: received a web page request");
    String html = HTML_CONTENT;  // Use the HTML content from the servo_html.h file
    request->send(200, "text/html", html);
  });

  server.begin();
  //Serial.print("ESP32 Web Server's IP address: ");
  //Serial.println(WiFi.localIP());
}

void loop() {
  webSocket.loop();
  // TO DO: Your code here
}


void init_motor(int a, int b, Encoder e)
{
  // Initialise motor pins
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);

  // Reset encoder
  e.write(0);

  digitalWrite(a, LOW);
  digitalWrite(b, LOW);

}

void l_forward()
{
  // Left forward
  digitalWrite(l1_pin, HIGH);
  digitalWrite(l2_pin, LOW);
}

void l_backward()
{
  // Left backward
  digitalWrite(l1_pin, LOW);
  digitalWrite(l2_pin, HIGH);
}

void r_forward()
{
  // Right forward
  digitalWrite(r1_pin, LOW);
  digitalWrite(r2_pin, HIGH);
}

void r_backward()
{
  // Right forward
  digitalWrite(r1_pin, HIGH);
  digitalWrite(r2_pin, LOW);
}

void forward()
{
  l_forward();
  r_forward();
}

void backward()
{
  l_backward();
  r_backward();
}

void left()
{
  l_forward();
  r_backward();
}

void right()
{
  r_forward();
  l_backward();
}

void stop()
{
  digitalWrite(r1_pin, LOW);
  digitalWrite(r2_pin, LOW);
  digitalWrite(l1_pin, LOW);
  digitalWrite(l2_pin, LOW);
}

