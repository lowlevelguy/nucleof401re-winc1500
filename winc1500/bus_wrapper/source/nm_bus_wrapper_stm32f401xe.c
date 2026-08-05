#include "nm_common.h"
#include "nm_bsp.h"
#include "nm_bus_wrapper.h"
#include "conf_winc.h"


#if !defined(CONF_WINC_USE_SPI) && !defined(CONF_WINC_USE_UART) && \
	!defined(CONF_WINC_USE_I2C)
#error "Please define a bus to use from the following: SPI, UART, I2C."
#endif

#if (defined(CONF_WINC_USE_UART) || defined(CONF_WINC_USE_I2C))
#error "Non-SPI buses are currently unsupported."
#endif


/* Types ---------------------------------------------------------------------*/
typedef enum {
	NM_BUS_WRAPPER_STATUS_OK = M2M_SUCCESS,
	NM_BUS_WRAPPER_STATUS_SPI_INIT_FAILED = -1,
	NM_BUS_WRAPPER_STATUS_IOCTL_INVALID_OPCODE = M2M_ERR_BUS_FAIL,
	NM_BUS_WRAPPER_STATUS_SPI_RW_INVALID_PARAMS = M2M_ERR_INVALID_ARG,
} NmBusWrapperStatus_e;


/* Private functions ---------------------------------------------------------*/
#ifdef CONF_WINC_USE_SPI
/**
 * @brief Drives the Slave Select pin low.
 */
void spi_assert_ss(void) {
	HAL_GPIO_WritePin(CONF_WINC_SPI_SS_PORT, CONF_WINC_SPI_SS_PIN,
		GPIO_PIN_RESET);
}

/**
 * @brief Drives the Slave Select pin high.
 */
void spi_deassert_ss(void) {
	HAL_GPIO_WritePin(CONF_WINC_SPI_SS_PORT, CONF_WINC_SPI_SS_PIN,
		GPIO_PIN_SET);
}
#endif


/* Public functions ----------------------------------------------------------*/
/**
 * @note Only supports SPI at the moment.
 *
 * @note Initialisiation is such that the Slave Select pin is deasserted by
 * default.
 */
sint8 nm_bus_init(void* config) {
	UNUSED(config);

#ifdef CONF_WINC_USE_SPI
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	/* ---- Configure SPI interface, if it isn't already ---- */
	if (HAL_SPI_GetState(&CONF_WINC_SPI_HANDLE) == HAL_SPI_STATE_RESET) {
		CONF_WINC_SPI_HANDLE.Instance = SPI2;
		CONF_WINC_SPI_HANDLE.Init.Mode = SPI_MODE_MASTER;
		CONF_WINC_SPI_HANDLE.Init.Direction = SPI_DIRECTION_2LINES;
		CONF_WINC_SPI_HANDLE.Init.DataSize = SPI_DATASIZE_8BIT;
		CONF_WINC_SPI_HANDLE.Init.CLKPolarity = SPI_POLARITY_LOW;
		CONF_WINC_SPI_HANDLE.Init.CLKPhase = SPI_PHASE_1EDGE;
		CONF_WINC_SPI_HANDLE.Init.NSS = SPI_NSS_SOFT;
		CONF_WINC_SPI_HANDLE.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
		CONF_WINC_SPI_HANDLE.Init.FirstBit = SPI_FIRSTBIT_MSB;
		CONF_WINC_SPI_HANDLE.Init.TIMode = SPI_TIMODE_DISABLE;
		CONF_WINC_SPI_HANDLE.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
		CONF_WINC_SPI_HANDLE.Init.CRCPolynomial = 10;

		if (HAL_SPI_Init(&CONF_WINC_SPI_HANDLE) != HAL_OK) {
			return NM_BUS_WRAPPER_STATUS_SPI_INIT_FAILED;
		}
	}

	/* ---- Configure Slave Select ---- */
	GPIO_InitTypeDef gpio_init;
	gpio_init.Pin = CONF_WINC_SPI_SS_PIN;
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(CONF_WINC_SPI_SS_PORT, &gpio_init);

	spi_deassert_ss();

	// Power module on
	nm_bsp_reset();

#endif

	return NM_BUS_WRAPPER_STATUS_OK;
}

