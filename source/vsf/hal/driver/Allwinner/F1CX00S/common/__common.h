/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __HAL_DRIVER_ALLWINNER_F1CX00S_COMMON_H__
#define __HAL_DRIVER_ALLWINNER_F1CX00S_COMMON_H__

/*============================ INCLUDES ======================================*/
#include "hal/vsf_hal_cfg.h"
#include "hal/arch/vsf_arch.h"

/*============================ MACROS ========================================*/

// fixed
#define F1CX00S_PLL_PERIPH_HZ           (600UL * 1000 * 1000)
#define F1CX00S_PLL_AHB                 (200UL * 1000 * 1000)
#define F1CX00S_PLL_APB                 (100UL * 1000 * 1000)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern void __delay_loops(int loops);
extern void f1cx00s_clock_init(uint_fast32_t pll_cpu);

#endif
/* EOF */
