#ifndef __DAP_CONFIG_H__
#define __DAP_CONFIG_H__

#include "stm32f1xx_hal.h"
#include "stdint.h"

#define CPU_CLOCK             72000000        ///< Specifies the CPU Clock in Hz
#define IO_PORT_WRITE_CYCLES  2 ///< I/O Cycles: 2=default, 1=Cortex-M0+ fast I/0

#define DAP_PACKET_SIZE       64
#define DAP_PACKET_COUNT      1

/* PIN_nRESET_LOW() */
#if (nRESET_PIN_NUMBER < 8)   // т.к. в stmf1 регистры CR разделены на CRL и CRH приходится так костылить
# define PIN_nRESET_LOW()   do {  \
  nRESET_PORT->CRL = (nRESET_PORT->CRL & ~(0xF << (4*nRESET_PIN_NUMBER))) \
                | (0x07 << (4*nRESET_PIN_NUMBER)); \
  } while (0)
#else    
# define PIN_nRESET_LOW()   do {  \
  nRESET_PORT->CRH = (nRESET_PORT->CRH & ~(0xF << (4*(nRESET_PIN_NUMBER - 8)))) \
                | (0x07 << (4*(nRESET_PIN_NUMBER - 8))); \
  } while (0)
#endif  /* PIN_nRESET_LOW() */

  
/* PIN_nRESET_HIGH() */
#if (nRESET_PIN_NUMBER < 8)   
# define PIN_nRESET_HIGH()  do {  \
  nRESET_PORT->CRL = (nRESET_PORT->CRL & ~(0xF << (4*nRESET_PIN_NUMBER))) \
                | (0x04 << (4*nRESET_PIN_NUMBER));  \
  } while (0)
#else
  define PIN_nRESET_HIGH()  do {  \
  nRESET_PORT->CRH = (nRESET_PORT->CRH & ~(0xF << (4*(nRESET_PIN_NUMBER - 8)))) \
                | (0x04 << (4*(nRESET_PIN_NUMBER - 8)));  \
  } while (0)
#endif  /* PIN_nRESET_HIGH() */

  
/*   PIN_SWDIO_OUT_ENABLE() */
/** SWDIO I/O pin: Switch to Output mode (used in SWD mode only).
Configure the SWDIO DAP hardware I/O pin to output mode. This function is
called prior \ref PIN_SWDIO_OUT function calls.
*/
#if (SWDIO_PIN_NUMBER < 8)
# define PIN_SWDIO_OUT_ENABLE(void) do { \
  SWDIO_PORT->CRL = (SWDIO_PORT->CRL & ~(0xF << (4*SWDIO_PIN_NUMBER))) \
                | (0x03 << (4*SWDIO_PIN_NUMBER)); \
  } while (0)
#else 
# define PIN_SWDIO_OUT_ENABLE(void) do { \
  SWDIO_PORT->CRH = (SWDIO_PORT->CRH & ~(0xF << (4*(SWDIO_PIN_NUMBER - 8)))) \
                | (0x03 << (4*(SWDIO_PIN_NUMBER - 8))); \
  } while (0)
#endif  /*   PIN_SWDIO_OUT_ENABLE() */
  
  
/* PIN_SWDIO_OUT_DISABLE() */  
/** SWDIO I/O pin: Switch to Input mode (used in SWD mode only).
Configure the SWDIO DAP hardware I/O pin to input mode. This function is
called prior \ref PIN_SWDIO_IN function calls.
*/
#if (SWDIO_PIN_NUMBER < 8)
# define PIN_SWDIO_OUT_DISABLE(void) do { \
  SWDIO_PORT->CRL = (SWDIO_PORT->CRL & ~(0xF << (4*SWDIO_PIN_NUMBER))) \
                | (0x08 << (4*SWDIO_PIN_NUMBER));                 \
  } while (0)
