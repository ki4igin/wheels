/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DMA_H__
#define __DMA_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "stm32f4xx_ll_dma.h"

void MX_DMA_SPI2_Init(void *tx_buf, void *rx_buf, uint32_t size);
void MX_DMA_SPI3_Init(void *tx_buf, void *rx_buf, uint32_t size);

inline static void MX_DMA_SPI3_SetRxAddr(void *addr)
{
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_0, (uint32_t)addr);
}

inline static void MX_DMA_SPI3_SetSize(uint32_t size)
{
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_0, size);
}

inline static void MX_DMA_SPI3_Start(void)
{
    LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_0);
    LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_5);
}

#ifdef __cplusplus
}
#endif

#endif /* __DMA_H__ */
