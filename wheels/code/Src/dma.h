/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DMA_H__
#define __DMA_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
/* Includes ------------------------------------------------------------------*/
void MX_DMA_SPI2_Init(void *tx_buf, void *rx_buf, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* __DMA_H__ */

