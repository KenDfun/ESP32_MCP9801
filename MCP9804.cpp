#include "MCP9804.h"

#include <Wire.h>

MCP9804::MCP9804()
{
}

bool MCP9804::begin(uint8_t addr)
{
  Wire.begin();
  Wire.beginTransmission(addr);
  error = Wire.endTransmission();
  if(error==0){
    Serial.printf("Found Device\n");
  }
  else{
    Serial.printf("Not Found Device\n");    
    return (false);
  }
  this->i2cAddr = addr;

  return (true);
}

uint8_t MCP9804::getDevID(void)
{
  byte dataH,dataL,num;
 
  Wire.beginTransmission(MCP9804_DEVICE_ADDR);
  Wire.write(MCP9804_ADDR_DEVICE_ID);
  Wire.endTransmission();
  Wire.requestFrom(MCP9804_DEVICE_ADDR,2);
  num=Wire.available();
  Serial.printf("num:%d\n",num);
  dataH=Wire.read(); 
  dataL=Wire.read();
  Serial.printf("MCP9804: Device ID[0x%02x]\n",dataH);

  return (dataH);
}

uint8_t MCP9804::getRev(void)
{
  byte dataH,dataL,num;
 
  Wire.beginTransmission(MCP9804_DEVICE_ADDR);
  Wire.write(MCP9804_ADDR_DEVICE_ID);
  Wire.endTransmission();
  Wire.requestFrom(MCP9804_DEVICE_ADDR,2);
  num=Wire.available();
  Serial.printf("num:%d\n",num);
  dataH=Wire.read(); 
  dataL=Wire.read();

  Serial.printf("MCP9804: Revision [0x%02x]\n",dataL);  

  return (dataL);
}

uint16_t MCP9804::getManufacture(void)
{
  uint16_t manufactureID;
  
  Wire.beginTransmission(MCP9804_DEVICE_ADDR);
  Wire.write(MCP9804_ADDR_MANUFACTURE_ID);
  Wire.endTransmission();
  Wire.requestFrom(MCP9804_DEVICE_ADDR,2);
  num=Wire.available();
  Serial.printf("num:%d\n",num);
  dataH=Wire.read(); 
  dataL=Wire.read();
  
  manufactureID = (dataH << 8) | dataL; 
  Serial.printf("MCP9804: Manufacture ID[0x%04x]\n",manufactureID);

  return (manufactureID);
  
}
