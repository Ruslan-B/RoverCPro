#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include "Contracts.h"
#include "RoverC.h"


TFT_eSprite Disbuff = TFT_eSprite(&M5.Lcd);
WiFiUDP Udp;

uint8_t packetBuffer[255]; //buffer to hold incoming packet
PackerHeader ReplyPacket = {HEADER_ID, Ack};
// the setup routine runs once when M5StickC starts up
void setup() {

  // Initialize the M5StickC object
  M5.begin();

  M5.Lcd.setRotation(1);
  M5.Lcd.setSwapBytes(false);
  Disbuff.createSprite(160, 80);
  Disbuff.setSwapBytes(true);
  Disbuff.setTextSize(1.5);
  Disbuff.setTextColor(WHITE);

  Serial.begin(115200);
  Serial.print("connecting to WIFI");

  WiFi.begin("Castle", "Korolevstvo");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  IPAddress myIP = WiFi.localIP();
  Serial.print("My IP ");
  Serial.println(myIP);

  Disbuff.setCursor(0, 0);
  Disbuff.println(myIP);
  Disbuff.pushSprite(0, 0);


  Udp.begin(myIP, UDP_PORT);

  RoverC_Init();
  Move_stop(100);
  Servo_angle(1, 90);
  delay(1000);
}


// the loop routine runs over and over again forever
void loop() {
  M5.update();
  if (M5.BtnA.wasReleased()) {
    delay(1500);
    Servo_angle(1, 0);
    delay(1000);
    Move_back(50);
    delay(2000);
    Move_turnleft(30);
    delay(2000);
    Move_turnright(30);
    delay(2000);
    Move_forward(50);
    delay(2000);
    Move_stop(100);
    Servo_angle(1, 90);
    delay(1000);
  }

  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int len = Udp.read(packetBuffer, 255);
        Serial.print("Server to client: ");
    Serial.println(len);

    int8_t movex = packetBuffer[3];
    int8_t movey = packetBuffer[4];
    int8_t rotation = packetBuffer[5];
    bool grip = (bool)packetBuffer[6];
    Disbuff.setCursor(20, 0);
    Disbuff.print(movex);
    Serial.print(movex);
    Serial.print(' ');
    Disbuff.print(' ');
    Disbuff.print(movey);
    Serial.print(movey);
    Serial.print(' ');
    Disbuff.print(' ');
    Disbuff.print(rotation);
    Serial.println(rotation);
    Disbuff.pushSprite(0, 0);

    if (grip) {
      Servo_angle(1, 90);
    } else {
      Servo_angle(1, 0);
    }

    RoverC_Move(movex, movey, rotation);

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write((uint8_t *)&ReplyPacket, sizeof(PackerHeader));
    Udp.endPacket();
  }
}
