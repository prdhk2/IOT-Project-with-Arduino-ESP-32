#include <ModbusMaster.h>
#include <SoftwareSerial.h>

#define baudrate 4800
#define timeout 100
#define polling 100
#define retry_count 10

ModbusMaster node;
SoftwareSerial Serial2(8, 9);

#define MAX485_DE      2
#define MAX485_RE_NEG  3

#define ID_meter  4
#define Total_of_Reg  1 

#define Reg_E       0x0E00 

float DATA_METER [Total_of_Reg];

uint16_t Reg_addr[Total_of_Reg] = {
  Reg_E,
};

void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 2);
  digitalWrite(MAX485_DE, 3);
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 2);
  digitalWrite(MAX485_DE, 3);
}

float HexToFloat(uint32_t x);

void setup() 
{
  Serial.begin(baudrate);
  Serial2.begin(baudrate);
  Serial.println(F("Test"));
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  // Init in receive mode
  digitalWrite(MAX485_RE_NEG, LOW);
  digitalWrite(MAX485_DE, LOW);
}

void loop() 
{
  delay(1000);                            
  for (char i = 0; i < Total_of_Reg; i++)
  {
    DATA_METER[i] = Read_Meter_float(ID_meter, Reg_addr[i]);
  } 
  
  Serial.println();
  Serial.print("Data = "); Serial.print(DATA_METER[4]);
}

float Read_Meter_float(char addr, uint16_t REG) 
{
  float i = 0;
  uint8_t result, j;

  uint16_t data[5];
  uint32_t value = 0;
  node.begin(ID_meter, Serial2);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  
  result = node.readHoldingRegisters(REG, 4);
  delay(500);
  if (result == node.ku8MBSuccess) 
  {
    for (j = 0; j < 4; j++)
    {
      data[j] = node.getResponseBuffer(j);
    }
    value = (uint32_t)data[2] << 16 | data[3]; 
    i = HexToFloat(value);
    Serial.println("Connect modbus Ok.");
    return i;
  } else 
  {
    Serial.print("Connect modbus fail. REG >>> "); Serial.println(REG); // Debug
    delay(1000); 
    return 0.0;
  }
}

float HexToFloat(uint32_t x) 
{
  return (*(float*)&x);
}
