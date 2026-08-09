#include <string.h>

#include "conf_winc.h"

/* Public variables ----------------------------------------------------------*/
/**
 * @brief IRQN Pin ISR function pointer
 */
void (*volatile irqn_isr)(uint16_t) = NULL;

/**
 * @brief DMA SPI Handles
 */
DMA_HandleTypeDef hdma_spi_tx, hdma_spi_rx;

/**
 * @brief SPI event ISR function pointers
 */
void (*volatile spi_tx_isr)(SPI_HandleTypeDef*) = NULL;
void (*volatile spi_rx_isr)(SPI_HandleTypeDef*) = NULL;
void (*volatile spi_tx_rx_isr)(SPI_HandleTypeDef*) = NULL;
void (*volatile spi_error_isr)(SPI_HandleTypeDef*) = NULL;

/**
 * @brief SPI mutexes
 */
volatile uint8_t spi_bus_free = 1, spi_transfer_done = 1;


/* Public functions ----------------------------------------------------------*/
/**
 * @brief Initialises DMA for SPI TX operations.
*
 * @note According to the RM0368 reference manual, page 170, table 28, the DMA
 * peripheral to use for SPI2 TX is DMA1, Channel 0, Stream 4.
 *
 * @return HAL_OK on success, error code otherwise.
 */
HAL_StatusTypeDef spi_dma_tx_init(void) {
	hdma_spi_tx.Instance = DMA1_Stream4;

	// Zero out init struct to set unused fields
	memset(&hdma_spi_tx.Init, 0, sizeof(hdma_spi_tx.Init));

	hdma_spi_tx.Init.Channel = DMA_CHANNEL_0;
	hdma_spi_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
	hdma_spi_tx.Init.Mode = DMA_NORMAL;
	hdma_spi_tx.Init.Priority = DMA_PRIORITY_LOW;

	hdma_spi_tx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_spi_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;

	hdma_spi_tx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_spi_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;

	hdma_spi_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

	return HAL_DMA_Init(&hdma_spi_tx);
}

/**
 * @brief Initialises DMA for SPI RX operations.
 *
 * @note According to the RM0368 reference manual, page 170, table 28, the DMA
 * peripheral to use for SPI2 RX is DMA1, Channel 0, Stream 3.
 *
 * @return HAL_OK on success, error code otherwise.
 */
HAL_StatusTypeDef spi_dma_rx_init(void) {
	hdma_spi_rx.Instance = DMA1_Stream3;

	// Zero out init struct to set unused fields
	memset(&hdma_spi_rx.Init, 0, sizeof(hdma_spi_rx.Init));

	hdma_spi_rx.Init.Channel = DMA_CHANNEL_0;
	hdma_spi_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_spi_rx.Init.Mode = DMA_NORMAL;
	hdma_spi_rx.Init.Priority = DMA_PRIORITY_LOW;

	hdma_spi_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_spi_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;

	hdma_spi_rx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_spi_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;

	hdma_spi_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

	return HAL_DMA_Init(&hdma_spi_rx);
}

/**
 * @defgroup Overriding HAL __weak EXTI ISR implementation
 * @{
 */
void HAL_GPIO_EXTI_Callback(uint16_t gpio_pin) {
	if (irqn_isr != NULL) {
		irqn_isr(gpio_pin);
	}
}
/**
 * @}
 */

/**
 * @defgroup Overriding HAL __weak SPI ISR implementations
 * @{
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi) {
	if (spi_tx_isr != NULL) {
		spi_tx_isr(hspi);
	}
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi) {
	if (spi_rx_isr != NULL) {
		spi_rx_isr(hspi);
	}
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi) {
	if (spi_tx_rx_isr != NULL) {
		spi_tx_rx_isr(hspi);
	}
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef* hspi) {
	if (spi_error_isr != NULL) {
		spi_error_isr(hspi);
	}
}
/**
 * @}
 */
