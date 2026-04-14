#include "FDC2214.h"
#include <math.h>
#include "main.h"
extern I2C_HandleTypeDef hi2c1;

uint8_t FDC2214_Init(void)
{
    uint8_t i = 0;
    uint8_t cof[2];
    volatile uint16_t check[2];

    // ========================================================
    // 【修改核心】：第一步！上来先执行真正的硬件强制复位
    // ========================================================
    cof[0] = 0x80; // 最高位写 1，触发硬件复位
    cof[1] = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, RESET_DEV, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(50); // 极其重要：复位后必须等它起振重启，50ms足够了

    // ========================================================
    // 第二步：芯片满血复活后，开始写入各项配置
    // ========================================================
    cof[0] = 0xFF;
    cof[1] = 0xFF;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, RCOUNT_CH0, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0xFF;
    cof[1] = 0xFF;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, RCOUNT_CH1, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0xFF;
    cof[1] = 0xFF;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, RCOUNT_CH2, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0xFF;
    cof[1] = 0xFF;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, RCOUNT_CH3, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0x00;
    cof[1] = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, OFFSET_CH0, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0x00;
    cof[1] = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, OFFSET_CH1, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0x00;
    cof[1] = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, OFFSET_CH2, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0x00;
    cof[1] = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, OFFSET_CH3, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0x04;
    cof[1] = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, SETTLECOUNT_CH0, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0x04;
    cof[1] = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, SETTLECOUNT_CH1, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0x04;
    cof[1] = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, SETTLECOUNT_CH2, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0x04;
    cof[1] = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, SETTLECOUNT_CH3, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0x10;
    cof[1] = 0x01;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, CLOCK_DIVIDERS_C_CH0, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0x10;
    cof[1] = 0x01;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, CLOCK_DIVIDERS_C_CH1, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0x10;
    cof[1] = 0x01;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, CLOCK_DIVIDERS_C_CH2, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0x10;
    cof[1] = 0x01;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, CLOCK_DIVIDERS_C_CH3, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0x00;
    cof[1] = 0x01;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, ERROR_CONFIG, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);

    // 注意：这里的 CONFIG 配置非常关键，它决定了芯片是否开始持续扫描
    cof[0] = 0x1e;
    cof[1] = 0x01;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, CONFIG, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0xC2;
    cof[1] = 0x0C;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, MUX_CONFIG, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);

    // （删除了这里原本错误的假复位代码）

    cof[0] = 0x8c;
    cof[1] = 0x40;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, DRIVE_CURRENT_CH0, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0x8c;
    cof[1] = 0x40;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, DRIVE_CURRENT_CH1, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0x88;
    cof[1] = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, DRIVE_CURRENT_CH2, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    cof[0] = 0x88;
    cof[1] = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, FDC2214, DRIVE_CURRENT_CH3, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);

    // ========================================================
    // 第三步：验证自检
    // ========================================================
    HAL_I2C_Mem_Read(&hi2c1, FDC2214, MANUFACTURER_ID, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    check[0] = cof[1] | cof[0] << 8;

    HAL_I2C_Mem_Read(&hi2c1, FDC2214, DEVICE_ID, I2C_MEMADD_SIZE_8BIT, cof, 2, 100);
    HAL_Delay(1);
    check[1] = cof[1] | cof[0] << 8;

    if ((check[0] == MANUFACTURER_ID_val) && (check[1] == DEVICE_ID_val))
       return 0; // 成功
    else
       return 1; // 失败
}

