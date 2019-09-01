#ifndef __USBD_CDC_IF_H
#define __USBD_CDC_IF_H

#ifdef __cplusplus
 extern "C" {
#endif
#include "usbd_dap_cdc.h"

/* Define size for the receive and transmit buffer over CDC */
/* It's up to user to redefine and/or remove those define */
#define APP_RX_DATA_SIZE  2048
#define APP_TX_DATA_SIZE  2048
  
extern USBD_DAP_CDC_ItfTypeDef  USBD_Interface_fops;

extern uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];
extern uint32_t TxReadPtr;

uint8_t CDC_Transmit(uint8_t* Buf, uint16_t Len);

  
#ifdef __cplusplus
}
#endif
  
#endif /* __USBD_CDC_IF_H */

