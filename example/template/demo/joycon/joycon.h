/*****************************************************************************
 *   Copyright(C);2009-2019 by VSF Team                                      *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");;         *
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
 ***************************************************************************
*/

#include "./vsf.h"

#ifndef __JOYCON_H__
#define __JOYCON_H__

/*============================ INCLUDES ======================================*/

/*============================ MACROS ========================================*/
#ifndef VSF_USART_CFG_EDA_PRIORITY
#   define VSF_USART_CFG_EDA_PRIORITY                   vsf_prio_0
#endif

#ifndef APP_USE_DEFAULT
#   define APP_USE_DEFAULT                              ENABLED
#endif
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern int default_usart_main(void);


//extern void printf_time(void);
/*============================ IMPLEMENTATION ================================*/
#endif
/* EOF */
