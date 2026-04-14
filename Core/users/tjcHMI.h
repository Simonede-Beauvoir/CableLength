/**
******************************************************************************
  * @file    tjcHMI.h
  * @brief   陶晶驰串口屏通用驱动库 (基于非阻塞字符串指令通信)
  * @note    文件编码：GB2312 / GBK
  ******************************************************************************
  */

#ifndef __TJC_HMI_H
#define __TJC_HMI_H

#include "main.h"  // 包含底层的 HAL 库定义

// ==========================================
// 宏定义与参数配置
// ==========================================
// 定义接收缓冲区的最大长度 (可根据需要发送的指令长度自行修改)
#define TJC_RX_MAX_LEN 64

// ==========================================
// 全局变量声明 (供外部主程序调用)
// ==========================================
extern char tjc_rx_str[64];  // 存放屏幕发来的字符串
extern uint8_t tjc_strFlag; // 字符串接收完成标志位(接收完成：1)
extern int32_t tjc_rxNum;       // 存放屏幕发来的纯数字
extern uint8_t tjc_numFlag;    // 数字接收完成标志位
// ==========================================
// 函数声明
// ==========================================

// --- 初始化与底层接收 ---
// 初始化串口屏接收 (在 main 函数 while(1) 之前调用)
void tjcHMI_Init(void);

// 串口接收处理核心 (必须放在 HAL_UART_RxCpltCallback 中调用)
void tjc_FeedRxByte(uint8_t rx_data);

// --- 基础输出与控制 API ---
// 基础的格式化输出函数，自动追加 \xFF\xFF\xFF (支持类似 printf 的变参)
void tjcPrintf(const char *format, ...);

// 设置文本控件的内容 (例: tjcSetText("t0", "Hello");)
void tjcSetText(const char *objName, const char *text);

// 设置数字/进度条控件的值 (例: tjcSetValue("j0", 50);)
void tjcSetValue(const char *objName, int32_t val);

// 切换页面 (例: tjcChangePage("main");)
void tjcChangePage(const char *pageName);

// 触发屏幕控件的点击事件 (例: tjcClick("b0", 1); 1为按下，0为弹起)
void tjcClick(const char *objName, uint8_t eventType);

#endif // __TJC_HMI_H


//2026.3.23