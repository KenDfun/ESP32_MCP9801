#include <ETH.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>

#include <Ambient.h>


#include "MCP9804.h"

#define MCP9804_DEVICE_ADDR          0x18

WiFiMulti wifiMulti;
WiFiClient client;
Ambient ambient;
MCP9804 Mcp9804;

uint64_t chipid;  
uint16_t ManufactureID;
uint8_t DeviceID;
uint8_t Revision;

void setup() {
  // LED Setup
  pinMode(4, OUTPUT);
  // I2C Setup
  pinMode(21, INPUT_PULLUP);
  pinMode(22, INPUT_PULLUP);

  Serial.begin(115200);

  chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  Serial.printf("ESP32 Chip ID [%04x",(uint16_t)(chipid>>32));//print High 2 bytes
  Serial.printf("%08x]\n",(uint32_t)chipid);//print Low 4bytes.


  Mcp9804.begin(MCP9804_DEVICE_ADDR);
  Serial.printf("MCP9804: Manufacture ID: [0x%08x]\n",Mcp9804.getManufacture());
  Serial.printf("MCP9804: Device ID:      [0x%02x]\n",Mcp9804.getDevID());  
  Serial.printf("MCP9804: Revision:       [0x%02x]\n",Mcp9804.getRevision());
  //dummy read cause first and second data sometimes wrong.
  Mcp9804.getTemperature();
  delay(500);
  Mcp9804.getTemperature();

#ifdef USE_AMBIENT
  wifiMulti.addAP("IOT1", "IOTIOTIOT");
  Serial.print("Wait wifi connect ");
  while(wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Conected!");
  Serial.printf("IP : %d\n",WiFi.localIP());
  ambient.begin(7189, "fe1ffb644b0c3611", &client); // チャネルIDとライトキーを指定してAmbientの初期化
#endif
  

}



void loop() {
  double dtemp;
  
  // put your main code here, to run repeatedly:
  digitalWrite(4, HIGH);
  delay(250);
  
  dtemp=Mcp9804.getTemperature();
  Serial.printf("temp_d: %.04f[C]\n\n",dtemp);

#ifdef USE_AMBIENT  
  ambient.set(1,temp_d);
  ambient.send();
#endif
  
  digitalWrite(4, LOW);
  delay(10000);
}