uint32_t FDC2214_GetCapacitanceData(uint8_t channel)
{
	volatile uint32_t data;
	uint8_t i = 0;
	uint8_t pdata[4];
	switch (channel)
	{
	case 0:
		HAL_I2C_Mem_Read(&hi2c1, FDC2214, DATA_MSB_CH0, I2C_MEMADD_SIZE_8BIT, pdata, 2, 100);
		HAL_Delay(1);
		HAL_I2C_Mem_Read(&hi2c1, FDC2214, DATA_LSB_CH0, I2C_MEMADD_SIZE_8BIT, pdata + 2, 2, 100);
		HAL_Delay(1);
		data = ((pdata[0] << 24 | pdata[1] << 16 | pdata[2] << 8 | pdata[3]) & 0x0fffffff);
		return data;
	case 1:
		HAL_I2C_Mem_Read(&hi2c1, FDC2214, DATA_MSB_CH1, I2C_MEMADD_SIZE_8BIT, pdata, 2, 100);
		HAL_Delay(1);
		HAL_I2C_Mem_Read(&hi2c1, FDC2214, DATA_LSB_CH1, I2C_MEMADD_SIZE_8BIT, pdata + 2, 2, 100);
		HAL_Delay(1);
		data = ((pdata[0] << 24 | pdata[1] << 16 | pdata[2] << 8 | pdata[3]) & 0x0fffffff);
		return data;
	case 2:
		HAL_I2C_Mem_Read(&hi2c1, FDC2214, DATA_MSB_CH2, I2C_MEMADD_SIZE_8BIT, pdata, 2, 100);
		HAL_Delay(1);
		HAL_I2C_Mem_Read(&hi2c1, FDC2214, DATA_LSB_CH2, I2C_MEMADD_SIZE_8BIT, pdata + 2, 2, 100);
		HAL_Delay(1);
		data = ((pdata[0] << 24 | pdata[1] << 16 | pdata[2] << 8 | pdata[3]) & 0x0fffffff);
		return data;
	case 3:
		HAL_I2C_Mem_Read(&hi2c1, FDC2214, DATA_MSB_CH3, I2C_MEMADD_SIZE_8BIT, pdata, 2, 100);
		HAL_Delay(1);
		HAL_I2C_Mem_Read(&hi2c1, FDC2214, DATA_LSB_CH3, I2C_MEMADD_SIZE_8BIT, pdata + 2, 2, 100);
		HAL_Delay(1);
		data = ((pdata[0] << 24 | pdata[1] << 16 | pdata[2] << 8 | pdata[3]) & 0x0fffffff);
		return data;
	default:
		return 0;
	}
}

double FDC2214_100pf(uint8_t channel) {
	uint32_t avg_raw_data = FDC2214_GetCapacitanceData(channel);
    // double f_sensor = ((double)raw_data * F_REF) / pow(2, 28);
    // double c_total = 1.0 / (L_SENSOR * pow(2 * PI * f_sensor, 2));
    // double c_pf = (c_total - C_BOARD) * 1e14-1750;
    // return c_pf;
	// 计算当前的传感器振荡频率
	double f_sensor = ((double)avg_raw_data * F_REF) / pow(2, 28);

	// 根据 LC 谐振公式计算真实物理总电容 (单位：法拉 F)
	double c_total_F = 1.0 / (L_SENSOR * pow(2 * PI * f_sensor, 2));

	// 换算为 pF 直接返回 (1F = 10^12 pF)
	return c_total_F * 1e12;

}
/**
 * @brief  高精度测量电容值 (带均值滤波与超时控制)
 * @param  channel: 测量通道 (0~3)
 * @param  sample_time_ms: 持续采样的时间窗口 (毫秒)
 * @retval double: 计算出的高精度电容值 (pF)
 */
double FDC2214_Measure_High_Precision(uint8_t channel, uint32_t sample_time_ms) {
	uint32_t start_time = HAL_GetTick();
	uint64_t sum_raw_data = 0; // 64位累加器，防止几百次读取后溢出
	uint32_t valid_sample_count = 0;

	// 1. 在规定的时间窗口内疯狂读取底层寄存器
	while ((HAL_GetTick() - start_time) < sample_time_ms) {
		uint32_t raw_data = FDC2214_GetCapacitanceData(channel);

		// 剔除可能的通信错误 (0值)
		if (raw_data > 0 && raw_data < 0x0FFFFFFF) {
			sum_raw_data += raw_data;
			valid_sample_count++;
		}

		// 短暂延时，避免过度占用 I2C 总线并匹配芯片转换速率
		HAL_Delay(5);
	}

	// 2. 如果没有采集到有效数据，返回 0
	if (valid_sample_count == 0) {
		return 0.0;
	}

	// 3. 计算原始数据的平均值
	uint32_t avg_raw_data = (uint32_t)(sum_raw_data / valid_sample_count);

	// 计算当前的传感器振荡频率
    double f_sensor = ((double)avg_raw_data * F_REF) / pow(2, 28);

    // 根据 LC 谐振公式计算真实物理总电容 (单位：法拉 F)
    double c_total_F = 1.0 / (L_SENSOR * pow(2 * PI * f_sensor, 2));

    // 换算为 pF 直接返回 (1F = 10^12 pF)
    return c_total_F * 1e12;

}
