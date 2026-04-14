/**
  ******************************************************************************
  * @file    tjcHMI.c
  * @brief   陶晶驰串口屏通用驱动库实现 (官方协议版)
  * @note    包含 printf 重定向、官方十六进制协议解析、自动追加结束符等功能
  * 支持从屏幕直接接收字符串 (get "xxx") 和纯数字 (get n0.val)
  ******************************************************************************
  */

#include "tjcHMI.h"
#include "usart.h"   // 引入串口句柄配置
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// ==========================================
// 硬件接口配置
// ==========================================
// 串口通道宏定义(huart1)
#define TJC_HUART &huart1
extern UART_HandleTypeDef huart1;

// ==========================================
// 内部变量定义
// ==========================================
#define TJC_RX_MAX_LEN 64            // 接收缓冲区的最大长度

// --- 供外部主程序读取的全局变量 ---
char tjc_rx_str[TJC_RX_MAX_LEN]; // 提取出的字符串本体
uint8_t tjc_string_ready = 0;        // 字符串接收完成标志位 (1:完成, 0:未完成)

int32_t tjc_rxNum = 0;              // 提取出的32位有符号纯数字
uint8_t tjc_numFlag = 0;           // 数字接收完成标志位 (1:完成, 0:未完成)

// --- 驱动内部使用的底层变量 ---
uint8_t tjc_rx_buf[TJC_RX_MAX_LEN];  // 存放串口收到的原始十六进制数据的缓冲池
uint8_t tjc_rx_index = 0;            // 当前接收缓冲池的写入索引
uint8_t tjc_rx_temp;                 // 用于存放单次接收的 1 个字节数据

// ==========================================
// 1. printf 串口重定向 (全平台兼容)
// ==========================================
// 注：本库舍弃了 scanf 的重定向，改用非阻塞的中断状态机接收，防止系统死锁。

#ifdef __GNUC__
// GCC 编译器 (适用于 CLion, STM32CubeIDE)
int __io_putchar(int ch) {
    // 防死锁逻辑：若全局中断被屏蔽，将超时时间设为极短，避免卡死 SysTick 定时器
    uint32_t timeout = __get_PRIMASK() ? 1 : HAL_MAX_DELAY;
    HAL_UART_Transmit(TJC_HUART, (uint8_t *)&ch, 1, timeout);
    return ch;
}
#else
// MDK-ARM 编译器 (适用于 Keil5)
int fputc(int ch, FILE *f) {
    uint32_t timeout = __get_PRIMASK() ? 1 : HAL_MAX_DELAY;
    HAL_UART_Transmit(TJC_HUART, (uint8_t *)&ch, 1, timeout);
    return ch;
}
#endif

// ==========================================
// 2. 初始化与接收逻辑 (官方协议解析机)
// ==========================================

/**
  * @brief  初始化串口屏，开启后台接收
  * @note   需在 main 函数的 while(1) 之前调用一次
  */
void tjcHMI_Init(void) {
    tjc_string_ready = 0;
    tjc_numFlag = 0;
    tjc_rx_index = 0;
    memset(tjc_rx_str, 0, TJC_RX_MAX_LEN);

    // 启动第一次中断接收，每次接收 1 个字节放入 tjc_rx_temp
    HAL_UART_Receive_IT(TJC_HUART, &tjc_rx_temp, 1);
}

/**
  * @brief  串口接收中断数据处理函数 (核心解析器)
  * @param  rx_data 接收到的单个字节数据
  * @note   必须在 HAL_UART_RxCpltCallback 中调用
  */
