/*
 * Datasheet: https://www.olimex.com/Products/Components/Sensors/Gas/SNS-MQ135/resources/SNS-MQ135.pdf
*/

#ifndef MQ135_H
#define MQ135_H

#include <math.h>
#include "main.h"

// Khả năng chịu tải trên board cho CO2 và CO
#define RLOAD 20.0
//#define RLOAD 44.0

// Điện trở hiệu chuẩn ở mức CO2 trong khí quyển
#define RZERO 77.63

// Định nghĩa các hằng số cho việc tính toán nồng độ CO2 từ giá trị trở kháng của cảm biến
#define CO2_PARA 116.6020682
#define CO2_PARB -2.769034857

// Định nghĩa các hằng số để mô hình hóa sự phụ thuộc vào nhiệt độ và độ ẩm (CO2)
#define CORA 0.00035
#define CORB 0.02718
#define CORC 1.39538
#define CORD 0.0018
#define CORE -0.003333333
#define CORF -0.001923077
#define CORG 1.130128205

// Mức CO2 trong không khí cho mục đích hiệu chỉnh
#define ATMOCO2 422.86      // Mức CO2 toàn cầu vào tháng 2 2024

// Định nghĩa các hằng số cho việc tính toán nồng độ CO
#define COEFFICIENT_A 19.32       // Hệ số a trong công thức tính ppm cho CO
#define COEFFICIENT_B -2.64       // Hệ số b trong công thức tính ppm cho CO

// Cấu trúc MQ135 để lưu trữ thông tin về cảm biến
typedef struct {
    ADC_HandleTypeDef* hadc;  // Con trỏ đến cấu trúc ADC
    uint32_t adcChannel;      // Kênh ADC cho giá trị đo của cảm biến
    float Co2;
    float CO;
} MQ135;

// Khai báo các hàm của thư viện (CO2)
void MQ135_init(MQ135* mq135, ADC_HandleTypeDef* hadc, uint32_t adcChannel);
float MQ135_getCorrectionFactor(MQ135* mq135, float t, float h);
float MQ135_getResistance(MQ135* mq135);
float MQ135_getCorrectedResistance(MQ135* mq135, float t, float h);
float MQ135_getCO2PPM(MQ135* mq135);
float MQ135_getCorrectedCO2PPM(MQ135* mq135, float t, float h);
float MQ135_getRZeroCO2(MQ135* mq135);
float MQ135_getCorrectedRZeroCO2(MQ135* mq135, float t, float h);

// Khai báo các hàm của thư viện (CO)
float MQ135_getRatio(MQ135* mq135);
float MQ135_getCOPPM(MQ135* mq135);
float MQ135_getSensorResistance(MQ135* mq135);

#endif // MQ135_H

