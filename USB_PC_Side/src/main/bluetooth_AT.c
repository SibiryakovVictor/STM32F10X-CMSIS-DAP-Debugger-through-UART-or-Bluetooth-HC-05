/***************************************************************************
********************************************************
Описание
Файл реализует функцию настройки блютуз-модуля через AT-команды
Разработчики: Халявин Никита и Сибиряков Виктор
Заметки
Может некорректно работать если мак-адрес slave модуля имеет ведущие нули 
(но испытания не проводились ввиду отсутствия таких модулей)
****************************************************************************
*******************************************************/



/***************************************************************************************************
Локальные инклуды
***************************************************************************************************/

#include "bluetooth_AT.h"
#include "pin_config.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/***************************************************************************************************
                            Локальные дефайны
***************************************************************************************************/

#define MAX_ADDRESS_LENGTH          15
#define SIZE_OF_RESPONSE_BUFFER     64

#define BLUETOOTH_ROLE_MASTER       1
#define BLUETOOTH_ROLE_SLAVE        0
#define BLUETOOTH_ROLE BLUETOOTH_ROLE_MASTER

#define MASTER_MODULE_NAME "YOBA-USB"
#define SLAVE_NAME_FOR_CONNECTION "YOBA-SWD-CONNECTION"
#define DEFAULT_SLAVE_NAME "YOBA-SWD"

#define HIGH_LOGICAL_LEVEL 1
#define LOW_LOGICAL_LEVEL 0

#define LEVEL_TO_EXIT_FROM_AT LOW_LOGICAL_LEVEL


/***************************************************************************************************
                            Локальные типы данных
***************************************************************************************************/

typedef enum {AT_OK, AT_EXECUTION_ERROR, AT_RESULT_ERROR} StatusAT;

typedef enum {SUCCESS_EXTRACT, SUCCESS_AND_END_OF_BUFFER, 
END_OF_BUFFER, ERROR_EXTRACT} ExtractingAddressStatus;


/***************************************************************************************************
                            Прототипы локальных функций
***************************************************************************************************/

static void configKeyPin(void);
static void exitAT_Mode(void);

static void configLinkButtonPin(void);
static bool isLinkButtonPressed(void);

static void transferAT(UART_HandleTypeDef * huart, uint8_t * command, uint8_t * response);
static StatusAT setAT_Param(UART_HandleTypeDef * huart, uint8_t * checkCommand, 
  const uint8_t * desiredAnswer, uint8_t * setCommand);

static StatusAT testAT(UART_HandleTypeDef * huart);
static StatusAT resetBluetoothModule(UART_HandleTypeDef * huart);
static StatusAT uartConfig(UART_HandleTypeDef * huart);
static StatusAT roleConfig(UART_HandleTypeDef * huart);
static StatusAT nameConfig(UART_HandleTypeDef * huart, const uint8_t * name);
static StatusAT connectionModeConfig(UART_HandleTypeDef * huart);

#if BLUETOOTH_ROLE == BLUETOOTH_ROLE_MASTER
    static StatusAT getAddressOfSlave(UART_HandleTypeDef * huart, uint8_t * address);
    static StatusAT connectionAddressConfig(UART_HandleTypeDef * huart, uint8_t * address);
    static ExtractingAddressStatus getNextAddress(uint8_t * result, 
      const uint8_t * receiveBuffer, uint8_t * cursor);
    static void addressInsert(uint8_t * targetString, const uint8_t * addressSource);
    static void changeRegisterOfHexademicalLetters(uint8_t * address);
#endif


static void errorHandler(StatusAT errorStatus);



/***************************************************************************************************
                            Глобальные функции
***************************************************************************************************/


