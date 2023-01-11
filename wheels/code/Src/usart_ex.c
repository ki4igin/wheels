#include "usart_ex.h"

void uart_send_str(USART_TypeDef *USARTx, uint8_t *str)
{
    while (*str != 0) {
        while ((USARTx->SR & USART_SR_TXE) != USART_SR_TXE) {
            /* code */
        }

        USARTx->DR = *str++;
    }
}

void uart_send_array(USART_TypeDef *USARTx, uint8_t *pbuf, uint32_t len)
{
    do {
        while ((USARTx->SR & USART_SR_TXE) != USART_SR_TXE) {
            /* code */
        }

        USARTx->DR = *pbuf++;
    } while (--len > 0);
}
