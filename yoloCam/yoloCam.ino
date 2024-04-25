#include <WebServer.h>
#include <WiFi.h>
#include <esp32cam.h>
#include "camuart.h"

const char* WIFI_SSID = "arbs";
const char* WIFI_PASS = "lafamilia";
static auto loRes = esp32cam::Resolution::find(320, 240);
static auto midRes = esp32cam::Resolution::find(350, 530);
static auto hiRes = esp32cam::Resolution::find(800, 600);
int frame_width = 800; // Update with the width of the camera resolution (e.g., hiRes)
int frame_height = 600; // Update with the height of the camera resolution (e.g., hiRes)

WebServer server(80);
// UART pins GPIO1, GPIO3
camuart mycam(2);

// Function to handle requests from Python script
void handleDataRequest() {
  // Process the request and prepare the data to send back
  String responseData = "Data from ESP32"; 
  
  // Send the response
  server.send(200, "text/plain", responseData);
}

void serveJpg() {
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  //Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),static_cast<int>(frame->size()));

  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

void handleJpgLo() {
  if (!esp32cam::Camera.changeResolution(loRes)) {
    Serial.println("SET-LO-RES FAIL");
  }
  serveJpg();
}

void handleJpgHi() {
  if (!esp32cam::Camera.changeResolution(hiRes)) {
    Serial.println("SET-HI-RES FAIL");
  }
  serveJpg();
}

void handleJpgMid() {
  if (!esp32cam::Camera.changeResolution(midRes)) {
    Serial.println("SET-MID-RES FAIL");
  }
  serveJpg();
}

//handler function for handling /alert endpoint
void handleAlertRequest() {
  // Process the alert request
  String label = server.arg("label");
  int x = server.arg("x").toInt();
  int y = server.arg("y").toInt();
  
  // Adjust x and y coordinates to center the robot
  int adjusted_x = x + (frame_width / 2);
  int adjusted_y = y + (frame_height / 2);
  
  // Combine all parts of the message into one string with comma as delimiter
  String message = "l," + label + ",x," + String(adjusted_x) + ",y," + String(adjusted_y);
  
  // Print the combined message - this is transmitted over the UART
  Serial.println(message);
  
  // Respond to the request
  server.send(200, "text/plain", "Alert received by ESP32");
}



void setup() {
  Serial.begin(115200);
  {
    using namespace esp32cam;
    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(hiRes);
    cfg.setBufferCount(2);
    cfg.setJpeg(80);

    bool ok = Camera.begin(cfg);
    Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");
  }
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.print("http://");
  Serial.println(WiFi.localIP());
  Serial.println("  /cam-lo.jpg");
  Serial.println("  /cam-hi.jpg");
  Serial.println("  /cam-mid.jpg");
  Serial.println("  /data-request"); // New endpoint for data request
  Serial.println("  /alert"); // New endpoint for alert request

  server.on("/cam-lo.jpg", handleJpgLo);
  server.on("/cam-hi.jpg", handleJpgHi);
  server.on("/cam-mid.jpg", handleJpgMid);
  server.on("/data-request", handleDataRequest); // Handler for data request
  server.on("/alert", handleAlertRequest); // Handler for alert request

  server.begin();

}

void loop() {
  server.handleClient();
}