/***************************************************************************
********************************************************
Описание: посредством АТ-команд производит настройку блютуз модуля
Аргументы:
Возврат:
Замечания: на момент вызова необходимо настроить uart на бодрейт 38400
****************************************************************************
*******************************************************/
void bluetooth_config(UART_HandleTypeDef * huart)
{
    configLinkButtonPin();

    //ожидание включения модуля
    for (uint32_t i = 0; i < 10000000; i++);

    StatusAT currentStatus;

    currentStatus = testAT(huart);
    if (currentStatus != AT_OK)
    {
        errorHandler(currentStatus);
    }

    currentStatus = uartConfig(huart);
    if (currentStatus != AT_OK)
    {
        errorHandler(currentStatus);
    }

    currentStatus = roleConfig(huart);
    if (currentStatus != AT_OK)
    {
        errorHandler(currentStatus);
    }
    #if BLUETOOTH_ROLE == BLUETOOTH_ROLE_MASTER
    const uint8_t masterName[] = MASTER_MODULE_NAME;
    currentStatus = nameConfig(huart, masterName);
    if (currentStatus != AT_OK)
    {
        errorHandler(currentStatus);
    }
    #else
    if (isLinkButtonPressed())
    {
        const uint8_t slaveNameForConnection[] = SLAVE_NAME_FOR_CONNECTION;
        currentStatus = nameConfig(huart, slaveNameForConnection);
        if (currentStatus != AT_OK)
        {
            errorHandler(currentStatus);
        }
    }
    else
    {
        const uint8_t defaultSlaveName[] = DEFAULT_SLAVE_NAME;
        currentStatus = nameConfig(huart, defaultSlaveName);
        if (currentStatus != AT_OK)
        {
            errorHandler(currentStatus);
        }
    }
    #endif

    currentStatus = connectionModeConfig(huart);
    if (currentStatus != AT_OK)
    {
        errorHandler(currentStatus);
    }

    #if BLUETOOTH_ROLE == BLUETOOTH_ROLE_MASTER
    if (isLinkButtonPressed())
    {
        //поиск slave-устройства (ищется устройство с заданным именем)

        uint8_t address[MAX_ADDRESS_LENGTH];
        currentStatus = getAddressOfSlave(huart, address);
        if (currentStatus != AT_OK)
        {
            errorHandler(currentStatus);
        }

        currentStatus = connectionAddressConfig(huart, address);
        if (currentStatus != AT_OK)
        {
            errorHandler(currentStatus);
        }
    }
    #endif

    configKeyPin();
    exitAT_Mode();
    currentStatus = resetBluetoothModule(huart);
}


/***************************************************************************************************
                            Локальные функции
***************************************************************************************************/


static void transferAT(UART_HandleTypeDef * huart, uint8_t * command, uint8_t * response)
{
    HAL_UART_Transmit(huart, command, strlen((const char *)command), 500 );
    HAL_UART_Receive(huart, response, SIZE_OF_RESPONSE_BUFFER, 500 ); 
} 



/***************************************************************************
********************************************************
Описание: проверяет отклик модуля на АТ-команду
Аргументы:
Возврат: AT_OK если получен ответ ОК, иначе AT_EXECUTION_ERROR
Замечания:
****************************************************************************
*******************************************************/
static StatusAT testAT(UART_HandleTypeDef * huart)
{
    uint8_t response[SIZE_OF_RESPONSE_BUFFER] = {0};
    uint8_t testCommand[] = "AT\r\n"; 
    uint8_t okResponse[] = "OK\r\n";
    transferAT(huart, testCommand, response);
    if (!strncmp((const char *)response, (const char *)okResponse, strlen((char *)okResponse) ) )
    {
        return AT_OK;
    }
    else
    {
        return AT_EXECUTION_ERROR;
    }
}

static StatusAT resetBluetoothModule(UART_HandleTypeDef * huart)
{
    uint8_t resetCommand[] = {"AT+RESET\r\n"};
    HAL_UART_Transmit(huart, resetCommand, strlen((const char *)resetCommand), 500 );
    return AT_OK;
}



