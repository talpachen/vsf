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

#ifndef __VSF_SDL2_STDINC_H__
#define __VSF_SDL2_STDINC_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define SDL_arraysize                       dimof
#define SDL_TABLESIZE                       SDL_arraysize

// stdlib
#define SDL_assert                          VSF_SDL2_ASSERT
#define SDL_free                            free
#define SDL_malloc                          malloc
#define SDL_realloc                         realloc
#define SDL_calloc                          calloc

#define SDL_stack_alloc(__type, __count)    (__type*)SDL_malloc(sizeof(__type) * (__count))
#define SDL_stack_free(__data)              SDL_free(__data)
#define SDL_OutOfMemory()

#define SDL_min                             min
#define SDL_max                             max

// string
#define SDL_memset                          memset
#define SDL_memcpy                          memcpy
#define SDL_strlen                          strlen
#define SDL_strcmp                          strcmp
#define SDL_strcasecmp                      strcasecmp
#define SDL_strlcpy                         strlcpy
#define SDL_strrchr                         strrchr
#define SDL_strchr                          strchr

// ctype
#define SDL_toupper                         toupper

// cmath
#define SDL_ceil                            ceil
#define SDL_ceilf                           ceilf
#define SDL_ceill                           ceill

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

// extern APIs in math
double ceil(double x);
float ceilf(float x);
long double ceill(long double x);

#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif      // VSF_USE_SDL2
#endif      // __VSF_SDL2_STDINC_H__
/* EOF */
