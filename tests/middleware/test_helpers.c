#include "conf_winc.h"

/* Definitions for the extern test globals referenced by the middleware
 * sources (via the dummy conf_winc.h / stm32_hal.h). Object-only compile. */

SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi_tx;
DMA_HandleTypeDef hdma_spi_rx;

GPIO_TypeDef *GPIOA = NULL;
GPIO_TypeDef *GPIOB = NULL;
GPIO_TypeDef *GPIOC = NULL;
GPIO_TypeDef *GPIOD = NULL;
GPIO_TypeDef *GPIOE = NULL;
GPIO_TypeDef *GPIOH = NULL;

static SysTick_Type sys_tick_storage;
SysTick_Type *SysTick = &sys_tick_storage;