/***************************************************************************
********************************************************
Описание: запрашивает значение параметра модуля и сравнивает с желаемым. Если различаются - 
пытается изменить на желаемый
Аргументы: указатели на 3 строки: команда проверки параметра, желаемый результат и команда установки параметра
Возврат: если в результате работы функции получен желаемый ответ на запрос параметра, возвращает 
AT_OK, если ответ на команду установки параметра отличен от ОК, возвращает AT_EXECUTION_ERROR,
иначе если ответ на запрос параметра так и не совпал с желаемым возвращает AT_RESULT_ERROR
Замечания:
****************************************************************************
*******************************************************/
static StatusAT setAT_Param(UART_HandleTypeDef * huart, uint8_t * checkCommand, const uint8_t * desiredAnswer, uint8_t * setCommand)
{
    uint8_t response[SIZE_OF_RESPONSE_BUFFER] = {0};
    const uint8_t okResponse[] = "OK\r\n";

    //проверка текущего значения параметра
    transferAT(huart, checkCommand, response);  
    if (!strncmp((const char *)response, (const char *)desiredAnswer, strlen((char *)desiredAnswer) ) )
    {
        return AT_OK;
    }

    //команда установки параметра
    transferAT(huart, setCommand, response);
    if (strncmp((const char *)response, (const char *)okResponse, strlen((char *)okResponse) ) )
    {
        return AT_EXECUTION_ERROR;
    }

    //проверка, получен ли в итоге желаемый ответ
    transferAT(huart, checkCommand, response);  
    if (!strncmp((const char *)response, (const char *)desiredAnswer, strlen((char *)desiredAnswer) ) )
    {
        return AT_OK;
    }
    else
    {
        return AT_RESULT_ERROR;
    }
}


static StatusAT uartConfig(UART_HandleTypeDef * huart)
{
    uint8_t uartCheckConfig[] = "AT+UART?\r\n";    
    uint8_t uartDesiredAnswer[] = "+UART:115200,1,0\r\nOK\r\n";
    uint8_t uartSetConfig[] = "AT+UART=115200,1,0\r\n";
    return setAT_Param(huart, uartCheckConfig, uartDesiredAnswer, uartSetConfig);
}


/***************************************************************************
********************************************************
Описание: настраивает роль (master/slave)
Аргументы: 
Возврат: результат работы (AT_OK успех, AT_RESULT_ERROR - не удалось изменить роль, 
AT_EXECUTION_ERROR - ошибка передачи AT-команд)
Замечания: 
****************************************************************************
*******************************************************/
static StatusAT roleConfig(UART_HandleTypeDef * huart)
{
    uint8_t roleCheck[] = "AT+ROLE?\r\n";    
    #if BLUETOOTH_ROLE == BLUETOOTH_ROLE_MASTER
    uint8_t roleDesiredAnswer[] = "+ROLE:1\r\nOK\r\n";
    uint8_t roleSet[] = "AT+ROLE=1\r\n";
    #else
    uint8_t roleDesiredAnswer[] = "+ROLE:0\r\nOK\r\n";
    uint8_t roleSet[] = "AT+ROLE=0\r\n";
    #endif
    return setAT_Param(huart, roleCheck, roleDesiredAnswer, roleSet);
}


/***************************************************************************
********************************************************
Описание: формирует строки АТ-команды и ожидаемого ответа для установки имени 
и выполняет установку
Аргументы: строка с новым именем (не длиннее maxNameLength)
Возврат: результат работы (AT_OK успех, AT_RESULT_ERROR - не удалось изменить имя, 
AT_EXECUTION_ERROR - ошибка передачи AT-команд)
Замечания: 
****************************************************************************
*******************************************************/
static StatusAT nameConfig(UART_HandleTypeDef * huart, const uint8_t * name)
{
    const uint32_t maxNameLength = 64;

    const uint8_t nameDesiredAnswerBegin[] = "+NAME:";
    const uint8_t nameDesiredAnswerEnd[] = "\r\nOK\r\n";

    //размер строк с началом и концом команды без учёта нуль-терминаторов
    const uint32_t beginAndEndOfDesiredAnswerLength = sizeof(nameDesiredAnswerBegin) + 
    sizeof(nameDesiredAnswerEnd) - 2;
    uint8_t nameDesiredAnswer[beginAndEndOfDesiredAnswerLength + maxNameLength] = {0};
    strcat((char *)nameDesiredAnswer, (const char *)nameDesiredAnswerBegin);
    strncat((char *)nameDesiredAnswer, (const char *)name, maxNameLength);
    strcat((char *)nameDesiredAnswer, (const char *)nameDesiredAnswerEnd);

    const uint8_t nameSetBegin[] = "AT+NAME=";
    const uint8_t nameSetEnd[] = "\r\n";

    //размер строк с началом и концом команды без учёта нуль-терминаторов
    const uint32_t beginAndEndOfSetLength = sizeof(nameSetBegin) + sizeof(nameSetEnd) - 2;
    uint8_t nameSet[beginAndEndOfSetLength + maxNameLength] = {0};
    strcat((char *)nameSet, (const char *)nameSetBegin);
    strncat((char *)nameSet, (const char *)name, maxNameLength);
    strcat((char *)nameSet, (const char *)nameSetEnd);

    uint8_t nameCheck[] = "AT+NAME?\r\n";    
    return setAT_Param(huart, nameCheck, nameDesiredAnswer, nameSet);
}


