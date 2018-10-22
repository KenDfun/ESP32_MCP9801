#include "MCP9804.h"

#include <Wire.h>

MCP9804::MCP9804()
{
}

bool MCP9804::begin(uint8_t addr)
{
  byte error;
  
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
  uint16_t i2cData;
 
  i2cData=this->readI2CData(MCP9804_ADDR_DEVICE_ID);

  return ((uint8_t)(i2cData>>8));
}

uint8_t MCP9804::getRevision(void)
{
  uint16_t i2cData;
 
  i2cData=this->readI2CData(MCP9804_ADDR_DEVICE_ID);


  return ((uint8_t)(i2cData&0x00ff));
}

uint16_t MCP9804::getManufacture(void)
{
  uint16_t manufactureID;

  manufactureID=this->readI2CData(MCP9804_ADDR_MANUFACTURE_ID);
  
  return (manufactureID);
}



double MCP9804::getTemperature(void)
{
  uint8_t     UpperByte;
  uint8_t     LowerByte;
  uint8_t     temperature;
  uint32_t    temperature_point;
  uint16_t  temp;
  double temp_d;

  temp=this->readI2CData(MCP9804_ADDR_TEMPERATURE);
  UpperByte = (uint8_t)(temp>>8);
  LowerByte = (uint8_t)(temp&0x00ff);
  
//  Serial.printf("MCP9804: temperature [0x%04x]\n",temp);

  if(UpperByte&0x80){
      Serial.printf("Warning: Temperature Critical\r\n");
  }
  if(UpperByte&0x40){
      Serial.printf("Warning: Temperature Upper\r\n");
  }
  if(UpperByte&0x20){
      Serial.printf("Warning: Temperature Lower\r\n");
  }
  
  UpperByte = UpperByte & 0x1F; //Clear flag bits
  if ((UpperByte & 0x10) == 0x10){ //TA < 0°C
      UpperByte = UpperByte & 0x0F; //Clear SIGN
      temperature = 256 - ((UpperByte << 4) + (LowerByte >> 4));
  }
  else{ //TA ≥ 0°C
      temperature = ((UpperByte << 4) + (LowerByte >> 4));
  }
  
  temperature_point = (uint32_t)625*(uint32_t)(LowerByte&0x0f);
  Serial.printf("\n");
//  Serial.printf("Temprature: %u.%04u[C]\n",temperature,temperature_point);
  
  temp_d = (double)temperature+(temperature_point/10000.0);

  return (temp_d);
}

uint16_t MCP9804::readI2CData(uint8_t subaddr)
{
  int num;
  uint8_t dataH,dataL;
  
  Wire.beginTransmission(this->i2cAddr);
  Wire.write(subaddr);
  Wire.endTransmission();
  Wire.requestFrom(this->i2cAddr,(uint8_t)2);
  num=Wire.available();
  if(num!=2){
    Serial.printf("Error! read data num:%d\n",num);
    while(1);
  }
  dataH=Wire.read(); 
  dataL=Wire.read();

  return ((uint16_t)(dataH<<8)|(uint16_t)dataL);
}
