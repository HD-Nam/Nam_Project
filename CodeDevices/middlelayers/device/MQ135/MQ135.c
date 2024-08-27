#include "MQ135.h"

// Thiết lập chân
void MQ135_init(MQ135* mq135, ADC_HandleTypeDef* hadc, uint32_t adcChannel) {
    mq135->hadc = hadc;         // Gán cấu trúc ADC
    mq135->adcChannel = adcChannel; // Gán kênh ADC
}

// Hàm tính toán yếu tố hiệu chỉnh dựa trên nhiệt độ và độ ẩm
/*
Tuyến tính hóa đường cong phụ thuộc nhiệt độ dưới và trên 20 độ C
    dưới 20degC: Fact = a * t * t - b * t - (h - 33) * d
    trên 20degC: Fact = a * t + b * h + c
điều này giả định sự phụ thuộc tuyến tính vào độ ẩm
*/
float MQ135_getCorrectionFactor(MQ135* mq135, float t, float h) {
    if (t < 20) {
        // Tính toán yếu tố hiệu chỉnh cho nhiệt độ dưới 20 độ C
        return CORA * t * t - CORB * t + CORC - (h - 33.0) * CORD;
    } else {
        // Tính toán yếu tố hiệu chỉnh cho nhiệt độ trên 20 độ C
        return CORE * t + CORF * h + CORG;
    }
}

// Hàm đọc giá trị trở kháng của cảm biến
float MQ135_getResistance(MQ135* mq135) {
    ADC_ChannelConfTypeDef sConfig = {0};	// Khai báo và khởi tạo một cấu trúc cấu hình cho kênh ADC
    sConfig.Channel = mq135->adcChannel;	// Đặt kênh ADC theo kênh được chỉ định trong cấu trúc MQ135.
    sConfig.Rank = ADC_REGULAR_RANK_1;		// Đặt thứ tự chuyển đổi của kênh ADC. Điều này xác định thứ tự chuyển đổi trong nhóm các kênh thông thường.
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;		  // Đặt thời gian lấy mẫu cho quá trình chuyển đổi ADC. Ở đây, 1.5 chu kỳ được sử dụng cho thời gian chuyển đổi, điều này ảnh hưởng đến tốc độ và độ chính xác của quá trình chuyển đổi.
    HAL_ADC_ConfigChannel(mq135->hadc, &sConfig);		// Cấu hình ADC với cấu hình kênh đã chỉ định.

    HAL_ADC_Start(mq135->hadc);		// Bắt đầu quá trình chuyển đổi ADC
    HAL_ADC_PollForConversion(mq135->hadc, HAL_MAX_DELAY);	 // Đợi cho đến khi quá trình chuyển đổi ADC hoàn thành. Hàm này chặn cho đến khi chuyển đổi xong.
    int val = HAL_ADC_GetValue(mq135->hadc);	 // Lấy giá trị ADC đã được chuyển đổi. Đây là giá trị số hóa của điện áp analog từ cảm biến.
    HAL_ADC_Stop(mq135->hadc);		// Dừng quá trình chuyển đổi ADC.

    // Tính toán và trả về giá trị điện trở của cảm biến MQ135.
    // Công thức: ((4095 / ADC_value) * 5V - 1) * RLOAD
    // - 4095 là giá trị tối đa của ADC 12-bit.
    // - 5V là điện áp cung cấp giả định.
    // - RLOAD là giá trị của điện trở tải.
    return ((4095.0 / (float)val) * 5.0 - 1.0) * RLOAD;
}

// Hàm tính toán giá trị trở kháng của cảm biến được hiệu chỉnh dựa trên nhiệt độ và độ ẩm
float MQ135_getCorrectedResistance(MQ135* mq135, float t, float h) {
    // Chia giá trị trở kháng đo được cho yếu tố hiệu chỉnh
    return MQ135_getResistance(mq135) / MQ135_getCorrectionFactor(mq135, t, h);
}

// Hàm tính toán nồng độ CO2 dựa trên giá trị trở kháng của cảm biến
float MQ135_getCO2PPM(MQ135* mq135) {
    // Tính toán nồng độ CO2 (ppm) sử dụng công thức đã được hiệu chỉnh
    return CO2_PARA * pow((MQ135_getResistance(mq135) / RZERO), -CO2_PARB);
}

// Hàm tính toán nồng độ CO2 dựa trên giá trị trở kháng được hiệu chỉnh của cảm biến
float MQ135_getCorrectedCO2PPM(MQ135* mq135, float t, float h) {
    // Tính toán nồng độ CO2 (ppm) sử dụng giá trị trở kháng đã được hiệu chỉnh
	mq135->Co2 = CO2_PARA * pow((MQ135_getCorrectedResistance(mq135, t, h) / RZERO), -CO2_PARB);
    return mq135->Co2;
}

// Hàm tính toán giá trị trở kháng RZero_CO2 của cảm biến để hiệu chỉnh
float MQ135_getRZeroCO2(MQ135* mq135) {
    // Tính toán giá trị RZero dựa trên mức CO2 trong khí quyển
    return MQ135_getResistance(mq135) * pow((ATMOCO2 / CO2_PARA), (1.0 / CO2_PARB));
}

// Hàm tính toán giá trị trở kháng RZero_CO2 được hiệu chỉnh của cảm biến để hiệu chỉnh
float MQ135_getCorrectedRZeroCO2(MQ135* mq135, float t, float h) {
    // Tính toán giá trị RZero đã được hiệu chỉnh dựa trên mức CO2 trong khí quyển
    return MQ135_getCorrectedResistance(mq135, t, h) * pow((ATMOCO2 / CO2_PARA), (1.0 / CO2_PARB));
}



// Hàm tính tỷ lệ Rs/R0 để tìm giá trị ppm của khí CO
float MQ135_getRatio(MQ135* mq135) {
    float resistance = MQ135_getResistance(mq135);
    if (resistance < 0) {
        return -1.0; // Tránh giá trị không hợp lệ
    }
    return resistance / RZERO; // Tính tỷ lệ Rs/R0
}

// Hàm trả về giá trị ppm của nồng độ khi CO
float MQ135_getCOPPM(MQ135* mq135) {
    float ratio = MQ135_getRatio(mq135);    // Tính tỷ lệ Rs/R0
    if (ratio < 0) {
        return -1.0;    // Tránh chia cho 0 hoặc giá trị không hợp lệ
    }
    mq135->CO = COEFFICIENT_A * pow(ratio, COEFFICIENT_B);
    // Tính và trả về giá trị ppm của CO
    return mq135->CO;
}

// Hàm tìm điện trở của cảm biến Rs (CO)
float MQ135_getSensorResistance(MQ135* mq135) {
    return MQ135_getResistance(mq135);
}