#else
# define PIN_SWDIO_OUT_DISABLE(void) do { \
  SWDIO_PORT->CRH = (SWDIO_PORT->CRH & ~(0xF << (4*(SWDIO_PIN_NUMBER - 8)))) \
                | (0x08 << (4*(SWDIO_PIN_NUMBER - 8)));   \
  } while (0)
#endif  /* PIN_SWDIO_OUT_DISABLE() */ 
  

// SWCLK/TCK I/O pin -------------------------------------  
/** SWCLK/TCK I/O pin: Get Input.
\return Current status of the SWCLK/TCK DAP hardware I/O pin.
*/
static inline __attribute__((always_inline)) uint32_t PIN_SWCLK_TCK_IN(void)
{
  return (SWCLK_PORT->ODR & SWCLK_PIN) ? 1 : 0;
}

/** SWCLK/TCK I/O pin: Set Output to High.
Set the SWCLK/TCK DAP hardware I/O pin to high level.
*/
static inline __attribute__((always_inline)) void     PIN_SWCLK_TCK_SET(void)
{
  SWCLK_PORT->BSRR = SWCLK_PIN;
}

/** SWCLK/TCK I/O pin: Set Output to Low.
Set the SWCLK/TCK DAP hardware I/O pin to low level.
*/
static inline __attribute__((always_inline)) void     PIN_SWCLK_TCK_CLR(void)
{
  SWCLK_PORT->BRR = SWCLK_PIN;
}

// SWDIO/TMS Pin I/O --------------------------------------

/** SWDIO/TMS I/O pin: Get Input.
\return Current status of the SWDIO/TMS DAP hardware I/O pin.
*/
static inline __attribute__((always_inline))  uint32_t PIN_SWDIO_TMS_IN(void)
{
  return (SWDIO_PORT->IDR & SWDIO_PIN) ? 1 : 0;
}

/** SWDIO/TMS I/O pin: Set Output to High.
Set the SWDIO/TMS DAP hardware I/O pin to high level.
*/
static inline __attribute__((always_inline))  void     PIN_SWDIO_TMS_SET(void)
{
  SWDIO_PORT->BSRR = SWDIO_PIN;
}

/** SWDIO/TMS I/O pin: Set Output to Low.
Set the SWDIO/TMS DAP hardware I/O pin to low level.
*/
static inline __attribute__((always_inline))   void     PIN_SWDIO_TMS_CLR(void)
{
  SWDIO_PORT->BRR = SWDIO_PIN;
}

/** SWDIO I/O pin: Get Input (used in SWD mode only).
\return Current status of the SWDIO DAP hardware I/O pin.
*/
static inline __attribute__((always_inline))   uint32_t PIN_SWDIO_IN(void)
{
  return (SWDIO_PORT->IDR & SWDIO_PIN) ? 1 : 0;
}

/** SWDIO I/O pin: Set Output (used in SWD mode only).
\param bit Output value for the SWDIO DAP hardware I/O pin.
*/
static inline __attribute__((always_inline))   void     PIN_SWDIO_OUT(uint32_t bit)
{
  if (bit & 0x1) {
    SWDIO_PORT->BSRR = SWDIO_PIN;
  } else {
    SWDIO_PORT->BRR = SWDIO_PIN;
  }
}



// nRESET Pin I/O------------------------------------------

/** nRESET I/O pin: Get Input.
\return Current status of the nRESET DAP hardware I/O pin.
*/
static inline __attribute__((always_inline))   uint32_t PIN_nRESET_IN(void)
{
  return (nRESET_PORT->IDR & nRESET_PIN) ? 1 : 0;
}

/** nRESET I/O pin: Set Output.
\param bit target device hardware reset pin status:
           - 0: issue a device hardware reset.
           - 1: release device hardware reset.
*/
static inline __attribute__((always_inline))   void     PIN_nRESET_OUT(uint32_t bit)
{
  if (bit) {
    PIN_nRESET_HIGH();
  } else {
    PIN_nRESET_LOW();
  }
}

#endif
