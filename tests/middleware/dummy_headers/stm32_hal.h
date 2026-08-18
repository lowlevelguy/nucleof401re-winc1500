#ifndef MOCK_STM32_HAL_H
#define MOCK_STM32_HAL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Status enum ---- */
typedef enum {
	HAL_OK       = 0,
	HAL_ERROR    = 1,
	HAL_BUSY     = 2,
	HAL_TIMEOUT  = 3
} HAL_StatusTypeDef;

/* ---- GPIO pin state ---- */
typedef enum {
	GPIO_PIN_RESET = 0,
	GPIO_PIN_SET   = 1
} GPIO_PinState;

/* ---- NVIC IRQ numbers ---- */
typedef enum {
	EXTI9_5_IRQn       = 23,
	DMA1_Stream3_IRQn  = 9,
	DMA1_Stream4_IRQn  = 10,
	SPI2_IRQn          = 35
} IRQn_Type;

/* ---- Peripheral register types (host dummies) ---- */
typedef struct { uint32_t MODER; uint32_t OTYPER; uint32_t OSPEEDR; uint32_t PUPDR; uint32_t IDR; uint32_t ODR; uint32_t BSRR; uint32_t LCKR; } GPIO_TypeDef;
typedef struct { uint32_t CR1; uint32_t CR2; uint32_t SR; uint32_t DR; } SPI_TypeDef;
typedef struct { uint32_t CR; uint32_t NDTR; uint32_t PAR; uint32_t M0AR; } DMA_Stream_TypeDef;
typedef struct { uint32_t LISR; uint32_t HISR; uint32_t LIFCR; uint32_t HIFCR; } DMA_TypeDef;

/* ---- SysTick ---- */
typedef struct {
	volatile uint32_t CTRL;
	volatile uint32_t LOAD;
	volatile uint32_t VAL;
	volatile uint32_t CALIB;
} SysTick_Type;

#define SysTick_CTRL_ENABLE_Msk    (0x1UL << 0)
#define SysTick_CTRL_TICKINT_Msk   (0x1UL << 1)

extern SysTick_Type *SysTick;

/* ---- Init structs ---- */
typedef struct {
	uint32_t Pin;
	uint32_t Mode;
	uint32_t Pull;
	uint32_t Speed;
} GPIO_InitTypeDef;

typedef struct {
	uint32_t Mode;
	uint32_t Direction;
	uint32_t DataSize;
	uint32_t CLKPolarity;
	uint32_t CLKPhase;
	uint32_t NSS;
	uint32_t BaudRatePrescaler;
	uint32_t FirstBit;
	uint32_t TIMode;
	uint32_t CRCCalculation;
	uint16_t CRCPolynomial;
} SPI_InitTypeDef;

typedef struct {
	uint32_t Direction;
	uint32_t PeriphInc;
	uint32_t MemInc;
	uint32_t Mode;
} DMA_InitTypeDef;

/* ---- Handles ---- */
typedef struct {
	DMA_Stream_TypeDef *Instance;
	DMA_InitTypeDef Init;
} DMA_HandleTypeDef;

typedef struct {
	SPI_TypeDef       *Instance;
	SPI_InitTypeDef   Init;
	DMA_HandleTypeDef *hdmatx;
	DMA_HandleTypeDef *hdmarx;
} SPI_HandleTypeDef;

/* ---- Peripheral base (macro, vendor style) ---- */
#define SPI2  ((SPI_TypeDef*)0x40003800UL)

/* ---- GPIO port globals ---- */
extern GPIO_TypeDef *GPIOA;
extern GPIO_TypeDef *GPIOB;
extern GPIO_TypeDef *GPIOC;
extern GPIO_TypeDef *GPIOD;
extern GPIO_TypeDef *GPIOE;
extern GPIO_TypeDef *GPIOH;

/* ---- GPIO pin numbers ---- */
#define GPIO_PIN_5   ((uint16_t)0x0020)
#define GPIO_PIN_6   ((uint16_t)0x0040)
#define GPIO_PIN_8   ((uint16_t)0x0100)
#define GPIO_PIN_12  ((uint16_t)0x1000)

/* ---- GPIO modes / pull / speed ---- */
#define GPIO_MODE_OUTPUT_PP   ((uint32_t)0x00000001)
#define GPIO_MODE_IT_FALLING  ((uint32_t)0x10010001)
#define GPIO_NOPULL           ((uint32_t)0x00000000)
#define GPIO_PULLUP           ((uint32_t)0x00000001)
#define GPIO_SPEED_FREQ_LOW   ((uint32_t)0x00000000)

/* ---- DMA constants ---- */
#define HAL_DMA_STATE_RESET    ((uint32_t)0x00)
#define DMA_MEMORY_TO_PERIPH   ((uint32_t)0x00000040)
#define DMA_PERIPH_TO_MEMORY   ((uint32_t)0x00000000)
#define DMA_PINC_DISABLE       ((uint32_t)0x00000000)
#define DMA_MINC_ENABLE        ((uint32_t)0x00000080)
#define DMA_NORMAL             ((uint32_t)0x00000000)

/* ---- SPI constants ---- */
#define SPI_MODE_MASTER            ((uint32_t)0x00000004)
#define SPI_DIRECTION_2LINES       ((uint32_t)0x00000000)
#define SPI_DATASIZE_8BIT          ((uint32_t)0x00000000)
#define SPI_POLARITY_LOW           ((uint32_t)0x00000000)
#define SPI_PHASE_1EDGE            ((uint32_t)0x00000000)
#define SPI_NSS_SOFT               ((uint32_t)0x00000200)
#define SPI_BAUDRATEPRESCALER_4    ((uint32_t)0x00000008)
#define SPI_FIRSTBIT_MSB           ((uint32_t)0x00000000)
#define SPI_TIMODE_DISABLE         ((uint32_t)0x00000000)
#define SPI_CRCCALCULATION_DISABLE ((uint32_t)0x00000000)

/* ---- RCC clock-enable macros (host no-ops) ---- */
#define __HAL_RCC_GPIOA_CLK_ENABLE()
#define __HAL_RCC_GPIOB_CLK_ENABLE()
#define __HAL_RCC_GPIOC_CLK_ENABLE()
#define __HAL_RCC_GPIOD_CLK_ENABLE()
#define __HAL_RCC_GPIOE_CLK_ENABLE()
#define __HAL_RCC_GPIOH_CLK_ENABLE()
#define __HAL_RCC_SPI2_CLK_ENABLE()
#define __HAL_RCC_DMA1_CLK_ENABLE()

/* ---- Link DMA to SPI handle ---- */
#define __HAL_LINKDMA(__HANDLE__, __FIELD__, __DMA__) \
	do { (__HANDLE__)->__FIELD__ = &(__DMA__); } while(0)

/* ---- HAL function declarations (mocked by CMock in a later phase) ---- */
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void HAL_GPIO_DeInit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

void HAL_Delay(uint32_t Delay);

void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority);
void HAL_NVIC_EnableIRQ(IRQn_Type IRQn);
void HAL_NVIC_DisableIRQ(IRQn_Type IRQn);

HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_DeInit(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Receive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Transmit_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_Receive_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_SPI_TransmitReceive_IT(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);

HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef *hdma);
uint32_t HAL_DMA_GetState(DMA_HandleTypeDef *hdma);

#ifdef __cplusplus
}
#endif

#endif /* MOCK_STM32_HAL_H */
