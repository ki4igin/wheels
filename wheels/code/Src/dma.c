#include "dma.h"
#include "stm32f4xx.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_spi.h"

void MX_DMA_SPI2_Init(void *tx_buf, void *rx_buf, uint32_t size)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    LL_SPI_EnableDMAReq_RX(SPI2);
    LL_SPI_EnableDMAReq_TX(SPI2);

    // SPI RX DMA
    LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_3, LL_DMA_CHANNEL_0);
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
        LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_3));
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_3, size);

    // SPI TX DMA
    LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_4, LL_DMA_CHANNEL_0);
    LL_DMA_ConfigTransfer(
        DMA1,
        LL_DMA_STREAM_4,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_PRIORITY_HIGH
            | LL_DMA_MODE_NORMAL | LL_DMA_PERIPH_NOINCREMENT
            | LL_DMA_MEMORY_NOINCREMENT | LL_DMA_PDATAALIGN_BYTE
            | LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_ConfigAddresses(
        DMA1,
        LL_DMA_STREAM_4,
        (uint32_t)tx_buf,
        LL_SPI_DMA_GetRegAddr(SPI2),
        LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_4));
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_4, size);

    LL_DMA_ClearFlag_TC3(DMA1);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_3);
    LL_DMA_ClearFlag_HT3(DMA1);
    LL_DMA_EnableIT_HT(DMA1, LL_DMA_STREAM_3);
    LL_DMA_ClearFlag_TC4(DMA1);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_4);

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
}

void MX_DMA_SPI3_Init(void *tx_buf, void *rx_buf, uint32_t size)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    LL_SPI_EnableDMAReq_RX(SPI3);
    LL_SPI_EnableDMAReq_TX(SPI3);

    // SPI RX DMA
    LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_0, LL_DMA_CHANNEL_0);
    LL_DMA_ConfigTransfer(
        DMA1,
        LL_DMA_STREAM_0,
        LL_DMA_DIRECTION_PERIPH_TO_MEMORY | LL_DMA_PRIORITY_HIGH
            | LL_DMA_MODE_NORMAL | LL_DMA_PERIPH_NOINCREMENT
            | LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_BYTE
            | LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_ConfigAddresses(
        DMA1,
        LL_DMA_STREAM_0,
        LL_SPI_DMA_GetRegAddr(SPI3),
        (uint32_t)rx_buf,
        LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_0));
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_0, size);

    // SPI TX DMA
    LL_DMA_SetChannelSelection(DMA1, LL_DMA_STREAM_5, LL_DMA_CHANNEL_0);
    LL_DMA_ConfigTransfer(
        DMA1,
        LL_DMA_STREAM_5,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_PRIORITY_HIGH
            | LL_DMA_MODE_NORMAL | LL_DMA_PERIPH_NOINCREMENT
            | LL_DMA_MEMORY_NOINCREMENT | LL_DMA_PDATAALIGN_BYTE
            | LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_ConfigAddresses(
        DMA1,
        LL_DMA_STREAM_5,
        (uint32_t)tx_buf,
        LL_SPI_DMA_GetRegAddr(SPI3),
        LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_STREAM_5));
    LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_5, size);

    LL_DMA_ClearFlag_TC0(DMA1);
    LL_DMA_ClearFlag_TC5(DMA1);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_0);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_5);

    /* DMA interrupt init */
    /* DMA1_Stream3_IRQn interrupt configuration */
    NVIC_SetPriority(
        DMA1_Stream5_IRQn,
        NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    /* DMA1_Stream4_IRQn interrupt configuration */
    NVIC_SetPriority(
        DMA1_Stream0_IRQn,
        NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(DMA1_Stream0_IRQn);
}