void tjc_FeedRxByte(uint8_t rx_data) {
    // 1. 将收到的字节放入原始缓冲池
    tjc_rx_buf[tjc_rx_index++] = rx_data;

    // 2. 检查是否凑齐了官方的结束符: 连续三个 0xFF
    if (tjc_rx_index >= 3 &&
        tjc_rx_buf[tjc_rx_index - 1] == 0xFF &&
        tjc_rx_buf[tjc_rx_index - 2] == 0xFF &&
        tjc_rx_buf[tjc_rx_index - 3] == 0xFF)
    {
        uint8_t frame_len = tjc_rx_index;

        // 【解析字符串包】：如果包头是 0x70，说明屏幕发来的是字符串 (例如执行了 get "run")
        if (tjc_rx_buf[0] == 0x70)
        {
            uint8_t str_len = frame_len - 4; // 计算字符串实际长度 (总长 减去 1个包头 和 3个0xFF)
            memcpy(tjc_rx_str, &tjc_rx_buf[1], str_len); // 从第2个字节开始提取字符串本体
            tjc_rx_str[str_len] = '\0';  // 补充 C 语言标准的字符串结束符
            tjc_string_ready = 1;            // 立起标志位，通知主程序字符串已就绪
        }

        // 【解析数字包】：如果包头是 0x71，说明屏幕发来的是纯数字 (例如执行了 get n0.val)
        // 官方数字返回格式固定为 8 个字节：0x71 + 4字节数据 + 3个0xFF
        else if (tjc_rx_buf[0] == 0x71 && frame_len == 8)
        {
            // 将 4 个字节(小端模式, 低位在前) 通过移位拼接成一个 32 位有符号整数
            tjc_rxNum = (tjc_rx_buf[4] << 24) | (tjc_rx_buf[3] << 16) |
                         (tjc_rx_buf[2] << 8)  | tjc_rx_buf[1];
            tjc_numFlag = 1;               // 立起标志位，通知主程序数字已就绪
        }

        // 成功解析一帧后，将索引清零，准备接收下一个完整报文
        tjc_rx_index = 0;
    }
    // 3. 溢出保护：如果一直没收到连续的三个 0xFF 导致缓冲区快满了，直接清空防止内存越界
    else if (tjc_rx_index >= TJC_RX_MAX_LEN)
    {
        tjc_rx_index = 0;
    }

    // 重新开启下一次接收中断，形成闭环
    HAL_UART_Receive_IT(TJC_HUART, &tjc_rx_temp, 1);
}

// ==========================================
// 3. 发送控制逻辑 (自动追加结束符 \xFF\xFF\xFF)
// ==========================================

/**
  * @brief  向串口屏发送格式化指令
  * @param  format 格式化字符串 (与 printf 用法完全一致)
  */
void tjcPrintf(const char *format, ...) {
    char tmp[128];
    va_list argptr;

    // 解析可变参数，生成最终的字符串
    va_start(argptr, format);
    int len = vsnprintf(tmp, sizeof(tmp), format, argptr);
    va_end(argptr);

    // 格式化失败直接返回
    if (len <= 0) return;

    // 长度截断保护，防止字符串超出发送缓冲区
    if (len >= sizeof(tmp)) {
        len = sizeof(tmp) - 1;
    }

    // 陶晶驰标准结束符 (连续三个 0xFF)
    uint8_t end_cmd[3] = {0xFF, 0xFF, 0xFF};
    uint32_t timeout = __get_PRIMASK() ? 1 : HAL_MAX_DELAY;

    // 先发送指令本体，再紧接着发送结束符
    HAL_UART_Transmit(TJC_HUART, (uint8_t *)tmp, len, timeout);
    HAL_UART_Transmit(TJC_HUART, end_cmd, 3, timeout);
}

// 快捷 API：设置文本内容
void tjcSetText(const char *objName, const char *text) {
    tjcPrintf("%s.txt=\"%s\"", objName, text);
}

// 快捷 API：设置控件的数值 (常用于进度条、数字框)
void tjcSetValue(const char *objName, int32_t val) {
    tjcPrintf("%s.val=%d", objName, val);
}

// 快捷 API：切换当前显示的页面
void tjcChangePage(const char *pageName) {
    tjcPrintf("page %s", pageName);
}

// 快捷 API：触发屏幕上某个控件的点击事件
void tjcClick(const char *objName, uint8_t eventType) {
    tjcPrintf("click %s,%d", objName, eventType);
}