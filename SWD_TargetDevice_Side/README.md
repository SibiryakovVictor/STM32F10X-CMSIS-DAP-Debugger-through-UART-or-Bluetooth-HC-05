# YOBA-DAP-Programmer
Версия программатора-отладчика YOBA-DAP на базе STM32f10x.
Доступно только SWD.
## Сборка 
Проект собран для Keil 5.

## Настройка и подключение
Настройка портов и пинов к которым привязано SWD находится в файле **src/main/pin_config.h**.
```
#define SWDIO_PORT	        GPIOB
#define SWDIO_PIN_NUMBER	  8   
#define SWCLK_PORT	        GPIOA
#define SWCLK_PIN_NUMBER	  5
#define nRESET_PORT	        GPIOA
#define nRESET_PIN_NUMBER	  6
```
**После их изменения Проект необходимо пересобрать**
## USB 
USB подцеплено к пинам 11 и 12 на порте A
## Настройка Keil'а под YOBA-DAP
![Image alt](https://github.com/ArtemZaZ/YOBA-DAP-Programmer/raw/stm32f1xx/images/yoba-dap-config.png)
