
#include "crc.h"

static uint8_t Crc8_CcitUpdate(uint8_t inCrc, uint8_t inData)
{
	
  uint8_t data;
  data = inCrc ^ inData;

  for ( uint8_t CountBit = 0; CountBit < 8; CountBit++ )
  {
    if (( data & 0x80 ) != 0 )
    {
      data <<= 1;
      data ^= 0x07;
    }
    else data <<= 1;
  }

  return data;
}

uint8_t Crc8_CalCcit(uint8_t *data, uint16_t length)
{
   uint8_t byteCrc = 0;
   
   for(uint8_t Countbyte = 0; Countbyte < length; Countbyte ++)
   {
     byteCrc = Crc8_CcitUpdate(byteCrc, data[Countbyte]);
   }
   return byteCrc;
}

uint16_t Get_CRC16_Modbus(uint8_t *Data, uint8_t Length)
{
    uint16_t Crc = 0xFFFF;
    if(Length == 0)
    {
        return 0;
    }
    else
    {
        uint8_t count = 1;
        for(count = 1; count < Length; count ++)
        {
        	Crc ^= *(Data + count);
            for(uint8_t i = 0; i < 8; i ++)
            {
                if((Crc & 0x01) !=0)
                {
                	Crc >>= 1;
                	Crc ^= 0xA001;
                }
                else
                {
                	Crc >>= 1;
                }
            }
        }
        return Crc;
    }
}




