/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2020 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */
#include "math.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include <string.h>
#define PI 3.141592
#define RX_BUFFER_SIZE 25
#define RX_BUFFER_SIZE1 1
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim9;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart6_rx;
DMA_HandleTypeDef hdma_usart6_tx;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

int cnt;
int a = 35;
int Pin_State;
int b,c,d,e = 0;

double euler[3];
int ieuler[3];

int cnt2;
int com1, com2, com3, com4, fin;
uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_buffer6[RX_BUFFER_SIZE];
uint8_t tx_buffer[];
uint8_t tx_len;
double pitch, roll, yaw;
int ipitch, iroll, iyaw, icheck;
char A_roll[7], A_pitch[7], A_yaw[7], check[7];
double radian = PI/180;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM9_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_USART1_UART_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/


void Angle()
{
  HAL_UART_Receive_DMA(&huart1, rx_buffer, RX_BUFFER_SIZE);
  if(rx_buffer[0] != '*')
  {
    for(int i = 0; i <= (RX_BUFFER_SIZE - 1); i++)
      rx_buffer[i] = 0;
  }
  for(int i = 0; i <= (RX_BUFFER_SIZE - 1); i++)
  {
    if(rx_buffer[i] == '*'){cnt2++;}
    if((rx_buffer[i] == ',')&&(com1==0)) com1 = i;
    if((rx_buffer[i] == ',')&&(com1!= i)&&(com2==0)) com2 = i;
    if((rx_buffer[i] == '\n')&&(com1!= i)&&(com2!= i)&&(com3==0)) com3 = i;
    //if(rx_buffer[i]=='\n'){cnt2++; fin = i;} 
  }
  for(int i = 1; i < com1 ; i++)
    A_roll[i-1] = rx_buffer[i];
  roll = atof(A_roll);
  iroll = atoi(A_roll);
  for(int i = (com1+1); i < com2; i++)
    A_pitch[i - (com1 + 1)] = rx_buffer[i];
  pitch = atof(A_pitch);
  ipitch = atoi(A_pitch);
  for(int i = (com2+1); i < com3; i++)
    A_yaw[i - (com2 + 1)] = rx_buffer[i];
  yaw = atof(A_yaw);
  iyaw = atoi(A_yaw);
  cnt2 = 0; com1 = 0; com2 = 0; com3 = 0; fin = 0;
  
}

unsigned short update_crc(unsigned short crc_accum, unsigned char *data_blk_ptr, unsigned short data_blk_size)
{
  unsigned short i, j;
  unsigned short crc_table[256] = {
    0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
    0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
    0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
    0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
    0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
    0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
    0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
    0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
    0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
    0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
    0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
    0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
    0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
    0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
    0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
    0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
    0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
    0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
    0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
    0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
    0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
    0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
    0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
    0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
    0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
    0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
    0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
    0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
    0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
    0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
    0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
    0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
  };
  
  for(j = 0; j < data_blk_size; j++)
  {
    i = ((unsigned short)(crc_accum >> 8) ^ data_blk_ptr[j]) & 0xFF;
    crc_accum = (crc_accum << 8) ^ crc_table[i];
  }
  
  return crc_accum;
}

void TorqueEnable(unsigned char ID , unsigned char OnOff)
{
  unsigned char Header1 = 0xff;
  unsigned char Header2 = 0xff;
  unsigned char Header3 = 0xfd;
  unsigned char Reserved = 0x00;
  
  //	unsigned char ID = 0x02;					
  unsigned char PacketLengthL = 0x09;			//총 길이: 9 /(parameter 6개  + 3개(기타) )
  unsigned char PacketLengthH = 0x00;
  unsigned char Instruction = 0x03;			//Write mode
  
  
  unsigned char Parameter_1= 0x40 ;	 		//0x40 은 torque enable
  unsigned char Parameter_2= 0x00 ;			// Data를 쓰고자 하는 곳의 시작 Address(상위바이트)
  
  
  unsigned char Parameter3 = OnOff;		        //on 은 1 , off 는 0
  unsigned char Parameter4 = 0x00;
  unsigned char Parameter5 = 0x00;				
  unsigned char Parameter6 = 0x00;			
  
  //crc 를 구하기 위한 수식
  unsigned short Packet_Length = (PacketLengthH << 8) + PacketLengthL;	
  unsigned short DataBlkSize = 5 + Packet_Length ;						
  unsigned char CRC_L = 0;
  unsigned char CRC_H = 0;
  unsigned short CRC_A = 0;
  
  
  unsigned char TxPacket[] = { Header1, Header2, Header3, Reserved, ID, PacketLengthL, PacketLengthH, Instruction, Parameter_1,
  Parameter_2, Parameter3, Parameter4, Parameter5, Parameter6, CRC_L, CRC_H };
  
  CRC_A = update_crc(0, TxPacket , DataBlkSize);   // 12 = 5 + Packet Length(7)
  CRC_L = (CRC_A & 0x00FF);               //Little-endian
  CRC_H = (CRC_A>>8) & 0x00FF;
  tx_len = 0;
  tx_len = sizeof(TxPacket);
  TxPacket[tx_len-1] = CRC_H;
  TxPacket[tx_len-2] = CRC_L;
  
  HAL_UART_Transmit(&huart6, (uint8_t *)TxPacket, tx_len, HAL_MAX_DELAY);  
}

