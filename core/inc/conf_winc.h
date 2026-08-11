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
 * @defgroup BSP Config.
 * @{
 */
/**
 * @brief GPIO mapping
 *
 * @note The WAKE pin is currently unused. However, the macros are there as
 * recommended by the datasheet (section 2.2, note 2) for possible future use.
 */
#define CONF_WINC_IRQN_PORT			GPIOC
#define CONF_WINC_IRQN_PIN			GPIO_PIN_5
#define CONF_WINC_CHIP_EN_PORT		GPIOC
#define CONF_WINC_CHIP_EN_PIN		GPIO_PIN_6
#define CONF_WINC_RESET_N_PORT		GPIOC
#define CONF_WINC_RESET_N_PIN		GPIO_PIN_8

#define CONF_WINC_USE_WAKE_PIN		0
#define CONF_WINC_WAKE_PORT			NULL
#define CONF_WINC_WAKE_PIN			0

/**
 * @brief EXTI Config For IRQN ISR
 */
extern void (*volatile irqn_isr)(uint16_t);
#define CONF_WINC_IRQN_EXTI_LINE		EXTI9_5_IRQn
#define CONF_WINC_EXTI_REGISTER_ISR(x)	do { irqn_isr = (x); } while(0)
#define CONF_WINC_EXTI_DEREGISTER_ISR()	do { irqn_isr = NULL; } while(0)
/**
 * @}
 */

/**
 * @defgroup Bus Wrapper Config.
 * @{
 */
#define CONF_WINC_USE_SPI
#define CONF_WINC_SPI_USE_DMA

#ifdef CONF_WINC_USE_SPI
/**
 * @brief SPI interface and Slave Select pin mapping
 */
extern SPI_HandleTypeDef hspi2;
#define CONF_WINC_SPI_HANDLE		hspi2
#define CONF_WINC_SPI_INSTANCE		SPI2
#define CONF_WINC_SPI_IRQN			SPI2_IRQn
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
extern volatile uint8_t spi_transfer_done, spi_transfer_error;
#define CONF_WINC_SPI_SYNC_PREPARE()	do { \
		spi_transfer_done = 0; \
		spi_transfer_error = 0; \
	} while(0)
#define CONF_WINC_SPI_SYNC_WAIT()		do { \
		while(spi_transfer_done != 1) { \
			HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); \
		} \
	} while(0)
#define CONF_WINC_SPI_SYNC_NOTIFY()		do { spi_transfer_done = 1; } while(0)
#define CONF_WINC_SPI_SYNC_NOTIFY_ERR() do { \
		spi_transfer_done = 1; \
		spi_transfer_error = 1; \
	} while(0)
#define CONF_WINC_SPI_SYNC_ERR_STATUS	(spi_transfer_error)

/**
 * @brief SPI ISR registering and de-registering
 */
extern void (*volatile spi_tx_isr)(SPI_HandleTypeDef*);
extern void (*volatile spi_rx_isr)(SPI_HandleTypeDef*);
extern void (*volatile spi_tx_rx_isr)(SPI_HandleTypeDef*);
extern void (*volatile spi_error_isr)(SPI_HandleTypeDef*);

#define CONF_WINC_SPI_REGISTER_TX_ISR(x)	do { spi_tx_isr = (x); } while(0)
#define CONF_WINC_SPI_REGISTER_RX_ISR(x)	do { spi_rx_isr = (x); } while(0)
#define CONF_WINC_SPI_REGISTER_TX_RX_ISR(x)	do { spi_tx_rx_isr = (x); } while(0)
#define CONF_WINC_SPI_REGISTER_ERROR_ISR(x)	do { spi_error_isr = (x); } while(0)

#define CONF_WINC_SPI_DEREGISTER_TX_ISR()		\
	do { spi_tx_isr = NULL; } while(0)
#define CONF_WINC_SPI_DEREGISTER_RX_ISR()		\
	do { spi_rx_isr = NULL; } while(0)
#define CONF_WINC_SPI_DEREGISTER_TX_RX_ISR()	\
	do { spi_tx_rx_isr = NULL; } while(0)
#define CONF_WINC_SPI_DEREGISTER_ERROR_ISR()	\
	do { spi_error_isr = NULL; } while(0)


#ifdef CONF_WINC_SPI_USE_DMA
extern DMA_HandleTypeDef hdma_spi_tx, hdma_spi_rx;

/**
 * @brief DMA mapping
 *
 * @note From RM0368's reference manual (table 28)
 */
HAL_StatusTypeDef spi_dma_tx_init(void);
#define CONF_WINC_SPI_DMA_TX_HANDLE		hdma_spi_tx
#define CONF_WINC_SPI_DMA_TX_INIT()		spi_dma_tx_init()
#define CONF_WINC_SPI_DMA_TX_IRQN		DMA1_Stream4_IRQn

HAL_StatusTypeDef spi_dma_rx_init(void);
#define CONF_WINC_SPI_DMA_RX_HANDLE		hdma_spi_rx
#define CONF_WINC_SPI_DMA_RX_INIT()		spi_dma_rx_init()
#define CONF_WINC_SPI_DMA_RX_IRQN		DMA1_Stream3_IRQn

#endif /* CONF_WINC_SPI_USE_DMA */

#endif /* CONF_WINC_USE_SPI */

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
