#include "PM25.h"

extern ADC_HandleTypeDef hadc1;

// Khởi tạo cảm biến mà không cần chỉ định điện áp cung cấp (mặc định là 5.0V)
void PM25_init(PM25 *pm25, GPIO_TypeDef *voPort, uint16_t voPin, GPIO_TypeDef *ledPort, uint16_t ledPin) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};  // Khởi tạo cấu trúc GPIO_InitTypeDef với tất cả các thành phần bằng 0

    // Đặt chân LED là đầu ra
    GPIO_InitStruct.Pin = ledPin;  // Cấu hình chân GPIO cho LED
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // Đặt chế độ của chân GPIO là chế độ đầu ra đẩy-kéo (output push-pull)
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;  // Đặt tốc độ của chân GPIO là tốc độ thấp
    HAL_GPIO_Init(ledPort, &GPIO_InitStruct);  // Khởi tạo chân GPIO với các thông số đã cấu hình

    // Gán các giá trị cho các thành phần của cấu trúc PM25
    pm25->voPort = voPort;  // Gán giá trị cổng GPIO sử dụng để đọc điện áp từ cảm biến bụi
    pm25->voPin = voPin;  // Gán giá trị chân GPIO sử dụng để đọc điện áp từ cảm biến bụi
    pm25->ledPort = ledPort;  // Gán giá trị cổng GPIO sử dụng để điều khiển LED
    pm25->ledPin = ledPin;  // Gán giá trị chân GPIO sử dụng để điều khiển LED
    pm25->vccVol = 5.0;  // Đặt điện áp cung cấp cho cảm biến là 5.0V
    pm25->dustDensity = 0;  // Khởi tạo mật độ bụi đo được là 0
    pm25->volMeasured = 0;  // Khởi tạo giá trị điện áp đo được là 0
    pm25->calcVoltage = 0;  // Khởi tạo giá trị điện áp tính toán là 0
}

// Tính toán mật độ bụi dựa trên các phép đo của cảm biến
void PM25_calc(PM25 *pm25) {
    HAL_GPIO_WritePin(pm25->ledPort, pm25->ledPin, GPIO_PIN_RESET); // Bật LED
    HAL_Delay(PM25_SAMPLINGTIME / 1000); // Đợi thời gian lấy mẫu (chuyển đổi từ us sang ms)
	ADC_ChannelConfTypeDef sConfig = {0};	// Cấu hình kênh ADC
	  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	  */
	  sConfig.Channel = ADC_CHANNEL_2;	  // Chọn kênh ADC 2 để đọc tín hiệu từ cảm biến
	  sConfig.Rank = 2;			 // Xác định thứ tự cho kênh ADC
	  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;	 // Chọn thời gian lấy mẫu là 1.5 chu kỳ

	  // Cấu hình ADC với các thiết lập trên và kiểm tra lỗi
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();	// Xử lý lỗi nếu cấu hình ADC không thành công
	  }
	  HAL_ADC_Start(&hadc1);	// Bắt đầu quá trình chuyển đổi ADC
	  HAL_ADC_PollForConversion(&hadc1, 1000);	 // Chờ cho quá trình chuyển đổi ADC hoàn thành, với timeout là 1000 ms
    pm25->volMeasured = HAL_ADC_GetValue(&hadc1); // Đọc giá trị analog từ chân Vo
    HAL_ADC_Stop(&hadc1);	   // Dừng quá trình ADC

    HAL_Delay(PM25_DELTATIME / 1000); // Đợi thời gian delta (giữa các lần đo)
    HAL_GPIO_WritePin(pm25->ledPort, pm25->ledPin, GPIO_PIN_SET); // Tắt LED
    HAL_Delay(PM25_SLEEPINGTIME / 1000); // Đợi thời gian ngủ trước khi bắt đầu chu kỳ đo tiếp theo

    // Tính toán điện áp thực tế từ giá trị ADC
    pm25->calcVoltage = pm25->volMeasured * (pm25->vccVol / 4096.0);

    // Tính toán mật độ bụi sử dụng phương trình tuyến tính được cung cấp bởi Chris Nafis
    pm25->dustDensity = 0.17 * pm25->calcVoltage - 0.1;

    // Đảm bảo rằng mật độ bụi không âm, nếu âm thì gán giá trị bằng 0
    if (pm25->dustDensity < 0) {
        pm25->dustDensity = 0.00;
    }
}

// Lấy giá trị mật độ bụi (kích hoạt tính toán)
// Đơn vị (ug/m3)
/*
0 - 12 µg/m³: Tốt
12.1 - 35.4 µg/m³: Trung bình
35.5 - 55.4 µg/m³: Không tốt cho nhóm nhạy cảm
55.5 - 150.4 µg/m³: Không tốt
150.5 - 250.4 µg/m³: Rất không tốt
250.5 µg/m³: Nguy hiểm
*/
float PM25_getDustDensity(PM25 *pm25) {
    PM25_calc(pm25); // Tính toán các giá trị
    return pm25->dustDensity; // Trả về mật độ bụi
}

// Lấy giá trị điện áp tính toán (kích hoạt tính toán)
float PM25_getVoltage(PM25 *pm25) {
    PM25_calc(pm25); // Tính toán các giá trị
    return pm25->calcVoltage; // Trả về điện áp tính toán
}

// Lấy giá trị ADC thô (kích hoạt tính toán)
uint32_t PM25_getADC(PM25 *pm25) {
    PM25_calc(pm25); // Tính toán các giá trị
    return pm25->volMeasured; // Trả về giá trị ADC thô
}

// Lấy giá trị mật độ bụi mà không kích hoạt tính toán mới
float PM25_getDustDensityField(PM25 *pm25) {
    return pm25->dustDensity; // Trả về mật độ bụi đã lưu trữ
}

// Lấy giá trị điện áp tính toán mà không kích hoạt tính toán mới
float PM25_getVoltageField(PM25 *pm25) {
    return pm25->calcVoltage; // Trả về điện áp tính toán đã lưu trữ
}

// Lấy giá trị ADC thô mà không kích hoạt tính toán mới
uint32_t PM25_getADCField(PM25 *pm25) {
    return pm25->volMeasured; // Trả về giá trị ADC thô đã lưu trữ
}

