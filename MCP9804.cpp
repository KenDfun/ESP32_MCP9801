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
  int num;
  uint8_t dataH,dataL;
 
  Wire.beginTransmission(this->i2cAddr);
  Wire.write(MCP9804_ADDR_DEVICE_ID);
  Wire.endTransmission();
  Wire.requestFrom(this->i2cAddr,(uint8_t)2);
  num=Wire.available();
  Serial.printf("num:%d\n",num);
  dataH=Wire.read(); 
  dataL=Wire.read();
  Serial.printf("MCP9804: Device ID[0x%02x]\n",dataH);

  return (dataH);
}

uint8_t MCP9804::getRevision(void)
{
  int num;
  uint8_t dataH,dataL;
 
  Wire.beginTransmission(this->i2cAddr);
  Wire.write(MCP9804_ADDR_DEVICE_ID);
  Wire.endTransmission();
  Wire.requestFrom(this->i2cAddr,(uint8_t)2);
  num=Wire.available();
  Serial.printf("num:%d\n",num);
  dataH=Wire.read(); 
  dataL=Wire.read();

  Serial.printf("MCP9804: Revision [0x%02x]\n",dataL);  

  return (dataL);
}

uint16_t MCP9804::getManufacture(void)
{
  int num;
  uint8_t dataH,dataL;
  uint16_t manufactureID;
  
  Wire.beginTransmission(this->i2cAddr);
  Wire.write(MCP9804_ADDR_MANUFACTURE_ID);
  Wire.endTransmission();
  Wire.requestFrom(this->i2cAddr,(uint8_t)2);
  num=Wire.available();
  Serial.printf("num:%d\n",num);
  dataH=Wire.read(); 
  dataL=Wire.read();
  
  manufactureID = (dataH << 8) | dataL; 
  Serial.printf("MCP9804: Manufacture ID[0x%04x]\n",manufactureID);

  return (manufactureID);
  
}



double MCP9804::getTemperature(void)
{
  uint8_t     UpperByte;
  uint8_t     LowerByte;
  uint8_t     temperature;
  uint32_t    temperature_point;
  uint16_t  temp;
  byte num;
  double temp_d;

  Wire.beginTransmission(this->i2cAddr);
  Wire.write(MCP9804_ADDR_TEMPERATURE);
  Wire.endTransmission();
  Wire.requestFrom(this->i2cAddr,(uint8_t)2);
  num=Wire.available();
  if(num!=2){
    Serial.printf("I2C failed @ MCP9804_read_temp\n");
    while(1);
  }

  UpperByte=Wire.read(); 
  LowerByte=Wire.read();
  temp = (UpperByte << 8) | LowerByte; 
  Serial.printf("MCP9804: temperature [0x%04x]\n",temp);

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

    printf("Temprature: %u.%04u[C]\n",temperature,temperature_point);
    temp_d = (double)temperature+(temperature_point/10000.0);
    printf("temp_d: %f[C]\n\n",temp_d);



  return (temp_d);
}
