#ifndef MCP9804_H
#define MCP9804_H



#define MCP9804_ADDR_T_CONFIG        0x01
#define MCP9804_ADDR_T_UPPER         0x02
#define MCP9804_ADDR_T_LOWER         0x03
#define MCP9804_ADDR_T_CRIT          0x04
#define MCP9804_ADDR_TEMPERATURE     0x05
#define MCP9804_ADDR_MANUFACTURE_ID  0x06
#define MCP9804_ADDR_DEVICE_ID       0x07

class MCP9804
{
  public:
    MCP9804(void);

    bool begin(uint8_t addr);
    uint16_t getDevIDRev(void);
    uint8_t getManufacture(void);
    uint8_t getTemperature(void);

  private:
    uint8_t i2cAddr;
}

#endif