void SyncWrite(unsigned int GoalPos1 , unsigned int GoalPos2 , unsigned int GoalPos3)
{
  unsigned char Header1 = 0xff;
  unsigned char Header2 = 0xff;
  unsigned char Header3 = 0xfd;
  unsigned char Reserved = 0x00;
  
  unsigned char ID = 0xFE;
  unsigned char PacketLengthL = 0x16;			//총 길이: 22 /(parameter 19개  + 3개(기타) )
  unsigned char PacketLengthH = 0x00;
  unsigned char Instruction = 0x83;			//Sync Write mode
  
  
  unsigned char Parameter_1 = 0x74;	 		//Data Address (0x74 : Goal Position)
  unsigned char Parameter_2 = 0x00;			//Address(상위바이트)
  
  unsigned char Parameter3 = 0x04;			//Data Length
  unsigned char Parameter4 = 0x00;
  
  unsigned char Parameter5 = 0x03;			//1st ID yaw
  unsigned char Parameter6 = (GoalPos1 & 0xff);
  unsigned char Parameter7 = ((GoalPos1>>8) & 0xff);
  unsigned char Parameter8 = ((GoalPos1>>16) & 0xff);
  unsigned char Parameter9 = ((GoalPos1>>24) & 0xff);
  
  unsigned char Parameter10 = 0x02;			//2nd ID pitch
  unsigned char Parameter11 = (GoalPos2 & 0xff);
  unsigned char Parameter12 = ((GoalPos2>>8) & 0xff);
  unsigned char Parameter13 = ((GoalPos2>>16) & 0xff);
  unsigned char Parameter14 = ((GoalPos2>>24) & 0xff);
  
  unsigned char Parameter15 = 0x01;			//3rd ID roll
  unsigned char Parameter16 = (GoalPos3 & 0xff);
  unsigned char Parameter17 = ((GoalPos3>>8) & 0xff);
  unsigned char Parameter18 = ((GoalPos3>>16) & 0xff);
  unsigned char Parameter19 = ((GoalPos3>>24) & 0xff);
  
  //crc 를 구하기 위한 수식
  unsigned short Packet_Length = (PacketLengthH << 8) + PacketLengthL;
  unsigned short DataBlkSize = 5 + Packet_Length;
  unsigned char CRC_L = 0;
  unsigned char CRC_H = 0;
  unsigned short CRC_A = 0;
  
  unsigned char TxPacket[] = { Header1, Header2, Header3, Reserved, ID, PacketLengthL, PacketLengthH, Instruction, Parameter_1,
  Parameter_2, Parameter3, Parameter4, Parameter5, Parameter6, Parameter7, Parameter8, Parameter9, Parameter10, Parameter11,
  Parameter12, Parameter13, Parameter14, Parameter15, Parameter16, Parameter17, Parameter18, Parameter19, CRC_L, CRC_H };
  
  CRC_A = update_crc(0, TxPacket, DataBlkSize);   // 12 = 5 + Packet Length(7)
  CRC_L = (CRC_A & 0x00FF);               //Little-endian
  CRC_H = (CRC_A >> 8) & 0x00FF;
  tx_len = 0;
  tx_len = sizeof(TxPacket);
  TxPacket[tx_len-1] = CRC_H;
  TxPacket[tx_len-2] = CRC_L;
  
  HAL_UART_Transmit(&huart6, (uint8_t *)TxPacket, tx_len, HAL_MAX_DELAY);     
}

unsigned int AngleXL(signed int A)
{
  double B = 2048 - (double)A*(1024/90);
  
  return (unsigned int)B;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &htim9)
  {
    cnt++;
    HAL_UART_Receive_DMA(&huart6, rx_buffer6, RX_BUFFER_SIZE1);
    if(cnt == 30)
    {
      if(rx_buffer6[0] == 'd')
        b = 1;
      if((rx_buffer6[0] == 'c')&&(b == 1))
        d = 1;
      if((b == 1)&&(c < 20))
      {
        a = a-3;
        SyncWrite(AngleXL(a),AngleXL(0),AngleXL(0));
        c++;
      }
      if((b == 1)&&(c >= 20)&&(c < 35))
      {
        a = a+3;
        SyncWrite(AngleXL(a),AngleXL(0),AngleXL(0));
        c++;
      }
      if((c == 35)&&(e==0))
      {
        HAL_UART_Transmit(&huart1, "*", 1, HAL_MAX_DELAY);
        Angle();
        SyncWrite(AngleXL(iroll),AngleXL(-ipitch),AngleXL(0));
        c = 35;
      }
      if((d == 1)&&(e < 25))
      {
        a = a+3;
        if(a >= 35)
          a = 35;
        SyncWrite(AngleXL(a),AngleXL(0),AngleXL(0));
        e++;
      }
      
      cnt = 0;
    }
  }
}

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM9_Init();
  MX_USART6_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim9);
 
  
  TorqueEnable(0x01, 0x01);
  HAL_Delay(5);
  TorqueEnable(0x02, 0x01);
  HAL_Delay(5);
  TorqueEnable(0x03, 0x01);
  HAL_Delay(5);
  SyncWrite(AngleXL(a),AngleXL(0),AngleXL(0));
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    
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

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* TIM9 init function */
static void MX_TIM9_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;

  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 839;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 100-1;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART6 init function */
static void MX_USART6_UART_Init(void)
{

  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
  /* DMA2_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
     PA15   ------> S_TIM2_CH1_ETR
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB13 PB14 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
