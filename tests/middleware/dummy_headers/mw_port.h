#ifndef MW_PORT_H
#define MW_PORT_H

#include <stdint.h>
#include "stm32_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Sync interface ---- */
void mw_port_sync_prepare(void);
void mw_port_sync_wait(void);
void mw_port_sync_notify(void);
void mw_port_sync_notify_err(void);
uint8_t mw_port_sync_err_status(void);

/* ---- Bus acquire / release ---- */
void mw_port_bus_acquire(void);
void mw_port_bus_release(void);

/* ---- WINC EXTI ISR registration ---- */
void mw_port_exti_register_isr(void (*isr)(uint16_t));
void mw_port_exti_deregister_isr(void);

/* ---- SPI ISR registration / deregistration ---- */
void mw_port_spi_register_tx_isr(void (*isr)(SPI_HandleTypeDef *));
void mw_port_spi_register_rx_isr(void (*isr)(SPI_HandleTypeDef *));
void mw_port_spi_register_tx_rx_isr(void (*isr)(SPI_HandleTypeDef *));
void mw_port_spi_register_error_isr(void (*isr)(SPI_HandleTypeDef *));
void mw_port_spi_deregister_tx_isr(void);
void mw_port_spi_deregister_rx_isr(void);
void mw_port_spi_deregister_tx_rx_isr(void);
void mw_port_spi_deregister_error_isr(void);

/* ---- DMA init ---- */
HAL_StatusTypeDef mw_port_dma_tx_init(void);
HAL_StatusTypeDef mw_port_dma_rx_init(void);

/* ---- System clock ---- */
void mw_port_system_clock_config(void);

#ifdef __cplusplus
}
#endif

#endif /* MW_PORT_H */
