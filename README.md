# STM32F10X-CMSIS-DAP-Debugger-through-UART-or-Bluetooth-HC-05
Проект CMSIS-DAP отладчика, позволяющего выполнять отладку целевой платы на STM32F10X, используя две платы на STM32F10X, соединенные между собой либо UART'ом, либо Bluetooth'ом (в данном проекте модулями HC-05), одна из которых подключается к PC с Keil по USB, а другая по SWD с отлаживаемой платой.

Выполнено на основе проекта "YOBA-DAP-Programmer":
https://github.com/ArtemZaZ/YOBA-DAP-Programmer

## Сборка
Проект собран для Keil 5.

## Подключение и соединение
Описано в файлах README в папках **SWD_TargetDevice_Side** и **USB_PC_Side**.

## Выбор интерфейса соединения частей отладчика
- Для того чтобы обмен данными между частями отладчика происходил посредством Bluetooth-модулей HC-05, необходимо **В ОБОИХ ПРОЕКТАХ** в начале файла **src/main/main.c** (строка №7) раскомментировать определение макроса BLUETOOTH_HC05_TRANSFER_MODE (раскомментировано по умолчанию):
```
#define BLUETOOTH_HC05_TRANSFER_MODE
```
- Для обмена данными между частями отладчика по UART достаточно закомментировать вышеописанную строку по указанному местоположению:
```
//#define BLUETOOTH_HC_05_TRANSFER_MODE
```
## Настройка Keil'а под данный отладчик
![Image alt](https://github.com/SibiryakovVictor/STM32F10X-CMSIS-DAP-Debugger-through-UART-or-Bluetooth-HC-05/blob/master/images/Keil_settings1.png)
![Image alt](https://github.com/SibiryakovVictor/STM32F10X-CMSIS-DAP-Debugger-through-UART-or-Bluetooth-HC-05/blob/master/images/Keil_settings2.png) 
