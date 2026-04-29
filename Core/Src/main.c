/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../users/delay.h"
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "../users/FDC2214.h"
#include "../users/sys.h"
#include "../users/myiic.h"
#include "tjcHMI.h"
#include <string.h>

double cap_val;
double cap_val2;
double difference;
double Calculate_Cable_Length_cm(double capacitance_pf);


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

extern UART_HandleTypeDef huart1;//tjc串口屏
//printf
// int __io_putchar(int ch) {
//   HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
//   return ch;
// }
//scanf
// int __io_getchar(void) {
//   uint8_t ch=0;
//   HAL_UART_Receive(&huart1,&ch,1,HAL_MAX_DELAY);
//   return ch;
// }


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */


  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  FDC2214_Init();
  iic_init();
  setbuf(stdout, NULL);
  tjcHMI_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (tjc_numFlag == 1) {
      tjc_numFlag = 0;//标志为

      //???????
      if (tjc_rxNum == 1) {
        tjc_rxNum = 0;
        tjcSetText("t8","...");
        tjcSetText("t9","0");
        tjcSetText("t6","??????");
        tjcSetText("t7","0");
        //??????
        HAL_I2C_DeInit(&hi2c1);
        HAL_Delay(10);
        MX_I2C1_Init();

        uint8_t init_status = FDC2214_Init();
        HAL_Delay(50);
        if (init_status != 0) {
          tjcSetText("t6","????");
        }
        else {
          cap_val = FDC2214_Measure_High_Precision(1, 3500);
          HAL_Delay(200);
        }


        tjcSetText("t6","???????");
        double Length = Calculate_Cable_Length_cm(cap_val);
        printf("t7.txt=\"%.1fcm\"\xff\xff\xff",Length+0.04);//?????????????С??????

      }

      //??????
      else if (tjc_rxNum == 2) {
        tjc_rxNum = 0;
        HAL_I2C_DeInit(&hi2c1);
        HAL_Delay(10);
        MX_I2C1_Init();
        tjcSetText("t6","??");

        uint8_t init_status = FDC2214_Init();
        HAL_Delay(50);
        if (init_status != 0) {
          tjcSetText("t6","????");
        }
        else {
          cap_val2 = FDC2214_Measure_High_Precision(1, 3500);
          difference=cap_val2-cap_val;
          HAL_Delay(200);
        }
        if (difference<5&&difference>-5) {
          tjcSetText("t8","???");
        }
        else if (difference>=5) {
          tjcSetText("t8","????");
          printf("t9.txt=\"%.1fpf\"\xff\xff\xff",difference);
        }
        else if (difference<-5) {
          tjcSetText("t8","????");
          
        }
        tjcSetText("t6","???????");


      }
    // HAL_I2C_DeInit(&hi2c1);
    // HAL_Delay(10);
    // MX_I2C1_Init();
    //
    // uint8_t init_status = FDC2214_Init();
    // HAL_Delay(50);
    // if (init_status != 0) {
    //   tjcSetText("t6","???????");
    // }
    // else {
    //   cap_val = FDC2214_Measure_High_Precision(1, 500);
    //   //cap_val = FDC2214_100pf(0);
    //   HAL_Delay(200);
    //   tjcSetText("t6","???????");
    //   printf("t7.txt=\"%.1fpf\"\xff\xff\xff",cap_val);





    }


      

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART1) {
    extern uint8_t tjc_rx_temp;
    tjc_FeedRxByte(tjc_rx_temp);
  }
}

/**
 * @brief  ???????????????????????
 * @param  capacitance_pf: ?????????????????????? double??????? pF
 * @retval ????????????????????????? double??????? cm
 */
double Calculate_Cable_Length_cm(double capacitance_pf)
{
  double length_cm;

  // ???????????????????????????????? 58.4pF?????????? 0.0 cm
  if (capacitance_pf < 58.4)
  {
    return 0.0;
  }

  // ????????????????????????????????? pow() ????
  // ???: L = (((c4 * C + c3) * C + c2) * C + c1) * C + c0
  // ????????????????????????????? 100 ????????
  length_cm = (2.70761e-11 * capacitance_pf - 2.47248e-7) * capacitance_pf + 3.85890e-4;
  length_cm = (length_cm * capacitance_pf + 1.07228) * capacitance_pf - 63.8886;

  // ????????????????????????????????? 0.0
  if (length_cm < 0.0)
  {
    length_cm = 0.0;
  }

  return length_cm;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
