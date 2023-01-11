#include "dma.h"
#include "stm32f4xx.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_spi.h"

#define BUF_SIZE (3 * 8)
uint8_t rx_buf[BUF_SIZE] = {0};
uint8_t tx_buf[BUF_SIZE] = {0};

void MX_DMA_SPI2_Init(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    /* DMA interrupt init */
    /* DMA1_Stream3_IRQn interrupt configuration */
    NVIC_SetPriority(
        DMA1_Stream3_IRQn,
        NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(DMA1_Stream3_IRQn);
    /* DMA1_Stream4_IRQn interrupt configuration */
    NVIC_SetPriority(
        DMA1_Stream4_IRQn,
        NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(DMA1_Stream4_IRQn);

    // SPI RX DMA
    LL_DMA_ConfigTransfer(
        DMA1,
        LL_DMA_STREAM_3,
        LL_DMA_DIRECTION_PERIPH_TO_MEMORY | LL_DMA_PRIORITY_HIGH
            | LL_DMA_MODE_NORMAL | LL_DMA_PERIPH_NOINCREMENT
            | LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_BYTE
            | LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_ConfigAddresses(
        DMA1,
        LL_DMA_STREAM_3,
        LL_SPI_DMA_GetRegAddr(SPI2),
        (uint32_t)rx_buf,
        LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_3, BUF_SIZE);

    // SPI TX DMA
    LL_DMA_ConfigTransfer(
        DMA1,
        LL_DMA_STREAM_4,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_PRIORITY_HIGH
            | LL_DMA_MODE_NORMAL | LL_DMA_PERIPH_NOINCREMENT
            | LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_BYTE
            | LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_ConfigAddresses(
        DMA1,
        LL_DMA_STREAM_4,
        LL_SPI_DMA_GetRegAddr(SPI2),
        (uint32_t)rx_buf,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_4, BUF_SIZE);

    LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_3);
}
