#include "stm32f1xx_hal.h"

extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;

extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;

extern void Error_Handler(void);

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  __HAL_RCC_AFIO_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/
  /* MemoryManagement_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
  /* BusFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
  /* UsageFault_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
  /* SVCall_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
  /* DebugMonitor_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 0, 0);

  /* NOJTAG: JTAG-DP Disabled and SW-DP Enabled */
  __HAL_AFIO_REMAP_SWJ_NOJTAG();
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{

  if(htim_base->Instance==TIM3)
  {
    /* Peripheral clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();
    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(TIM3_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  }
  else if(htim_base->Instance==TIM4)
  {
    /* Peripheral clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();
    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(TIM4_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
  }

}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{

  if(htim_base->Instance==TIM3)
  {
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();

    /* Peripheral interrupt DeInit*/
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
  }
  else if(htim_base->Instance==TIM4)
  {
    /* Peripheral clock disable */
    __HAL_RCC_TIM4_CLK_DISABLE();

    /* Peripheral interrupt DeInit*/
    HAL_NVIC_DisableIRQ(TIM4_IRQn);
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(huart->Instance==USART1)
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = DBG_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DBG_TX_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = DBG_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DBG_RX_GPIO_PORT, &GPIO_InitStruct);

    /* Peripheral DMA init*/
  
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(huart,hdmarx,hdma_usart1_rx);

    hdma_usart1_tx.Instance = DMA1_Channel4;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(huart,hdmatx,hdma_usart1_tx);

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(USART1_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
  else 
  {
    if(huart->Instance==USART2) 
      {
      /* Peripheral clock enable */
      __HAL_RCC_USART2_CLK_ENABLE();
    
      /**USART2 GPIO Configuration    
      PA2     ------> USART2_TX
      PA3     ------> USART2_RX 
      */
      GPIO_InitStruct.Pin = TX_PIN;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(TX_GPIO_PORT, &GPIO_InitStruct);

      GPIO_InitStruct.Pin = RX_PIN;
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      HAL_GPIO_Init(RX_GPIO_PORT, &GPIO_InitStruct);

      /* Peripheral DMA init*/
    
      hdma_usart2_rx.Instance = DMA1_Channel6;
      hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
      hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
      hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
      hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
      hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;
      hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
      if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
      {
        Error_Handler();
      }

      __HAL_LINKDMA(huart,hdmarx,hdma_usart2_rx);

      hdma_usart2_tx.Instance = DMA1_Channel7;
      hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
      hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
      hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
      hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
      hdma_usart2_tx.Init.Mode = DMA_NORMAL;
      hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
      if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
      {
        Error_Handler();
      }

      __HAL_LINKDMA(huart,hdmatx,hdma_usart2_tx);

      /* Peripheral interrupt init */
      HAL_NVIC_SetPriority(USART2_IRQn, 4, 0);
      HAL_NVIC_EnableIRQ(USART2_IRQn);
    }
    else if(huart->Instance==USART3)
    {
      /* Peripheral clock enable */
      __HAL_RCC_USART3_CLK_ENABLE();
    
      /**USART3 GPIO Configuration    
      PB10     ------> USART3_TX
      PB11     ------> USART3_RX 
      */
      GPIO_InitStruct.Pin = USART3_TX_PIN;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(USART3_TX_GPIO_PORT, &GPIO_InitStruct);

      GPIO_InitStruct.Pin = USART3_RX_PIN;
      GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      HAL_GPIO_Init(USART3_RX_GPIO_PORT, &GPIO_InitStruct);

      /* Peripheral DMA init*/
    
        
//      hdma_usart2_rx.Instance = DMA1_Channel6;
//      hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
//      hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
//      hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
//      hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//      hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//      hdma_usart2_rx.Init.Mode = DMA_CIRCULAR;
//      hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
//      if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
//      {
//        Error_Handler();
//      }

//      __HAL_LINKDMA(huart,hdmarx,hdma_usart2_rx);

//      hdma_usart2_tx.Instance = DMA1_Channel7;
//      hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
//      hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
//      hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
//      hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//      hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//      hdma_usart2_tx.Init.Mode = DMA_NORMAL;
//      hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
//      if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
//      {
//        Error_Handler();
//      }

//      __HAL_LINKDMA(huart,hdmatx,hdma_usart2_tx);

      /* Peripheral interrupt init */
      HAL_NVIC_SetPriority(USART3_IRQn, 1, 0);
      HAL_NVIC_EnableIRQ(USART3_IRQn);
    }
  }

}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==USART1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(DBG_TX_GPIO_PORT, DBG_TX_PIN);
    HAL_GPIO_DeInit(DBG_RX_GPIO_PORT, DBG_RX_PIN);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(huart->hdmarx);
    HAL_DMA_DeInit(huart->hdmatx);

    /* Peripheral interrupt DeInit*/
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  }
  else if(huart->Instance==USART2)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(TX_GPIO_PORT, TX_PIN);
    HAL_GPIO_DeInit(RX_GPIO_PORT, RX_PIN);

    /* Peripheral DMA DeInit*/
    HAL_DMA_DeInit(huart->hdmarx);
    HAL_DMA_DeInit(huart->hdmatx);

    /* Peripheral interrupt DeInit*/
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  }

}

