#ifndef CONF_WINC_H
#define CONF_WINC_H

#ifdef __cplusplus
extern "C" {

#endif

#include <stdio.h>
#include <stdint.h>
#include "stm32_hal.h"
#include "mw_port.h"

/* ---- Test-double build guards ---- */
/* Under BUILD_TESTING the middleware sources expose their internal static
 * symbols (they do this themselves via STATIC); the -DSTATIC= define on the
 * test target covers the bus wrapper, whose #ifndef branch has no #else. */
#ifndef STM32F401xE
#define STM32F401xE
#endif

/* ---- Debug macros ---- */
#define CONF_WINC_DEBUG     1
#define CONF_WINC_PRINTF(...)  printf(__VA_ARGS__)

/* ---- BSP Config ---- */
#define CONF_WINC_SYSTEM_CLOCK_INIT()  mw_port_system_clock_config()

#define CONF_WINC_IRQN_PORT     GPIOC
#define CONF_WINC_IRQN_PIN      GPIO_PIN_5
#define CONF_WINC_CHIP_EN_PORT  GPIOC
#define CONF_WINC_CHIP_EN_PIN   GPIO_PIN_6
#define CONF_WINC_RESET_N_PORT  GPIOC
#define CONF_WINC_RESET_N_PIN   GPIO_PIN_8

#define CONF_WINC_USE_WAKE_PIN	0
#define CONF_WINC_WAKE_PORT     NULL
#define CONF_WINC_WAKE_PIN      0

#define CONF_WINC_IRQN_EXTI_LINE		EXTI9_5_IRQn
#define CONF_WINC_EXTI_REGISTER_ISR(x)	mw_port_exti_register_isr(x)
#define CONF_WINC_EXTI_DEREGISTER_ISR()	mw_port_exti_deregister_isr()

/* ---- Bus Wrapper Config ---- */
#define CONF_WINC_USE_SPI
#define CONF_WINC_SPI_USE_DMA

extern SPI_HandleTypeDef hspi2;

#define CONF_WINC_SPI_HANDLE     hspi2
#define CONF_WINC_SPI_INSTANCE   SPI2
#define CONF_WINC_SPI_IRQN       SPI2_IRQn
#define CONF_WINC_SPI_SS_PORT    GPIOB
#define CONF_WINC_SPI_SS_PIN     GPIO_PIN_12

#define CONF_WINC_SPI_BUS_ACQUIRE()  mw_port_bus_acquire()
#define CONF_WINC_SPI_BUS_RELEASE()  mw_port_bus_release()

#define CONF_WINC_SPI_SYNC_PREPARE()     mw_port_sync_prepare()
#define CONF_WINC_SPI_SYNC_WAIT()        mw_port_sync_wait()
#define CONF_WINC_SPI_SYNC_NOTIFY()      mw_port_sync_notify()
#define CONF_WINC_SPI_SYNC_NOTIFY_ERR()  mw_port_sync_notify_err()
#define CONF_WINC_SPI_SYNC_ERR_STATUS    mw_port_sync_err_status()

#define CONF_WINC_SPI_REGISTER_TX_ISR(x)     mw_port_spi_register_tx_isr(x)
#define CONF_WINC_SPI_REGISTER_RX_ISR(x)     mw_port_spi_register_rx_isr(x)
#define CONF_WINC_SPI_REGISTER_TX_RX_ISR(x)  mw_port_spi_register_tx_rx_isr(x)
#define CONF_WINC_SPI_REGISTER_ERROR_ISR(x)  mw_port_spi_register_error_isr(x)

#define CONF_WINC_SPI_DEREGISTER_TX_ISR()     mw_port_spi_deregister_tx_isr()
#define CONF_WINC_SPI_DEREGISTER_RX_ISR()     mw_port_spi_deregister_rx_isr()
#define CONF_WINC_SPI_DEREGISTER_TX_RX_ISR()  mw_port_spi_deregister_tx_rx_isr()
#define CONF_WINC_SPI_DEREGISTER_ERROR_ISR()  mw_port_spi_deregister_error_isr()

extern DMA_HandleTypeDef hdma_spi_tx, hdma_spi_rx;

#define CONF_WINC_SPI_DMA_TX_HANDLE  hdma_spi_tx
#define CONF_WINC_SPI_DMA_TX_INIT()  mw_port_dma_tx_init()
#define CONF_WINC_SPI_DMA_TX_IRQN    DMA1_Stream4_IRQn

#define CONF_WINC_SPI_DMA_RX_HANDLE  hdma_spi_rx
#define CONF_WINC_SPI_DMA_RX_INIT()  mw_port_dma_rx_init()
#define CONF_WINC_SPI_DMA_RX_IRQN    DMA1_Stream3_IRQn

#ifdef __cplusplus
}
#endif

#endif /* CONF_WINC_H */
