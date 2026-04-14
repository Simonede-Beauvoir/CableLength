#include "sys.h" // 确保这里包含了你的 sys.h 或 stm32f4xx_hal.h

// 进入待机模式
void WFI_SET(void)
{
	__WFI();
}

// 关闭所有中断(但是不包括fault和NMI中断)
void INTX_DISABLE(void)
{
	__disable_irq(); // CMSIS 标准接口，等同于 CPSID I
}

// 开启所有中断
void INTX_ENABLE(void)
{
	__enable_irq();  // CMSIS 标准接口，等同于 CPSIE I
}

// 设置栈顶地址
// addr:栈顶地址
void MSR_MSP(uint32_t addr) // 建议把原先的 u32 改为标准的 uint32_t
{
	__set_MSP(addr); // CMSIS 标准接口，直接设置主堆栈指针
}














