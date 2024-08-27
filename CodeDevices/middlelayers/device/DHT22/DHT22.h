/*
 * Library for temperature and humidity values by using DHT22 sensor
 * Datasheet: https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf
 * Algorithm: https://www.teachmemicro.com/how-dht22-sensor-works/
*/

#ifndef DHT22_H
#define DHT22_H

#include "main.h"

// Giá trị trả về khi kiểm tra lỗi trong quá trình xử lý DHT22
typedef enum {
    ERROR_NONE = 0, // Không có lỗi
    ERROR_TIMEOUT,  // Lỗi timeout
    ERROR_CHECKSUM  // Lỗi checksum
} DHT_ERROR_t;

// Cấu trúc đối tượng DHT22
typedef struct {
    DHT_ERROR_t error;          // Mã lỗi
    uint32_t lastReadTime;      // Thời gian cuối cùng cảm biến được đọc
    float temperature;          // Nhiệt độ
    float humidity;             // Độ ẩm
    GPIO_TypeDef *GPIOx;        // Cổng GPIO
    uint16_t GPIO_Pin;          // Chân GPIO
} DHT22;

void DHT22_init(DHT22 *dht, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void DHT22_setup(DHT22 *dht, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void DHT22_resetTimer(DHT22 *dht);
DHT_ERROR_t DHT22_getStatus(DHT22 *dht);
void DHT22_readSensor(DHT22 *dht);
float DHT22_getTemperature(DHT22 *dht);
float DHT22_getHumidity(DHT22 *dht);

#endif // DHT22_H