/***************************************************************************
********************************************************
Описание: получает адрес устройства, имеющего фиксированное имя - маркер slave в режиме соединения
Аргументы: 
Возврат: результат работы (AT_OK успех, AT_RESULT_ERROR - не удалось изменить режим, 
AT_EXECUTION_ERROR - ошибка передачи AT-команд)
Замечания: ответ реального модуля отличался от ответа по даташиту (по даташиту в строке
connectionModeDesiredAnswer вместо CMOD должно ыть CMODE)
****************************************************************************
*******************************************************/
static StatusAT connectionModeConfig(UART_HandleTypeDef * huart)
{
    uint8_t connectionModeCheck[] = "AT+CMODE?\r\n";    
    #if BLUETOOTH_ROLE == BLUETOOTH_ROLE_MASTER
        uint8_t connectionModeDesiredAnswer[] = "+CMOD:0\r\nOK\r\n";
        uint8_t connectionModeSet[] = "AT+CMODE=0\r\n";
    #else
        uint8_t connectionModeDesiredAnswer[] = "+CMOD:1\r\nOK\r\n";
        uint8_t connectionModeSet[] = "AT+CMODE=1\r\n";
    #endif
    return setAT_Param(huart, connectionModeCheck, connectionModeDesiredAnswer, connectionModeSet);
}


