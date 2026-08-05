#include "nm_bsp.h"
#include "nm_common.h"
#include "conf_winc.h"

/**
 * @brief Macro enabling the export of static object to unit tests.
 */
#ifndef BUILD_TESTING
#define STATIC static
#else /* defined(BUILD_TESTING) */
#define	STATIC
#endif

extern void SystemClock_Config(void);


/* Private variables ---------------------------------------------------------*/
STATIC volatile tpfNmBspIsr module_irqn_pin_isr = NULL;


/* Private functions ---------------------------------------------------------*/
/**
 * @brief Initialises module control pins: CHIP_EN, RESET_N, IRQN and, if
 * configured, WAKE.
 *
 * @note The pins are initialised such that the module is powered off by default
 * until the user explicitly turns it on.
 */
STATIC void module_ctrl_pins_init(void) {
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	GPIO_InitTypeDef gpio_init = {0};

	/* Configure CHIP_EN, RESET_N and WAKE (if configured) pins.
	 * - CHIP_EN, WAKE: active-high
	 * - RESET_N: active-low */
#if (CONF_WINC_USE_WAKE_PIN == 1)
	gpio_init.Pin =
		CONF_WINC_CHIP_EN_PIN | CONF_WINC_WAKE_PIN | CONF_WINC_RESET_N_PIN;
#else
	gpio_init.Pin = CONF_WINC_CHIP_EN_PIN | CONF_WINC_RESET_N_PIN;
#endif
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(CONF_WINC_CHIP_EN_PORT, &gpio_init);

	/* Configure IRQN pin: active-low.
	 * Note: we use a pull-up to force the line to high on module power off. */
	gpio_init.Pin = CONF_WINC_IRQN_PIN;
	gpio_init.Mode = GPIO_MODE_IT_FALLING;
	gpio_init.Pull = GPIO_PULLUP;
	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(CONF_WINC_IRQN_PORT, &gpio_init);

	nm_bsp_interrupt_ctrl(1);

	/* Pin init values (module off):
	 * - RESET_N: high
	 * - CHIP_EN: low
	 * - WAKE: low (if configured) */
	HAL_GPIO_WritePin(CONF_WINC_RESET_N_PORT, CONF_WINC_RESET_N_PIN,
		GPIO_PIN_SET);
	HAL_GPIO_WritePin(CONF_WINC_CHIP_EN_PORT, CONF_WINC_CHIP_EN_PIN,
		GPIO_PIN_RESET);
#if (CONF_WINC_USE_WAKE_PIN == 1)
	HAL_GPIO_WritePin(CONF_WINC_WAKE_PORT, CONF_WINC_WAKE_PIN, GPIO_PIN_RESET);
#endif
}


/* Public functions ----------------------------------------------------------*/
/**
 * @note Initialisation is such that the module is powered off by default, until
 * the user explicitly turns it on via reset.
 */
sint8 nm_bsp_init(void) {
	module_irqn_pin_isr = NULL;

	// Ensure the system clock is configured
	if ((SysTick->CTRL & SysTick_CTRL_ENABLE_Msk) == 0 ||
		(SysTick->CTRL & SysTick_CTRL_TICKINT_Msk) == 0) {
		SystemClock_Config();
	}

	// Initialise the module control pins
	module_ctrl_pins_init();

	return M2M_SUCCESS;
}

sint8 nm_bsp_deinit(void) {
	nm_bsp_interrupt_ctrl(0);

	HAL_GPIO_DeInit(CONF_WINC_IRQN_PORT, CONF_WINC_IRQN_PIN);
	HAL_GPIO_DeInit(CONF_WINC_CHIP_EN_PORT, CONF_WINC_CHIP_EN_PIN);
	HAL_GPIO_DeInit(CONF_WINC_RESET_N_PORT, CONF_WINC_RESET_N_PIN);
#if (CONF_WINC_USE_WAKE_PIN == 1)
	HAL_GPIO_DeInit(CONF_WINC_WAKE_PORT, CONF_WINC_WAKE_PIN);
#endif

	return M2M_SUCCESS;
}

