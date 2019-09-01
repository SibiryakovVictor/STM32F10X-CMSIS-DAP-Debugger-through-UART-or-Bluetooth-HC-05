## Подключение и настройка
- Соединение с целевой для отладки платой по **SWD**:

Настройка портов и пинов к которым привязано SWD находится в файле **src/main/pin_config.h**.
```
#define SWDIO_PORT          GPIOB
#define SWDIO_PIN_NUMBER      8   
#define SWCLK_PORT	        GPIOA
#define SWCLK_PIN_NUMBER      8
#define nRESET_PORT         GPIOB
#define nRESET_PIN_NUMBER     1
```
- Соединение по **UART** с платой "USB_PC_Side" / Bluetooth-модулем HC-05:
```
/* USART1 GPIO Configuration    
PA9     ------> USART1_TX
PA10     ------> USART1_RX 
*/
```
