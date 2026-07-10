
#ifndef DWT_H_
#define DWT_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/* init cycle counter */
/* DWT (Data Watchpoint and Trace) registers, only exists on ARM Cortex with a DWT unit */
/*!< DEMCR: Debug Exception and Monitor Control Register */
/*!< TRCENA: Enable trace and debug block DEMCR (Debug Exception and Monitor Control Register) */
/*!< DWT Cycle Counter register */
/*!< CYCCNTENA bit in DWT_CONTROL register */
static inline void cycle_counter_init(void) __attribute__((always_inline));
static inline void cycle_counter_init(void)
{
	 CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;/* enable DWT hardware */
	 DWT->CYCCNT = 0;								/* reset cycle counter */
	 DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;			/* start counting */
}

/* reset cycle counter */
/*!< DWT Cycle Counter register */
static inline void cycle_counter_reset(void) __attribute__((always_inline));
static inline void cycle_counter_reset(void)
{
	DWT->CYCCNT = 0;
}

/* enable counting */
/*!< CYCCNTENA bit in DWT_CONTROL register */
static inline void cycle_counter_enable(void) __attribute__((always_inline));
static inline void cycle_counter_enable(void)
{
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/* disable counting */
/*!< CYCCNTENA bit in DWT_CONTROL register */
static inline void cycle_counter_disable(void) __attribute__((always_inline));
static inline void cycle_counter_disable(void)
{
	DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
}

/* read cycle counter */
/*!< DWT Cycle Counter register */
static inline uint32_t cycle_counter_get(void) __attribute__((always_inline));
static inline uint32_t cycle_counter_get(void)
{
	return (DWT->CYCCNT);
}

static inline uint32_t cycle_counter_get_time_us(void) __attribute__((always_inline));
static inline uint32_t cycle_counter_get_time_us(void)
{
	return (DWT->CYCCNT / (SystemCoreClock / 1000000));
}

/*  uint32_t cycle_counter = 0;
 *  uint32_t cycle_counter_time_us = 0;
 *															// PC8 (GPIO)
 *  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);	// => ______
 *  cycle_counter_init();
 *
 *  ...
 *  ...														// => ______
 *															//		 ___
 *  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); 	// => __/
 *  // or => HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
 *  cycle_counter_reset();
 *															//	  ______
 *  ...														// =>
 *
 *  cycle_counter = cycle_counter_get();
 *  cycle_counter_time_us = cycle_counter_get_time_us();	//	  __
 *  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);	// =>   \___
 *  // or => HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
 *
 *  														// => ______
 *
 *  LOGGER_LOG("Cycles: %lu - Time %lu uS\r\n", cycle_counter, cycle_counter_time_us);
 */

/********************** typedef **********************************************/

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* DWT_H_ */

/********************** end of file ******************************************/
