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

#ifndef __VSF_SDL2_MUTEX_H__
#define __VSF_SDL2_MUTEX_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#include "kernel/vsf_kernel.h"
#include "SDL_stdinc.h"

#include "begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_SDL_CFG_FAKE_API == ENABLED
#define SDL_CreateMutex                 __vsf_sdl2_create_mutex
#define SDL_DestroyMutex                __vsf_sdl2_destroy_mutex
#define SDL_TryLockMutex                __vsf_sdl2_try_lock_mutex
#define SDL_LockMutex                   __vsf_sdl2_lock_mutex
#define SDL_UnlockMutex                 __vsf_sdl2_unlock_mutex
#endif

/*============================ TYPES =========================================*/

typedef vsf_mutex_t SDL_mutex;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern SDL_mutex* SDL_CreateMutex(void);
extern void SDL_DestroyMutex(SDL_mutex * mutex);
extern int SDL_TryLockMutex(SDL_mutex * mutex);
extern int SDL_LockMutex(SDL_mutex * mutex);
extern int SDL_UnlockMutex(SDL_mutex * mutex);

#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif      // VSF_USE_SDL2
#endif      // __VSF_SDL2_MUTEX_H__
/* EOF */