/**
 * @note Only supports SPI at the moment.
 */
sint8 nm_bus_deinit(void) {
#ifdef CONF_WINC_USE_SPI

	HAL_GPIO_DeInit(CONF_WINC_SPI_SS_PORT, CONF_WINC_SPI_SS_PIN);

#endif

	return NM_BUS_WRAPPER_STATUS_OK;
}

sint8 nm_bus_ioctl(uint8 opcode, void* params) {
#ifdef CONF_WINC_USE_SPI
	tstrNmSpiRw* spi_rw_params = (tstrNmSpiRw*)params;
#endif

	switch (opcode) {
#ifdef CONF_WINC_USE_SPI
	case NM_BUS_IOCTL_RW:
		nm_spi_rw(spi_rw_params->pu8InBuf, spi_rw_params->pu8OutBuf,
			spi_rw_params->u16Sz);
		break;
#endif

	default:
		return NM_BUS_WRAPPER_STATUS_IOCTL_INVALID_OPCODE;
	}

	return NM_BUS_WRAPPER_STATUS_OK;
}

/**
 * @note Stub implementation; the module's current driver makes no reference to
 * it.
 */
sint8 nm_bus_reinit(void* config) {
	UNUSED(config);
	return NM_BUS_WRAPPER_STATUS_OK;
}

#ifdef CONF_WINC_USE_SPI
sint8 nm_spi_rw(uint8* tx_buf, uint8* rx_buf, uint16 buf_size) {
	// At least one of the provided buffers should be non-NULL
	if ((tx_buf == NULL && rx_buf == NULL) || buf_size == 0) {
		return NM_BUS_WRAPPER_STATUS_SPI_RW_INVALID_PARAMS;
	}

	// Claim SPI bus
	CONF_WINC_SPI_BUS_ACQUIRE();
	spi_assert_ss();

	CONF_WINC_SPI_SYNC_PREPARE();

	/* Fire SPI transfer over DMA.
	 * We ignore HAL return values, as a busy SPI interface would be the result
	 * of either hardware issues, or misuse of the bus sync interface by the
	 * user. */
	if (tx_buf == NULL) {
		(void)HAL_SPI_Receive_DMA(&CONF_WINC_SPI_HANDLE, rx_buf, buf_size);
	} else if (rx_buf == NULL) {
		(void)HAL_SPI_Transmit_DMA(&CONF_WINC_SPI_HANDLE, tx_buf, buf_size);
	} else {
		(void)HAL_SPI_TransmitReceive_DMA(&CONF_WINC_SPI_HANDLE,
			tx_buf, rx_buf, buf_size);
	}

	// Wait until done
	CONF_WINC_SPI_SYNC_WAIT();

	// Free SPI bus
	spi_deassert_ss();
	CONF_WINC_SPI_BUS_RELEASE();

	return NM_BUS_WRAPPER_STATUS_OK;
}

/**
 * @defgroup Override HAL's default __weak implementations.
 * @{
 */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
	if (hspi->Instance == CONF_WINC_SPI_HANDLE.Instance) {
		CONF_WINC_SPI_SYNC_NOTIFY();
	}
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef* hspi) {
	if (hspi->Instance == CONF_WINC_SPI_HANDLE.Instance) {
		CONF_WINC_SPI_SYNC_NOTIFY();
	}
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi) {
	if (hspi->Instance == CONF_WINC_SPI_HANDLE.Instance) {
		CONF_WINC_SPI_SYNC_NOTIFY();
	}
}
/**
 * @}
 */
#endif