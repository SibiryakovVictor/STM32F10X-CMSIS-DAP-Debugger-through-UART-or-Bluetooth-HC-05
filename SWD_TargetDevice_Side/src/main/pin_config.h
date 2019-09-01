#ifndef SWD_PIN_CONFIG_H_
#define SWD_PIN_CONFIG_H_
#include "stm32f1xx_hal_gpio.h"

/* определение портов и пинов SWD */
#define SWDIO_PORT          GPIOB
#define SWDIO_PIN_NUMBER      8   // не баг, а фича: так приходится определять для быстродействия, см. макросы в DAP_config.h
#define SWCLK_PORT	        GPIOA
#define SWCLK_PIN_NUMBER      8
#define nRESET_PORT         GPIOB
#define nRESET_PIN_NUMBER     1

#define SWDIO_PIN   ((uint16_t)(1 << SWDIO_PIN_NUMBER))
#define SWCLK_PIN   ((uint16_t)(1 << SWCLK_PIN_NUMBER))
#define nRESET_PIN  ((uint16_t)(1 << nRESET_PIN_NUMBER))

/* остальные определения */
#define BLUETOOTH_KEY_PIN               GPIO_PIN_6
#define BLUETOOTH_KEY_PORT              GPIOC

#define BLUETOOTH_LINK_BUTTON_PIN       GPIO_PIN_5
#define BLUETOOTH_LINK_BUTTON_PORT      GPIOC

#define LED1_PIN                        GPIO_PIN_13
#define LED1_GPIO_PORT                  GPIOC

#define DAP_LED_1_PIN                   GPIO_PIN_0
#define DAP_LED_1_GPIO_PORT             GPIOA
#define DAP_LED_2_PIN                   GPIO_PIN_1
#define DAP_LED_2_GPIO_PORT             GPIOA


#define SYS_SWDIO_PIN                   GPIO_PIN_13
#define SYS_SWDIO_GPIO_PORT             GPIOA
#define SYS_SWCLK_PIN                   GPIO_PIN_14
#define SYS_SWCLK_GPIO_PORT             GPIOA

#endif /* SWD_PIN_CONFIG_H_ */
