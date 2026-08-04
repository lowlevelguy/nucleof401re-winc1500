#ifndef CONF_WINC_H
#define CONF_WINC_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include "stm32f401xe.h"
#include "stm32f4xx_hal.h"

/**
 * @brief Debug Macros
 */
#define CONF_WINC_DEBUG				1
#define CONF_WINC_PRINTF(...)		printf(__VA_ARGS__)

/**
 * @brief GPIO mapping
 *
 * @note The WAKE pin is currently unused. However, the macros are there as
 * recommended by the datasheet (section 2.2, note 2) for possible future use.
 */
#define CONF_WINC_IRQN_PORT			GPIOC
#define CONF_WINC_IRQN_PIN			GPIO_PIN_5
#define CONF_WINC_IRQN_EXTI_LINE	EXTI9_5_IRQn
#define CONF_WINC_CHIP_EN_PORT		GPIOC
#define CONF_WINC_CHIP_EN_PIN		GPIO_PIN_6
#define CONF_WINC_RESET_N_PORT		GPIOC
#define CONF_WINC_RESET_N_PIN		GPIO_PIN_8

#define CONF_WINC_USE_WAKE_PIN		0
#define CONF_WINC_WAKE_PORT			NULL
#define CONF_WINC_WAKE_PIN			0


#ifdef __cplusplus
}
#endif

#endif /* CONF_WINC_H */
