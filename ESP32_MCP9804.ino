#include <Wire.h>
#include "MCP9804.h"

uint64_t chipid;  
uint16_t ManufactureID;
uint8_t DeviceID;
uint8_t Revision;

void setup() {
    byte error;
    
  // put your setup code here, to run once:
  pinMode(4, OUTPUT);
  pinMode(21, INPUT_PULLUP);
  pinMode(21, INPUT_PULLUP);

  Serial.begin(115200);

  chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
  Serial.printf("ESP32 Chip ID [%04x",(uint16_t)(chipid>>32));//print High 2 bytes
  Serial.printf("%08x]\n",(uint32_t)chipid);//print Low 4bytes.

  
  Wire.begin();
  Wire.beginTransmission(MCP9804_DEVICE_ADDR);
  error = Wire.endTransmission();
  if(error==0){
    Serial.printf("Found Device\n");
    MCP9804_read_devID();
  }
  else{
    Serial.printf("Not Found Device\n");    
    while(1);
  }

}



void MCP9804_read_devID(void)
{
  byte dataH,dataL,num;
 
  Wire.beginTransmission(MCP9804_DEVICE_ADDR);
  Wire.write(MCP9804_ADDR_MANUFACTURE_ID);
  Wire.endTransmission();
  Wire.requestFrom(MCP9804_DEVICE_ADDR,2);
  num=Wire.available();
  Serial.printf("num:%d\n",num);
  dataH=Wire.read(); 
  dataL=Wire.read();
  
  ManufactureID = (dataH << 8) | dataL; 
  Serial.printf("MCP9804: Manufacture ID[0x%04x]\n",ManufactureID);

  Wire.beginTransmission(MCP9804_DEVICE_ADDR);
  Wire.write(MCP9804_ADDR_DEVICE_ID);
  Wire.endTransmission();
  Wire.requestFrom(MCP9804_DEVICE_ADDR,2);
  num=Wire.available();
  Serial.printf("num:%d\n",num);
  DeviceID=Wire.read(); 
  Revision=Wire.read();
  Serial.printf("MCP9804: Device ID[0x%02x]\n",DeviceID);
  Serial.printf("MCP9804: Revision [0x%02x]\n",Revision);  
}

uint16_t MCP9804_read_temp(void)
{
  uint8_t     UpperByte;
  uint8_t     LowerByte;
  uint8_t     temperature;
  uint32_t    tempeature_point;
  uint16_t  temp;
  byte num;

  Wire.beginTransmission(MCP9804_DEVICE_ADDR);
  Wire.write(MCP9804_ADDR_TEMPERATURE);
  Wire.endTransmission();
  Wire.requestFrom(MCP9804_DEVICE_ADDR,2);
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
  
    tempeature_point = (uint32_t)625*(uint32_t)(LowerByte&0x0f);

    printf("Temprature: %u.%04u[C]\r\n\r\n",temperature,tempeature_point);
    

  return (temp);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(4, HIGH);

  MCP9804_read_temp();    
  
  delay(1500);
  digitalWrite(4, LOW);
  delay(1500);
}
