#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "usbd_def.h"

#define USBD_MAX_NUM_INTERFACES        2
#define USBD_MAX_NUM_CONFIGURATION     1
#define USBD_MAX_STR_DESC_SIZ         64
#define USBD_SUPPORT_USER_STRING       1
#define USBD_DEBUG_LEVEL               0
#define USBD_SELF_POWERED              1

/* #define for FS and HS identification */
#define DEVICE_FS 		0

#define USBD_DAP_OUTREPORT_BUF_SIZE   64
#define USBD_DAP_REPORT_DESC_SIZE     33

/* USBD_Exported_Macros */ 

#define USBD_Delay   HAL_Delay

/* DEBUG macros */    
#if (USBD_DEBUG_LEVEL > 0)
#define  USBD_UsrLog(...)   printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_UsrLog(...)   
#endif 
                            
                            
#if (USBD_DEBUG_LEVEL > 1)

#define  USBD_ErrLog(...)   printf("ERROR: ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_ErrLog(...)   
#endif 
                            
                            
#if (USBD_DEBUG_LEVEL > 2)                         
#define  USBD_DbgLog(...)   printf("DEBUG : ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_DbgLog(...)                         
#endif

#endif /*__USBD_CONF__H__*/


