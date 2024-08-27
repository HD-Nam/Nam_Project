/*
 * Library for dust density value by using PM2.5 GP2Y1010AU0F sensor
 * Datasheet: https://www.sparkfun.com/datasheets/Sensors/gp2y1010au_e.pdf
 * Reference: http://arduino.vn/tutorial/6073-su-dung-cam-bien-bui-sharp-gp2y10
 * Algorithm: https://github.com/lnquy065/OpticalDustSensor
*/

#ifndef PM25_H
#define PM25_H

#include "main.h"

// Mỗi xung lấy mẫu là 10ms
// Mỗi xung bật LED là 0.32ms, trong đó:
//  + 0.28ms: Thời gian lấy mẫu.
//  + 0.04ms: Thời gian không lấy mẫu.
// => Quy trình:
//  + Bật LED, delay 0.28ms
//  + Đọc Input, delay 0.04ms
//  + Tắt LED, delay 9.680ms
#define PM25_SAMPLINGTIME 280        // Định nghĩa thời gian lấy mẫu là 280 microseconds
#define PM25_DELTATIME 40            // Định nghĩa thời gian delta là 40 microseconds
#define PM25_SLEEPINGTIME 9680       // Định nghĩa thời gian ngủ là 9680 microseconds

typedef struct {
    GPIO_TypeDef *voPort;        // Port của chân Vo của cảm biến
    uint16_t voPin;              // Chân Vo của cảm biến
    GPIO_TypeDef *ledPort;       // Port của chân LED của cảm biến
    uint16_t ledPin;             // Chân LED của cảm biến
    float dustDensity;           // Mật độ bụi đo được
    float volMeasured;           // Điện áp đo được từ chân Vo
    float calcVoltage;           // Điện áp tính toán từ giá trị ADC
    float vccVol;                // Điện áp cung cấp cho cảm biến
} PM25;

void PM25_init(PM25 *pm25, GPIO_TypeDef *voPort, uint16_t voPin, GPIO_TypeDef *ledPort, uint16_t ledPin);
void PM25_calc(PM25 *pm25);

float PM25_getDustDensity(PM25 *pm25);
float PM25_getVoltage(PM25 *pm25);
uint32_t PM25_getADC(PM25 *pm25);

float PM25_getDustDensityField(PM25 *pm25);
float PM25_getVoltageField(PM25 *pm25);
uint32_t PM25_getADCField(PM25 *sensor);

#endif // PM25_H