#if BLUETOOTH_ROLE == BLUETOOTH_ROLE_MASTER
/***************************************************************************
********************************************************
Описание: получает адрес устройства, имеющего фиксированное имя - маркер slave в режиме соединения
Аргументы: строка для сохранения адреса (не меньше 15 символов)
Возврат: результат работы (AT_OK успех, AT_RESULT_ERROR - устройство не найдено, 
AT_EXECUTION_ERROR - ошибка передачи AT-команд)
Замечания: функция работает десятки секунд, если мак-адрес содержит ведущие нули может работать 
некорректно
****************************************************************************
*******************************************************/
    static StatusAT getAddressOfSlave(UART_HandleTypeDef * huart, uint8_t * address)
    {    
        const uint8_t okResponse[] = "OK\r\n";

        const uint32_t sizeOfResponseBuffer = 64;
        uint8_t response[sizeOfResponseBuffer];

        uint8_t initCommand[] = "AT+INIT\r\n";    
        transferAT(huart, initCommand, response);
        if (strncmp((const char *)response, (const char *)okResponse, strlen((char *)okResponse) ) )
        {
            return AT_EXECUTION_ERROR;
        }

        uint8_t inqm[] = "AT+INQM=0,8,10\r\n";    
        transferAT(huart, inqm, response);
        if (strncmp((const char *)response, (const char *)okResponse, strlen((char *)okResponse) ) )
        {
            return AT_EXECUTION_ERROR; 
        }

        const uint32_t sizeOfAdressResponseBuffer = 256;
        uint8_t responseForResults[sizeOfAdressResponseBuffer];
        uint8_t searchDevices[] = "AT+INQ\r\n";    

        //таймаут поиска задан в миллисекундах
        const uint32_t searchingTimeOut = 15000;

        HAL_UART_Transmit(huart, searchDevices, strlen((const char *)searchDevices), 500 );
        HAL_UART_Receive(huart, responseForResults, sizeOfAdressResponseBuffer, searchingTimeOut );

        uint8_t cursor = 0;

        while (1)
        {
            ExtractingAddressStatus result = getNextAddress(address, responseForResults, &cursor);
            if ( (result == END_OF_BUFFER) || (result == ERROR_EXTRACT) )
            { 
                return AT_RESULT_ERROR;
            }
            else
            {
                //тайм-фут запроса имени задан в миллисекундах
                const uint32_t nameRequestTimeOut = 5000;

                #define SEARCHING_ANSWER_BEGIN "+RNAME:"
                uint8_t searchingAnswer[] = SEARCHING_ANSWER_BEGIN SLAVE_NAME_FOR_CONNECTION;

                uint8_t getName[] = "AT+RNAME?xxxx,xx,xxxxxx\r\n";
                addressInsert(getName, address);

                HAL_UART_Transmit(huart, getName, strlen((const char *)getName), 500 );
                HAL_UART_Receive(huart, response, sizeOfResponseBuffer, nameRequestTimeOut );

                if (!strncmp((const char *)response, (const char *)searchingAnswer, 
                strlen((const char *)searchingAnswer) ) )
                {  
                    return AT_OK;
                }

                if (result == SUCCESS_AND_END_OF_BUFFER)
                {
                    return AT_RESULT_ERROR;
                }
            }
        }
    }


/***************************************************************************
********************************************************
Описание: заменяет прописные буквы (только шестнадцатеричные цифры) на строчные
Аргументы: строка для изменения
Возврат:
Замечания:
****************************************************************************
*******************************************************/
static void changeRegisterOfHexademicalLetters(uint8_t * address)
{
    for (uint32_t cursor = 0; cursor < strlen((const char *)address); cursor++)
    {
        if ( (address[cursor] >= 'A') && ((address[cursor] <= 'F')) )
        {
            address[cursor] -= ('A' - 'a');
        }
    }
}


/***************************************************************************
********************************************************
Описание: устанавливает адрес сопряжённого модуля
Аргументы: строка с адресом
Возврат:
Замечания: строка с адресом обязательно должна содержать 12 шестнадцатеричных цифр, 
допускается разделение двоеточием
****************************************************************************
*******************************************************/
static StatusAT connectionAddressConfig(UART_HandleTypeDef * huart, uint8_t * address)
{
    changeRegisterOfHexademicalLetters(address);
    uint8_t connectionAddressCheck[] = "AT+BIND?\r\n";    
    uint8_t connectionAddressDesiredAnswer[] = "+BIND:xxxx:xx:xxxxxx\r\nOK\r\n";
    addressInsert(connectionAddressDesiredAnswer, address);
    uint8_t connectionAddressSet[] = "AT+BIND=xxxx,xx,xxxxxx\r\n";
    addressInsert(connectionAddressSet, address);
    return setAT_Param(huart, connectionAddressCheck, connectionAddressDesiredAnswer, connectionAddressSet);
}

/***************************************************************************
********************************************************
Описание: заменяет первые 12 символов 'x' в строке targetString на символы из строки addressSource
Аргументы:
Возврат:
Замечания: функция игнорирует двоеточия в строке addressSource
****************************************************************************
*******************************************************/
static void addressInsert(uint8_t * targetString, const uint8_t * addressSource)
{
    const uint8_t numOfSymbols = 12;

    uint8_t cursorSource = 0;
    uint8_t symbolsChanged = 0;

    uint8_t addressStringLength = strlen((const char *)addressSource);
    uint8_t targetStringLength = strlen((const char *)targetString);

    for (uint8_t coursorTarget = 0; coursorTarget < targetStringLength; coursorTarget++)
    {
        if (targetString[coursorTarget] == 'x')
        {
            while (addressSource[cursorSource] == ':')
            {
                cursorSource++;
            }
            targetString[coursorTarget] = addressSource[cursorSource];
            cursorSource++;
            if (cursorSource > addressStringLength)
            {
                //символы адреса закончились
                return;
            }
            symbolsChanged++;
            if (symbolsChanged == numOfSymbols)
            {
                return;
            }
        }
    }
}


