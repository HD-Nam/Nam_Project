#include "mainApp.h"
#include "stdlib.h"

extern ADC_HandleTypeDef hadc1;

DHT22 dht;
MQ135 mq135;
PM25 pm25;


data_sensor value;

void Fullpower_session(void)
{
//	dht.temperature = 33.5;
//	dht.humidity = 69.3;

	DHT22_init(&dht, GPIOA, GPIO_PIN_0);
	MQ135_init(&mq135, &hadc1, ADC_CHANNEL_1);
	PM25_init(&pm25, GPIOA, GPIO_PIN_2, GPIOA, GPIO_PIN_6);

	while(1)
	{
		if(conn == NULL || conn->state == CLOSE)
		{
			conn = Lcp_Connect(_lcp_cl, (point){0x21});
			HAL_Delay(500);
		}

		flag_read_SS_done = 0;
		DHT22_getTemperature(&dht);
		DHT22_getHumidity(&dht);
		MQ135_getCorrectedCO2PPM(&mq135, dht.temperature, dht.humidity);
		MQ135_getCOPPM(&mq135);
		PM25_getDustDensity(&pm25);

//		mq135.CO = (float)(rand() % 100) / 100.0;
//		mq135.Co2 = 150.0 + (rand() % (2500 + 1 - 1500)) / 10.0;
//		pm25.dustDensity = (float)(rand() % 50) / 100.0;

		value.paramBCD.temp1 = (uint8_t)dht.temperature;
		value.paramBCD.temp2 = (uint8_t)(dht.temperature * 100.0) % 100;
		value.paramBCD.Humi1 = (uint8_t)dht.humidity;
		value.paramBCD.Humi2 = (uint8_t)(dht.humidity * 100.0) % 100;
		value.paramBCD.C02_1 = (uint8_t)mq135.Co2;
		value.paramBCD.C02_2 = (uint8_t)(mq135.Co2 * 100.0) % 100;
		value.paramBCD.Pm25_1 = (uint8_t)pm25.dustDensity;
		value.paramBCD.Pm25_2 = (uint8_t)(pm25.dustDensity * 100.0) % 100;
		value.paramBCD.CO_1 = (uint8_t)mq135.CO;
		value.paramBCD.CO_2 = (uint8_t)(mq135.CO * 100.0) % 100;

//		value.paramBCD.temp1 = (uint8_t)33.5;
//		value.paramBCD.temp2 = (uint8_t)(33.5 * 100.0) % 100;
//		value.paramBCD.Humi1 = (uint8_t)66.4;
//		value.paramBCD.Humi2 = (uint8_t)(66.4 * 100.0) % 100;
//		value.paramBCD.C02_1 = (uint8_t)1.6;
//		value.paramBCD.C02_2 = (uint8_t)(1.6 * 100.0) % 100;
//		value.paramBCD.Pm25_1 = (uint8_t)2.7;
//		value.paramBCD.Pm25_2 = (uint8_t)(2.7 * 100.0) % 100;
//		value.paramBCD.CO_1 = (uint8_t)2.3;
//		value.paramBCD.CO_2 = (uint8_t)(2.3 * 100.0) % 100;

		flag_read_SS_done = 1;
//		HAL_Delay(2000);
	}
}
