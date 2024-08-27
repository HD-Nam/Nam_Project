#include "DHT22.h"

// Hàm khởi tạo 1 đối tượng DHT22 mới
void DHT22_init(DHT22 *dht, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    dht->GPIOx = GPIOx;			// Gán chân kết nối cảm biến
    dht->GPIO_Pin = GPIO_Pin;	// Gán port kết nối cảm biến
    DHT22_resetTimer(dht);	// Gọi hàm thiết lập hẹn giờ cho cảm biến

    // Kích hoạt DWT để đếm chu kỳ
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {  // Kiểm tra nếu DWT được kích hoạt chưa
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  // Kích hoạt DWT
        DWT->CYCCNT = 0;  // Đặt lại bộ đếm chu kỳ
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;  // Bật bộ đếm chu kỳ
    }
}

// Thiết lập chân kết nối với vi điều khiển
void DHT22_setup(DHT22 *dht, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    dht->GPIOx = GPIOx;
    dht->GPIO_Pin = GPIO_Pin;
    DHT22_resetTimer(dht);
}

// Kiểm tra cảm biến hoạt động
DHT_ERROR_t DHT22_getStatus(DHT22 *dht) {
    return dht->error;
}

// Đặt lại hẹn giờ cho việc cài đặt sensor
void DHT22_resetTimer(DHT22 *dht) {
    dht->lastReadTime = HAL_GetTick() - 3000;	// Đặt lại hẹn giờ cho việc đọc cảm biến
}

// Đọc giá trị cảm biến đã đo
void DHT22_readSensor(DHT22 *dht) {
    // Đảm bảo rằng không quét cảm biến quá thường xuyên
    uint32_t startTime = HAL_GetTick();  // Lấy thời gian hiện tại

    dht->lastReadTime = startTime;  // Cập nhật thời gian lần cuối đọc cảm biến

    uint16_t rawHumidity = 0;  // Khởi tạo biến độ ẩm gốc
    uint16_t rawTemperature = 0;  // Khởi tạo biến nhiệt độ gốc
    uint16_t data = 0;  // Khởi tạo biến dữ liệu

    // Yêu cầu mẫu
    HAL_GPIO_WritePin(dht->GPIOx, dht->GPIO_Pin, GPIO_PIN_RESET);  // Đặt chân GPIO xuống mức thấp
    GPIO_InitTypeDef GPIO_InitStruct = {0};  // Khởi tạo cấu trúc GPIO
    GPIO_InitStruct.Pin = dht->GPIO_Pin;  // Chỉ định chân GPIO
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // Chế độ đầu ra push-pull
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;  // Tốc độ thấp
    HAL_GPIO_Init(dht->GPIOx, &GPIO_InitStruct);  // Khởi tạo GPIO với cấu hình trên

    HAL_Delay(2);  // Delay 2ms

    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;  // Chuyển chế độ chân GPIO sang đầu vào
    GPIO_InitStruct.Pull = GPIO_NOPULL;  // Không sử dụng điện trở kéo lên/kéo xuống
    HAL_GPIO_Init(dht->GPIOx, &GPIO_InitStruct);  // Khởi tạo lại GPIO với chế độ đầu vào

    HAL_GPIO_WritePin(dht->GPIOx, dht->GPIO_Pin, GPIO_PIN_SET);  // Đặt chân GPIO lên mức cao

    // Vòng lặp qua số lượng edges dự kiến
    for (int8_t i = -3; i < 2 * 40; i++) {  // Vòng lặp để đọc 40 bit dữ liệu từ cảm biến
        uint8_t age;
        startTime = DWT->CYCCNT;  // Lấy thời gian hiện tại của bộ đếm chu kỳ

        do {
            age = (DWT->CYCCNT - startTime) / (HAL_RCC_GetHCLKFreq() / 1000000);  // Chia số chu kỳ đã trôi qua cho tần số của HCLK để tính toán thời gian trôi qua tính bằng micro giây
            if (age > 90) {  // Nếu thời gian trôi qua lớn hơn 90 micro giây, báo lỗi timeout
                dht->error = ERROR_TIMEOUT;
                return;
            }
        } while (HAL_GPIO_ReadPin(dht->GPIOx, dht->GPIO_Pin) == (i & 1) ? GPIO_PIN_SET : GPIO_PIN_RESET);  // Đợi cho đến khi trạng thái chân GPIO thay đổi

        if (i >= 0 && (i & 1)) {  // Nếu i >= 0 và i lẻ
            data <<= 1;  // Dịch bit trái dữ liệu

            if (age > 30) {  // Nếu thời gian trôi qua lớn hơn 30 micro giây, bit hiện tại là 1
                data |= 1;
            }
        }

        switch (i) {
            case 31:
                rawHumidity = data;  // Lưu dữ liệu độ ẩm
                break;
            case 63:
                rawTemperature = data;  // Lưu dữ liệu nhiệt độ
                data = 0;  // Đặt lại dữ liệu
                break;
        }
    }

    // Xác minh checksum
    if ((uint8_t)(((uint8_t)rawHumidity) + (rawHumidity >> 8) + ((uint8_t)rawTemperature) + (rawTemperature >> 8)) != data) {  // Kiểm tra tổng kiểm tra
        dht->error = ERROR_CHECKSUM;  // Nếu không khớp, báo lỗi checksum
        return;
    }

    // Lưu trữ giá trị đọc
    dht->humidity = rawHumidity * 0.1;  // Chuyển đổi và lưu giá trị độ ẩm
    if (rawTemperature & 0x8000)  // Nếu nhiệt độ âm
        rawTemperature = -(int16_t)(rawTemperature & 0x7FFF);  // Lấy giá trị âm của nhiệt độ
    dht->temperature = ((int16_t)rawTemperature) * 0.1;  // Chuyển đổi và lưu giá trị nhiệt độ

    dht->error = ERROR_NONE;  // Không có lỗi
}

// Lấy giá trị nhiệt độ được đo
float DHT22_getTemperature(DHT22 *dht) {
    DHT22_readSensor(dht);
    if (dht->error == ERROR_TIMEOUT)
        DHT22_readSensor(dht);
    return dht->temperature;
}

// Lấy giá trị độ ẩm được đo
float DHT22_getHumidity(DHT22 *dht) {
    DHT22_readSensor(dht);
    if (dht->error == ERROR_TIMEOUT)
        DHT22_readSensor(dht);
    return dht->humidity;
}

