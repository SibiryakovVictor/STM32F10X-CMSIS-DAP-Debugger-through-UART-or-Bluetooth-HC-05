#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_dap_cdc.h"
#include "usbd_dap_cdc_if.h"

/* USB Device Core handle declaration */
USBD_HandleTypeDef hUsbDeviceFS;
extern UART_HandleTypeDef huart2;

/* init function */				        
void MX_USB_DEVICE_Init(void)
{
  HAL_UART_Receive_DMA(&huart2, UserTxBufferFS, APP_RX_DATA_SIZE);
  TxReadPtr = 0;

  /* Init Device Library,Add Supported Class and Start the library*/
  USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);

  USBD_RegisterClass(&hUsbDeviceFS, &USBD_DAP_CDC);

  USBD_DAP_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops);

  USBD_Start(&hUsbDeviceFS);

}


