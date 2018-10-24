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

//#define USE_AMBIENT
#include <Ambient.h>

#include "MCP9804.h"
#define MCP9804_DEVICE_ADDR          0x18

WiFiMulti wifiMulti;
WiFiClient client;
Ambient ambient;
MCP9804 Mcp9804;

#define MCP9804_CRIT_TEMP 34.0
#define MCP9804_UPPER_TEMP 30.75
#define MCP9804_LOWER_TEMP 10.0

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
  Mcp9804.setUpperTemperature(MCP9804_UPPER_TEMP);
  Mcp9804.setCriticalTemperature(MCP9804_CRIT_TEMP);
  Mcp9804.setLowerTemperature(MCP9804_LOWER_TEMP);
  //dummy read cause first and second data sometimes wrong.
  Mcp9804.getTemperature();
  delay(500);
  Mcp9804.getTemperature();

#ifdef USE_AMBIENT
  scanNetworks();
  ambient.begin(7189, "fe1ffb644b0c3611", &client); // チャネルIDとライトキーを指定してAmbientの初期化
#endif
}

#define MAX_NUM_SSID_PRINT  10
#define MAX_NUM_PASS  64
char PassWd[MAX_NUM_PASS+1];
char SsidName[64];

void scanNetworks() {
  int totalSsid;
  int startNet=0;
  int maxPrint=0;
  int thisNet;
  int inByte;
  int ssid;

  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  byte numSsid = WiFi.scanNetworks();

  // print the list of networks seen:
  Serial.print("SSID List:");
  Serial.println(numSsid);
  
  while(1){
    maxPrint+=MAX_NUM_SSID_PRINT;
    if(numSsid>maxPrint){totalSsid=maxPrint;}
    else{totalSsid=numSsid;}
    // print the network number and name for each network found:
    for (thisNet = startNet; thisNet<totalSsid; thisNet++) {
      Serial.print(thisNet-startNet);
      Serial.print(") Network: ");
      Serial.print(WiFi.SSID(thisNet));
      long rssi = WiFi.RSSI(thisNet);
      Serial.print(" : ");
      Serial.print(rssi);
      Serial.println(" dBm");
    }
    
    readBufferClear();
    Serial.print("Input Network Num > ");
    inByte=readByteData();
    Serial.println("");
    
    if(inByte<0x30 || inByte>0x39){
      if(thisNet>=numSsid){
        Serial.println("No more Network!");
        break;
      }
      Serial.println("Search next Network...");
      startNet=thisNet;
      continue;
    }
    else{
      ssid=inByte-0x30+startNet;
      Serial.printf("Select Network num : [%d] ",ssid);
      strcpy(SsidName,WiFi.SSID(ssid).c_str());
      Serial.println(SsidName);
      break;
    }
  }
  
  if(readWifiPass(PassWd)){
    Serial.print("Pass : ");
    Serial.println(PassWd);
  }
  else{
    Serial.print("Miss! Pass : ");
    Serial.println(PassWd);
    while(1);
  }

  wifiMulti.addAP(SsidName, PassWd);
  Serial.print("Wait wifi connect ");
  while(wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("Conected!");
  Serial.printf("IP : %d\n",WiFi.localIP());  
}
 
byte readByteData(void)
{
  byte inByte; 

  while(1){
    if (Serial.available()) {
      inByte = Serial.read();
      Serial.write(inByte);
      break;
    }
  }
  readBufferClear();
  return (inByte);
}

void readBufferClear(void)
{
  // buffer clear
  while(Serial.available()) {
    Serial.read(); //dummy read
  }
}

bool readWifiPass(char *pass)
{
  bool retValue;
  int i=0;
  byte inByte; 
    
  readBufferClear();
  Serial.print("Enter Password > ");
  while(1){
     if (Serial.available()) {
      inByte = Serial.read();
      if(inByte>=0x21 && inByte<=0x7e){
        Serial.write(inByte);
        pass[i++]=inByte;
        if(i>=MAX_NUM_PASS){
          pass[i]='\0';
          retValue=false;
        }
      }
      else{
        pass[i]='\0';
        retValue=true;
        break;  
      }
     }
  }
  Serial.println("");
  readBufferClear();
  return (retValue);
}

void loop() {
  double dtemp;
  
  // put your main code here, to run repeatedly:
  digitalWrite(4, HIGH);
  delay(250);
  
  dtemp=Mcp9804.getTemperature();
  Serial.printf("temp_d: %.04f[C]\n\n",dtemp);

#ifdef USE_AMBIENT  
  ambient.set(1,dtemp);
  ambient.send();
#endif
  
  digitalWrite(4, LOW);
  delay(10000);
}
