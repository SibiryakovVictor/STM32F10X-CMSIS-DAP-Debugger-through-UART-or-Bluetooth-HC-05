#ifndef SWD_PIN_CONFIG_H_
#define SWD_PIN_CONFIG_H_
#include "stm32f1xx_hal_gpio.h"

#define USB_UP              GPIO_PIN_11
#define USB_UP_PORT         GPIOC

#define LED1_PIN            GPIO_PIN_13
#define LED1_GPIO_PORT      GPIOC

#define DAP_LED_1_PIN       GPIO_PIN_0
#define DAP_LED_1_GPIO_PORT GPIOA
#define DAP_LED_2_PIN       GPIO_PIN_1
#define DAP_LED_2_GPIO_PORT GPIOA

#define TX_PIN              GPIO_PIN_2
#define TX_GPIO_PORT        GPIOA
#define RX_PIN              GPIO_PIN_3
#define RX_GPIO_PORT        GPIOA

#define USART3_TX_PIN        GPIO_PIN_10
#define USART3_TX_GPIO_PORT  GPIOB
#define USART3_RX_PIN        GPIO_PIN_11
#define USART3_RX_GPIO_PORT  GPIOB

#define DBG_TX_PIN          GPIO_PIN_9
#define DBG_TX_GPIO_PORT    GPIOA
#define DBG_RX_PIN          GPIO_PIN_10
#define DBG_RX_GPIO_PORT    GPIOA

#define USB_DM_PIN          GPIO_PIN_11
#define USB_DM_GPIO_PORT    GPIOA
#define USB_DP_PIN          GPIO_PIN_12
#define USB_DP_GPIO_PORT    GPIOA
#define USB_FS_PIN          GPIO_PIN_15
#define USB_FS_GPIO_PORT    GPIOA

#define BLUETOOTH_KEY_PORT GPIOB
#define BLUETOOTH_KEY_PIN  GPIO_PIN_12

#define BLUETOOTH_LINK_BUTTON_PORT GPIOB
#define BLUETOOTH_LINK_BUTTON_PIN  GPIO_PIN_13

#endif /* SWD_PIN_CONFIG_H_ */
