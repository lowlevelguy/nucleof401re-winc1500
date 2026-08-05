#ifndef CONF_WINC_H
#define CONF_WINC_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include "stm32f401xe.h"
#include "stm32f4xx_hal.h"
#include "compiler.h"


/**
 * @brief Debug Macros
 */
#define CONF_WINC_DEBUG				1
#define CONF_WINC_PRINTF(...)		printf(__VA_ARGS__)

/**
 * @defgroup GPIO mapping for BSP
 * @{
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
/**
 * @}
 */

/**
 * @defgroup Bus Wrapper Config.
 * @{
 */
#define CONF_WINC_USE_SPI
#ifdef CONF_WINC_USE_SPI
extern SPI_HandleTypeDef hspi2;

/**
 * @brief SPI interface and Slave Select pin mapping
 */
#define CONF_WINC_SPI_HANDLE		hspi2
#define CONF_WINC_SPI_SS_PORT		GPIOB
#define CONF_WINC_SPI_SS_PIN		GPIO_PIN_12

/**
 * @brief SPI bus sync interface
 */
extern volatile uint8_t spi_bus_free;
#define CONF_WINC_SPI_BUS_ACQUIRE()		do { \
		while (spi_bus_free != 1) { \
			HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); \
		} \
		spi_bus_free = 0; \
	} while (0)
#define CONF_WINC_SPI_BUS_RELEASE()		do { spi_bus_free = 1; } while(0)

/**
 * @brief SPI operations sync interface
 */
extern volatile uint8_t spi_transfer_done;
#define CONF_WINC_SPI_SYNC_PREPARE()	do { spi_transfer_done = 0; } while(0)
#define CONF_WINC_SPI_SYNC_WAIT()		do { \
		while(spi_transfer_done != 1) { \
			HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); \
		} \
	} while(0)
#define CONF_WINC_SPI_SYNC_NOTIFY()		do { spi_transfer_done = 1; } while(0)
#endif

#ifdef CONF_WINC_USE_I2C
#undef CONF_WINC_USE_I2C
#endif

#ifdef CONF_WINC_USE_UART
#undef CONF_WINC_USE_UART
#endif
/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif /* CONF_WINC_H */
