
#ifndef __GPIO_EX_H__
#define __GPIO_EX_H__

#include "main.h"

inline static void test_pin14_enable(void)
{
    LL_GPIO_SetOutputPin(PC14_GPIO_Port, PC14_Pin);
}

inline static void test_pin14_disable(void)
{
    LL_GPIO_ResetOutputPin(PC14_GPIO_Port, PC14_Pin);
}

inline static void test_pin15_enable(void)
{
    LL_GPIO_SetOutputPin(PC15_GPIO_Port, PC15_Pin);
}

inline static void test_pin15_disable(void)
{
    LL_GPIO_ResetOutputPin(PC15_GPIO_Port, PC15_Pin);
}

inline static void test_pin14_toggle(void)
{
    LL_GPIO_TogglePin(PC14_GPIO_Port, PC14_Pin);
}

inline static void test_pin15_toggle(void)
{
    LL_GPIO_TogglePin(PC15_GPIO_Port, PC15_Pin);
}

#endif
