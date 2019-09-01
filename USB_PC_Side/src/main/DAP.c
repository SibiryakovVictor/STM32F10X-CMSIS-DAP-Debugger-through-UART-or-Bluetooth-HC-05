#include "pin_config.h"
#include "string.h"
#include "DAP.h"
#include "stm32f1xx_hal.h"

#define DAP_FW_VER      "1.0"   // Firmware Version

#define DAP_PACKET_SIZE       64
#define DAP_PACKET_COUNT      1


DAP_Data_t DAP_Data;            // DAP Data
__IO uint8_t DAP_TransferAbort; // Transfer Abort Flag

const char DAP_FW_Ver [] = DAP_FW_VER;
extern UART_HandleTypeDef huart3;

void sendThroughtUartWithHandler(uint8_t * request) {
  uint8_t protocolHandler[] = { 0xAA, 0xBB };  
  HAL_UART_Transmit( &huart3, protocolHandler, 2, 500 );
  HAL_UART_Transmit( &huart3, request, 64, 500 );
}

void transfer_throught_UART(uint8_t *request, uint8_t *response) {
 
  HAL_UART_Receive( &huart3, response, 1, 10 );
  sendThroughtUartWithHandler(request);
  HAL_UART_Receive_IT( &huart3, response, 64 );
  while(huart3.RxXferCount > 0) {}
}

void send_request_throurht_UART(uint8_t * requestSecondByte)
{
  sendThroughtUartWithHandler(requestSecondByte - 1);
  for(uint32_t i = 0; i < 1000; i++) {}
}




// Get DAP Information
//   id:      info identifier
//   info:    pointer to info data
//   return:  number of bytes in info data
static uint8_t DAP_Info(uint8_t id, uint8_t* info)
{
  uint8_t length = 0;

  switch (id) {
    case DAP_ID_VENDOR:
      break;
    case DAP_ID_PRODUCT:
      break;
    case DAP_ID_SER_NUM:
      break;
    case DAP_ID_FW_VER:
      memcpy(info, DAP_FW_Ver, sizeof(DAP_FW_Ver)+1);
      length = sizeof(DAP_FW_Ver)+1;
      break;
    case DAP_ID_DEVICE_VENDOR:
      break;
    case DAP_ID_DEVICE_NAME:
      break;
    case DAP_ID_CAPABILITIES:
      info[0] = 1;  /* SWD */
      length = 1;
      break;
    case DAP_ID_PACKET_SIZE:
      info[0] = (uint8_t)(DAP_PACKET_SIZE >> 0);
      info[1] = (uint8_t)(DAP_PACKET_SIZE >> 8);
      length = 2;
      break;
    case DAP_ID_PACKET_COUNT:
      info[0] = DAP_PACKET_COUNT;
      length = 1;
      break;
    default:
      break;
  }

  return (length);
}


// Process Host Status command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response
static uint32_t DAP_HostStatus(uint8_t *request, uint8_t *response)
{
  switch (*request) {
    case DAP_DEBUGGER_CONNECTED:
      if (*(request+1) & 1) {
        DAP_LED_1_GPIO_PORT->BSRR = DAP_LED_1_PIN;
      } else {
        DAP_LED_1_GPIO_PORT->BRR = DAP_LED_1_PIN;
      }
      break;
    case DAP_TARGET_RUNNING:
      if (*(request+1) & 1) {
        DAP_LED_2_GPIO_PORT->BSRR = DAP_LED_2_PIN;
      } else {
        DAP_LED_2_GPIO_PORT->BRR = DAP_LED_2_PIN;
      }
      break;
    default:
      *response = DAP_ERROR;
      return (1);
  }

  *response = DAP_OK;
  return (1);
}

// Process Connect command and prepare response
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response
static uint32_t DAP_Connect(uint8_t *request, uint8_t *response)
{
  uint32_t port;

  if (*request == DAP_PORT_AUTODETECT) {
    port = 1;   /* SWD */
  } else {
    port = *request;
  }

  switch (port) {
    case DAP_PORT_SWD:
      DAP_Data.debug_port = DAP_PORT_SWD;
      send_request_throurht_UART(request);
      LED1_GPIO_PORT->BRR = LED1_PIN;
      break;

    default:
      *response = DAP_PORT_DISABLED;
      return (1);
  }

  *response = port;
  return (1);
}



// Process DAP Vendor command and prepare response
// Default function (can be overridden)
//   request:  pointer to request data
//   response: pointer to response data
//   return:   number of bytes in response
// this function is declared as __weak in DAP.c
uint32_t DAP_ProcessVendorCommand(uint8_t *request, uint8_t *response)
{
  *response = ID_DAP_Invalid;
  return (1);
}



// Process DAP command and send it to swd side, prepare response
// request:  pointer to request data
// response: pointer to response data
// return:   number of bytes in response
uint32_t DAP_ProcessCommandNew(uint8_t *request, uint8_t *response)
{
  uint32_t num;

  if ((*request >= ID_DAP_Vendor0) && (*request <= ID_DAP_Vendor31)) {
    return DAP_ProcessVendorCommand(request, response);
  }

  *response++ = *request;

  switch (*request++) {
    case ID_DAP_Info:
      num = DAP_Info(*request, response+1);
      *response = num;
      return (2 + num);
    case ID_DAP_HostStatus:
      num = DAP_HostStatus(request, response);
      break;
    case ID_DAP_Connect:
      send_request_throurht_UART(request);
      num = DAP_Connect(request, response);
      break;
    case ID_DAP_Disconnect:            
      send_request_throurht_UART(request);
      *response = DAP_OK;
      break;

    case ID_DAP_TransferConfigure:           
      send_request_throurht_UART(request);
      *response = DAP_OK;
      break;

    case ID_DAP_Transfer:
      transfer_throught_UART(request-1, response-1);
      break;

    case ID_DAP_TransferBlock: 
      transfer_throught_UART(request-1, response-1);
      break;

    case ID_DAP_WriteABORT:
      send_request_throurht_UART(request);
      *response = DAP_OK;
      break;

    case ID_DAP_SWJ_Pins:
      transfer_throught_UART(request-1, response-1);
      break;
    case ID_DAP_SWJ_Clock:               
      send_request_throurht_UART(request);
      *response = DAP_OK;      
      break;
    case ID_DAP_SWJ_Sequence:           
      send_request_throurht_UART(request);
      *response = DAP_OK;  
      break;
    case ID_DAP_SWD_Configure:                
      send_request_throurht_UART(request);
      *response = DAP_OK; 
      break;

    default:
      *(response-1) = ID_DAP_Invalid;
      return (1);
  }

  return (1 + num);
}



// Setup DAP
void DAP_Setup(void) {

  // Default settings (only non-zero values)
//DAP_Data.debug_port  = 0;
//DAP_Data.fast_clock  = 0;
  DAP_Data.clock_delay = 2;
//DAP_Data.transfer.idle_cycles = 0;
  DAP_Data.transfer.retry_count = 100;
//DAP_Data.transfer.match_retry = 0;
//DAP_Data.transfer.match_mask  = 0x000000;
  DAP_Data.swd_conf.turnaround  = 1;
//DAP_Data.swd_conf.data_phase  = 0;

}