/***************************************************************************
********************************************************
Описание: извлекает ближайший после положения курсора адрес и модифицирует курсор
Аргументы: указатель на строку результата (не менее 15 символов), указатель на буфер приёма 
и на курсор
Возврат: результат попытки извлечения
Замечания: если в мак-адресе модуля есть ведущие нули, формат выхода может отличаться
****************************************************************************
*******************************************************/
static ExtractingAddressStatus getNextAddress(uint8_t * result, 
  const uint8_t * receiveBuffer, uint8_t * cursor)
{
    if (receiveBuffer[*cursor] == '+')
    {
        (*cursor) += 5;
        uint32_t resultCursor = 0;

        for (; resultCursor < MAX_ADDRESS_LENGTH; resultCursor++)
        {
            if (receiveBuffer[*cursor] != ',')
            {
                result[resultCursor] = receiveBuffer[*cursor];
            }
            else
            {
                result[resultCursor] = 0;
                break;
            }
            (*cursor)++;
        }

        while (receiveBuffer[*cursor] != '\n')
        {
            if (*cursor == 255)
            {
              return SUCCESS_AND_END_OF_BUFFER;
            }
            (*cursor)++;
        }

        return SUCCESS_EXTRACT;
    }
    else 
    {
        if (receiveBuffer[*cursor] == 'O')
        {
            return END_OF_BUFFER;
        }
        else
        {
            return ERROR_EXTRACT;
        }
    }
}
#endif


/***************************************************************************
********************************************************
Описание: настраивает пин, который переводит модуль в режим АТ-команд
Аргументы:
Возврат:
Замечания: сам факт настройки пина может привести к выходу модуля из режима АТ-команд 
(но зависит от схемы)
****************************************************************************
*******************************************************/
static void configKeyPin()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = BLUETOOTH_KEY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BLUETOOTH_KEY_PORT, &GPIO_InitStruct);
}


static void exitAT_Mode()
{
    #if LEVEL_TO_EXIT_FROM_AT == LOW_LOGICAL_LEVEL
        //вывести логический 0
        BLUETOOTH_KEY_PORT->ODR &= ~BLUETOOTH_KEY_PIN;  
    #else
        //вывести логическую 1
        BLUETOOTH_KEY_PORT->ODR |= BLUETOOTH_KEY_PIN;
    #endif
}


/***************************************************************************
********************************************************
Описание: настраивает пин, к которому подключается кнопка перехода в режим соединения
Аргументы:
Возврат:
Замечания: предполагается что при нажатой кнопке на пине низкий логический уровень
****************************************************************************
*******************************************************/
static void configLinkButtonPin()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = BLUETOOTH_LINK_BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BLUETOOTH_LINK_BUTTON_PORT, &GPIO_InitStruct);
} 


/***************************************************************************
********************************************************
Описание: возвращает true если кнопка режима соединения нажата
Аргументы:
Возврат:
Замечания: предполагается что при нажатой кнопке на пине низкий логический уровень
****************************************************************************
*******************************************************/
static bool isLinkButtonPressed()
{
    if ( (BLUETOOTH_LINK_BUTTON_PORT->IDR & BLUETOOTH_LINK_BUTTON_PIN) == 0)
    {
        return true;
    }
    return false;
}


static void errorHandler(StatusAT errorStatus)
{
    if (errorStatus == AT_EXECUTION_ERROR)
    {
        //можно добавить индикацию ошибки, вызванной некорректным возвратом
    }
    if (errorStatus == AT_RESULT_ERROR)
    {
        //можно добавить индикацию ошибки, вызванной некорректным результатом выполнения команды
    }
    while (1)
    {

    }
}
