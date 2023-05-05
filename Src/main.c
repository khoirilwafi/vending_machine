#include "main.h"

ADC_HandleTypeDef hadc1;

unsigned char     nama_slot[6] = {0,0,0,0,0,0};		//         	<
unsigned char namatemp_slot[6] = {0,0,0,0,0,0};	  //           |
unsigned char    harga_slot[6] = {0,0,0,0,0,0};   //           |  data temp
unsigned char    aktif_slot[6] = {1,0,0,0,0,0};   //           |
unsigned char  minuman_slot[6] = {0,0,0,0,0,0};   //          <
unsigned char flag_push  = 0;
unsigned char flag_show  = 0;

int adcvalue;
int phase = 0;
int coin = 0;
int data_harga = 2;
int data_minuman = 0;
int data_slot = 0;

unsigned int preset_tombol;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);


void h595shift(unsigned char data){     
	for(unsigned char i = 0; i < 8; i++){
		if(data & 0x80){
			HAL_GPIO_WritePin(DATA_GPIO_Port,DATA_Pin,GPIO_PIN_SET);
		}
		else{
			HAL_GPIO_WritePin(DATA_GPIO_Port,DATA_Pin,GPIO_PIN_RESET);
		}
		HAL_GPIO_WritePin(CLOCK_GPIO_Port,CLOCK_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(CLOCK_GPIO_Port,CLOCK_Pin,GPIO_PIN_RESET);
		data = data << 1;
	}
	HAL_GPIO_WritePin(LOAD_GPIO_Port,LOAD_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(LOAD_GPIO_Port,LOAD_Pin,GPIO_PIN_RESET);
}


void lcd_en(){
	HAL_GPIO_WritePin(EN_GPIO_Port,EN_Pin,GPIO_PIN_SET);
	for(unsigned int i = 0; i <= 13000; i++){}
	HAL_GPIO_WritePin(EN_GPIO_Port,EN_Pin,GPIO_PIN_RESET);
	for(unsigned int i = 0; i <= 13000; i++){}
}

void lcd_command(unsigned char cmd){
	HAL_GPIO_WritePin(RW_GPIO_Port,RW_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RS_GPIO_Port,RS_Pin,GPIO_PIN_RESET);
	GPIOB->ODR &= 0x0f;
	GPIOB->ODR |= (cmd & 0xf0);
	lcd_en();
	GPIOB->ODR &= 0x0f;
	GPIOB->ODR |= (cmd << 4);
	lcd_en();
}

void lcd_char(unsigned char datachar){
	HAL_GPIO_WritePin(RW_GPIO_Port,RW_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(RS_GPIO_Port,RS_Pin,GPIO_PIN_SET);
	GPIOB->ODR &= 0x0f;
	GPIOB->ODR |= (datachar & 0xf0);
	lcd_en();
	GPIOB->ODR &= 0x0f;
	GPIOB->ODR |= (datachar << 4);
	lcd_en();
}


void lcd_init(){
	lcd_command(0x02);
	lcd_command(0x28);
	lcd_command(0x0C);
	lcd_command(0x06);
	lcd_command(0x80);
}



void lcd_string(char *str){
	int i;
	for(i = 0; str[i] != 0; i++){
		lcd_char(str[i]);
	}
}


void lcd_xy(char x , char y){
	     if(x == 0 && y < 20) lcd_command((y & 0x0f) | 0x80);
	else if(x == 1 && y < 20) lcd_command((y & 0x0f) | 0xC0);
	else if(x == 2 && y < 20) lcd_command((y & 0x0f) | 0x94);
	else if(x == 3 && y < 20) lcd_command((y & 0x0f) | 0xD4);
}

void lcd_clear(){
	lcd_command(0x01);
	lcd_command(0x80);
}

void lcd_int(unsigned int dataint){
	char st[5] = "";
	sprintf(st,"%d",dataint);
	lcd_string(st);
}
void cektombol(){
	
	HAL_ADC_PollForConversion(&hadc1 , 100);
	adcvalue = HAL_ADC_GetValue(&hadc1);
	
			 if((adcvalue < 2000) & (adcvalue > 1800)){ preset_tombol = 1;} // up
	else if((adcvalue < 3100) & (adcvalue > 2900)){ preset_tombol = 2;} // down
	else if((adcvalue < 1000) & (adcvalue > 800 )){ preset_tombol = 3;} // right
	else if((adcvalue < 50  ) & (adcvalue >= 0  )){ preset_tombol = 4;} // left
	else if((adcvalue < 3700) & (adcvalue > 3400)){ preset_tombol = 5;} // ok 
	else{preset_tombol = 0;}

}

void ubah_minuman(unsigned int data_minuman){
	lcd_xy(2,9);
	switch(data_minuman){
		case 0 : lcd_string("COCA COLA   "); break;
		case 1 : lcd_string("TEH BOTOL   "); break;
		case 2 : lcd_string("FANTA       "); break;
		case 3 : lcd_string("SPRITE      "); break;
		case 4 : lcd_string("COFFIE      "); break;
		case 5 : lcd_string("COCOLATE    "); break;
	}
}

void ubah_harga(unsigned int data_harga){
	lcd_xy(3,9);
	switch(data_harga){
		case 1 : lcd_string("RP.500      "); break;
		case 2 : lcd_string("RP.1,000    "); break;
		case 3 : lcd_string("RP.1,500    "); break;
		case 4 : lcd_string("RP.2,000    "); break;
		case 5 : lcd_string("RP.2,500    "); break;
		case 6 : lcd_string("RP.3,000    "); break;
		case 7 : lcd_string("RP.3,500    "); break;
		case 8 : lcd_string("RP.4,000    "); break;
		case 10 : lcd_string("RP.5,000   "); break;
		case 11 : lcd_string("RP.5,500   "); break;
		case 12 : lcd_string("RP.6,000   "); break;
		case 13 : lcd_string("RP.6,500   "); break;
		case 14 : lcd_string("RP.7,000   "); break;
		case 15 : lcd_string("RP.7,500   "); break;
		case 16 : lcd_string("RP.8,000   "); break;
		case 17 : lcd_string("RP.8,500   "); break;
		case 18 : lcd_string("RP.9,000   "); break;
		case 19 : lcd_string("RP.9,500   "); break;
		case 20 : lcd_string("RP.10,000  "); break;

	}
}

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_ADC1_Init();
	
  lcd_init();
  lcd_clear();
  HAL_ADC_Start(&hadc1);
	
  HAL_GPIO_WritePin(CLEAR_GPIO_Port,CLEAR_Pin,GPIO_PIN_SET);
	
  lcd_xy(0,0); lcd_string("   SELAMAT DATANG   ");
  lcd_xy(1,0); lcd_string("Vending Machine 7640");
  lcd_xy(2,0); lcd_string(" LKS SMK Tahun 2019 ");
  lcd_xy(3,0); lcd_string("   NASIONAL JOGJA   ");
  HAL_Delay(5000);
  lcd_clear();

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		HAL_GPIO_WritePin(CLEAR_GPIO_Port,CLEAR_Pin,GPIO_PIN_SET);
		for(unsigned char i = 0; i < 8; i++){
		HAL_GPIO_WritePin(DATA_GPIO_Port,DATA_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(CLOCK_GPIO_Port,CLOCK_Pin,GPIO_PIN_SET);
		HAL_Delay(1);
		HAL_GPIO_WritePin(CLOCK_GPIO_Port,CLOCK_Pin,GPIO_PIN_RESET);
		HAL_Delay(1);
	}
	HAL_GPIO_WritePin(LOAD_GPIO_Port,LOAD_Pin,GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(LOAD_GPIO_Port,LOAD_Pin,GPIO_PIN_RESET);
	HAL_Delay(1);

	switch(phase){
  case 0 :
    lcd_xy(0,0); lcd_string("   SELAMAT DATANG   ");
    lcd_xy(1,0); lcd_string("Vending Machine 7640");
    lcd_xy(2,0); lcd_string(" LKS SMK Tahun 2019 ");
    lcd_xy(3,0); lcd_string("   NASIONAL JOGJA   ");
		cektombol();
    if((preset_tombol == 5) && (flag_push == 0)){
      phase += 1;
			lcd_clear();
    }
  break;
  
  case 1 :
    lcd_xy(0,0);
    lcd_string("    SETTING MODE    ");
    lcd_xy(2,2); lcd_string("Nama  :");
    lcd_xy(1,2); lcd_string("Slot  :");
    lcd_xy(3,2); lcd_string("Harga :");
	
		for(unsigned char i=0;i<6;i++){
      if(flag_show == 0){
        h595shift(minuman_slot[i],aktif_slot[i]);
      }
    }
		
		flag_show = 1;
    
    ubah_minuman(data_minuman);
    lcd_xy(1,9); lcd_int(data_slot+1);
    lcd_xy(3,9); lcd_int(data_harga*500);
    
    cektombol();
    
    if((preset_tombol == 1) && (flag_push == 0)){
      if(data_harga >= 18){data_harga = 18;}
      else{ data_harga += 1;}
      flag_push = 1;
    }
    else if((preset_tombol == 2) && (flag_push == 0)){
      if(data_harga <= 2){data_harga = 2;}
      else{ data_harga -= 1;}
      flag_push = 1;
    }
    else if((preset_tombol == 3) && (flag_push == 0)){
      if(data_minuman >= 5){data_minuman = 5;}
      else{ data_minuman += 1;}
      flag_push = 1;
    }
    else if((preset_tombol == 4) && (flag_push == 0)){
      if(data_minuman <= 0){data_minuman = 0;}
      else{ data_minuman -= 1;}
      flag_push = 1;
    }
    else if((preset_tombol == 5) && (flag_push == 0)){
      if(namatemp_slot[data_minuman] == 0){			// jika nama belum terpakai
        nama_slot[data_slot] = data_minuman;		// mengisi data nama ke variabel nama_slot
        namatemp_slot[data_minuman] = 1;				// beri tanda pada data_temp nama
        harga_slot[data_slot] = data_harga;			// mengisi data harga ke variabel harga_slot
        lcd_clear();
        lcd_xy(1,0); lcd_string("    PROCCESSSING    "); HAL_Delay(500);
        lcd_xy(1,0); lcd_string("      OK  DONE      "); HAL_Delay(1000);
        lcd_clear();
        if(data_slot >= 5){ 
					phase += 1; 
					data_slot = 0; 
					for(unsigned char i=0;i<6;i++){
						aktif_slot[i] = 0;
					}
					aktif_slot[0] = 1;
				}
        else {data_slot += 1; aktif_slot[data_slot] = 1; aktif_slot[data_slot-1] = 0;}
				//                     aktifkan index slot      matikan index slot sebelumnya   
      }
      else{
        lcd_clear();
        lcd_xy(1,0); lcd_string("    PROCCESSSING    "); HAL_Delay(500);
        lcd_xy(1,0); lcd_string("       ERROR        ");
        lcd_xy(2,0); lcd_string("Nama Sudah Terpakai "); HAL_Delay(2000);
        lcd_clear();
      }
      flag_push = 1;    
			flag_show = 0;
    }
    else if(preset_tombol == 0){
      flag_push = 0;
    }
  break;

  case 2 :
    lcd_xy(0,0);
    lcd_string("    FILLING MODE    ");
    lcd_xy(2,1); lcd_string("Nama  :");
    lcd_xy(1,1); lcd_string("Slot  :");
    lcd_xy(3,1); lcd_string("Jumlah:");
    
    for(unsigned char i=0;i<6;i++){
      if(flag_show == 0){
        h595shift(minuman_slot[i],aktif_slot[i]);
      }
    }
    
    flag_show = 1;
    
    ubah_minuman(nama_slot[data_slot]);
    lcd_xy(1,9); lcd_int(data_slot+1);
    lcd_xy(3,9); lcd_int(minuman_slot[data_slot]);
    
    cektombol();
    
    if((preset_tombol == 1) && (flag_push == 0)){
        if(minuman_slot[data_slot] >= 7){ minuman_slot[data_slot] = 7; }
      else{ minuman_slot[data_slot] += 1; }
      flag_show = 0;
      flag_push = 1;
    }
    else if((preset_tombol == 2) && (flag_push == 0)){
        if(minuman_slot[data_slot] <= 0){ minuman_slot[data_slot] = 0; }
      else{ minuman_slot[data_slot] -= 1; }
      flag_show = 0;
      flag_push = 1;
    }
    else if((preset_tombol == 5) && (flag_push == 0)){
      lcd_clear();
      lcd_xy(1,0); lcd_string("    PROCCESSSING    "); HAL_Delay(500);
      lcd_xy(1,0); lcd_string("      OK  DONE      "); HAL_Delay(1000);
      lcd_clear();
      if(data_slot >= 5){ phase += 1; }
      else {data_slot += 1; aktif_slot[data_slot] = 1; aktif_slot[data_slot-1] = 0;}
      flag_show = 0;
      flag_push = 1;
    }
    else if(preset_tombol == 0){
      flag_push = 0;
    }
  break;

  case 3 :
    lcd_xy(0,0);
    lcd_string("    RUNNING MODE    ");
    lcd_xy(1,0); lcd_string("   Masukkan  Coin   ");
    lcd_xy(2,0); lcd_string("      Rp");
    lcd_xy(2,13); lcd_int(coin*500);
	
		if(flag_show == 0){
      for(unsigned char i=0;i<6;i++){
        h595shift(minuman_slot[i],aktif_slot[i]);
      }
      flag_show = 1;
    }

    cektombol();

    if((preset_tombol == 5) && (flag_push == 0)){
      if(coin >= 2){
        phase += 1;
				data_slot = 0;
				for(unsigned char i=0;i<6;i++){
					aktif_slot[i] = 0;
				}
				aktif_slot[0] = 1;
				lcd_clear();
      }
      flag_push = 1;
			flag_show = 0;
    }

    else if(HAL_GPIO_ReadPin(COIN_GPIO_Port,COIN_Pin) == 0){
      HAL_GPIO_WritePin(LED_COIN_GPIO_Port,LED_COIN_Pin,GPIO_PIN_RESET);
      if(flag_push == 0){
        coin += 1;
        flag_push = 1;
      }
    }
    else if((HAL_GPIO_ReadPin(COIN_GPIO_Port,COIN_Pin) == 1) && (preset_tombol == 0)){
      HAL_GPIO_WritePin(LED_COIN_GPIO_Port,LED_COIN_Pin,GPIO_PIN_SET);
      flag_push = 0;
    }
  break;

  case 4 :
    HAL_GPIO_WritePin(LED_COIN_GPIO_Port,LED_COIN_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_TAKE_GPIO_Port,LED_TAKE_Pin,GPIO_PIN_SET);
    
    lcd_xy(0,0);
    lcd_string("    RUNNING MODE    ");
    lcd_xy(1,1); lcd_string("Slot  :");
    lcd_xy(2,1); lcd_string("Nama  :");
    lcd_xy(3,1); lcd_string("Harga :");
      
    ubah_minuman(nama_slot[data_slot]);
    ubah_harga(harga_slot[data_slot]);
    lcd_xy(1,9); lcd_int(data_slot+1);
  
    if(flag_show == 0){
      for(unsigned char i=0;i<6;i++){
        h595shift(minuman_slot[i],aktif_slot[i]);
      }
      flag_show = 1;
    }
  
    cektombol();
    
    if((preset_tombol == 3) && (flag_push == 0)){
      if(data_slot >= 5){ data_slot = 0; aktif_slot[0] = 1; aktif_slot[5] = 0;}
      else{
        data_slot += 1; aktif_slot[data_slot] = 1; aktif_slot[data_slot-1] = 0;
        flag_push = 1;
      }
      flag_show = 0;
    }
    else if((preset_tombol == 4) && (flag_push == 0)){
      if(data_slot <= 0){ data_slot = 5; aktif_slot[5] = 1; aktif_slot[0] = 0;}
      else{
        data_slot -= 1; aktif_slot[data_slot] = 1; aktif_slot[data_slot+1] = 0;
        flag_push = 1;
      }
      flag_show = 0;
    }
		else if(HAL_GPIO_ReadPin(COIN_GPIO_Port,COIN_Pin) == 0){
      coin = 0;
			lcd_clear();
      flag_show = 0;
			phase = 3;
    }
    else if(HAL_GPIO_ReadPin(TAKE_GPIO_Port,TAKE_Pin) == 0){
      HAL_GPIO_WritePin(LED_TAKE_GPIO_Port,LED_TAKE_Pin,GPIO_PIN_RESET);
      lcd_clear();
      lcd_xy(1,0); lcd_string("    PROCCESSING     ");
      HAL_Delay(2000);
      if(coin >= harga_slot[data_slot]){
				if(minuman_slot[data_slot] == 0){
					lcd_xy(1,0); lcd_string("       ERROR!       ");
					lcd_xy(2,0); lcd_string("   Minuman Habis    ");
					HAL_Delay(2000);
					lcd_xy(2,0); lcd_string(" Pilih Minuman Lain ");
					HAL_Delay(2000);
					phase = 4;
					for(unsigned char i=0;i<6;i++){
					aktif_slot[i] = 0;
					}
					aktif_slot[0] = 1;
					data_slot = 0;
				}
				else{
					lcd_xy(1,0); lcd_string("       SUKSES       ");
					minuman_slot[data_slot] -= 1;
					HAL_Delay(1000);
					if((coin - harga_slot[data_slot]) > 0){
						lcd_xy(2,0); lcd_string("Ambil Uang Kembalian");
						HAL_Delay(3000);
					}
					else{
						lcd_xy(2,0); lcd_string("   Uang Anda Pas    ");
						HAL_Delay(3000);
					}
					lcd_xy(1,0); lcd_string("    TERIMA KASIH    ");
					lcd_xy(2,0); lcd_string("  Sudah Berbelanja  ");
					HAL_Delay(2000);
					coin = 0;
					phase = 3;
				}
			}
      else{
        lcd_xy(1,0); lcd_string("       ERROR!       ");
        lcd_xy(2,0); lcd_string("  Uang Anda Kurang  ");
        HAL_Delay(2000);
        lcd_xy(2,0); lcd_string(" Pilih Minuman Lain ");
        HAL_Delay(2000);
				phase = 4;
				for(unsigned char i=0;i<6;i++){
					aktif_slot[i] = 0;
				}
				aktif_slot[0] = 1;
			}
			flag_show = 0;
			data_slot = 0;
      lcd_clear();
    }
    else if(preset_tombol == 0){
      flag_push = 0;
    }
  break;
}

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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
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
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /**Common config 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CLOCK_Pin|LOAD_Pin|LED_COIN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, EN_Pin|RW_Pin|RS_Pin|D4_Pin 
                          |D5_Pin|D6_Pin|D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : CLOCK_Pin LOAD_Pin LED_COIN_Pin */
  GPIO_InitStruct.Pin = CLOCK_Pin|LOAD_Pin|LED_COIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : CLEAR_Pin COIN_Pin */
  GPIO_InitStruct.Pin = CLEAR_Pin|COIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : EN_Pin RW_Pin RS_Pin D4_Pin 
                           D5_Pin D6_Pin D7_Pin */
  GPIO_InitStruct.Pin = EN_Pin|RW_Pin|RS_Pin|D4_Pin 
                          |D5_Pin|D6_Pin|D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