/**
 * @note The reset procedure is described in the datasheet's sections 7.4 - 7.6.
 *
 * @note The implemented reset procedure is leveraged purely via by the CHIP_EN
 * and RESET_N pins; at no point is the module's power supply voltage cut off.
 */
void nm_bsp_reset(void) {
	/* ---- Power off module ---- */
	/* Pin values:
	 * - RESET_N: low
	 * - CHIP_EN: low
	 * - WAKE: low (if configured) */
	HAL_GPIO_WritePin(CONF_WINC_RESET_N_PORT, CONF_WINC_RESET_N_PIN,
		GPIO_PIN_RESET);
	HAL_GPIO_WritePin(CONF_WINC_CHIP_EN_PORT, CONF_WINC_CHIP_EN_PIN,
		GPIO_PIN_RESET);

	/* Since the module's current firmware doesn't make use of the WAKE pin,
	 * it's unclear whether it should be set before or after the 10ms sleep. */
#if (CONF_WINC_USE_WAKE_PIN == 1)
	HAL_GPIO_WritePin(CONF_WINC_WAKE_PORT, CONF_WINC_WAKE_PIN, GPIO_PIN_RESET);
#endif

	/* The datasheet (section 7.6) specifies a minimum time of 1us for RESET_N
	 * pulsing low. */
	nm_bsp_sleep(1);

	/* ---- Power on module ---- */
	/* Pin values:
	 * - CHIP_EN: high
	 * - RESET_N: high
	 * - WAKE: high (if configured) */
	HAL_GPIO_WritePin(CONF_WINC_CHIP_EN_PORT, CONF_WINC_CHIP_EN_PIN,
		GPIO_PIN_SET);

	/* The datasheet (table 7-2) specifies a minimum time of 5ms in-between
	 * setting CHIP_EN and RESET_N. We use double that as the vendor BSPs do
	 * for safety purposes. */
	nm_bsp_sleep(10);

	HAL_GPIO_WritePin(CONF_WINC_RESET_N_PORT, CONF_WINC_RESET_N_PIN,
		GPIO_PIN_SET);

	/* Since the module's current firmware doesn't make use of the WAKE pin,
	 * it's unclear whether it should be set before or after the 10ms sleep. */
#if (CONF_WINC_USE_WAKE_PIN == 1)
	HAL_GPIO_WritePin(CONF_WINC_WAKE_PORT, CONF_WINC_WAKE_PIN, GPIO_PIN_SET);
#endif

}

void nm_bsp_sleep(uint32 ms) {
	HAL_Delay(ms);
}

/**
 * @note Should never be called by an ISR.
 */
void nm_bsp_register_isr(tpfNmBspIsr cb) {
	/* Even though the variable is used by concurrent contexts, a 32-bit write
	 * is a single ldr instruction on Cortex-M4; atomicity is guaranteed by
	 * the platform. */
	module_irqn_pin_isr = cb;
}

void nm_bsp_interrupt_ctrl(uint8 state) {
	if (state == 1u) {
		HAL_NVIC_SetPriority(CONF_WINC_IRQN_EXTI_LINE, 0, 0);
		HAL_NVIC_EnableIRQ(CONF_WINC_IRQN_EXTI_LINE);
	} else if (state == 0u) {
		HAL_NVIC_DisableIRQ(CONF_WINC_IRQN_EXTI_LINE);
	}
}

/**
 * @brief Overrides HAL's default __weak implementation.
 */
void HAL_GPIO_EXTI_Callback(uint16_t gpio_pin) {
	if (gpio_pin == CONF_WINC_IRQN_PIN) {
		/* As nm_bsp_register_isr is never called by an ISR, no preemption can
		 * cause a time-of-check time-of-use race. */
		if (module_irqn_pin_isr != NULL) {
			module_irqn_pin_isr();
		}
	}
}