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

#ifndef __VSF_SDL2_EVENTS_H__
#define __VSF_SDL2_EVENTS_H__

/*============================ INCLUDES ======================================*/

#include "../../vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#include "vsf.h"

#include "SDL_stdinc.h"

#include "SDL_keyboard.h"
#include "SDL_mouse.h"
//#include "SDL_joystick.h"
//#include "SDL_gamecontroller.h"
//#include "SDL_touch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_SDL_CFG_FAKE_API == ENABLED
#define SDL_PollEvent                   __vsf_sdl2_poll_event
#define SDL_WaitEventTimeout            __vsf_sdl2_wait_event_timeout
#define SDL_FlushEvent                  __vsf_sdl2_flush_event
#define SDL_EventState                  __vsf_sdl2_event_state
#endif

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/

// events
enum {
    SDL_QUERY                           = -1,
    SDL_IGNORE                          = 0,
    SDL_DISABLE                         = 0,
    SDL_ENABLE                          = 1,
    SDL_RELEASED                        = 0,
    SDL_PRESSED                         = 1,
};

typedef enum {
    SDL_FIRSTEVENT                      = 0,
    SDL_QUIT                            = 0x100,

    SDL_WINDOWEVENT                     = 0x200,
    SDL_SYSWMEVENT,

    SDL_KEYDOWN                         = 0x300,
    SDL_KEYUP,
    SDL_TEXTEDITING,
    SDL_TEXTINPUT,

    SDL_MOUSEMOTION                     = 0x400,
    SDL_MOUSEBUTTONDOWN,
    SDL_MOUSEBUTTONUP,
    SDL_MOUSEWHEEL,

    SDL_JOYAXISMOTION                   = 0x600,
    SDL_JOYBALLMOTION,
    SDL_JOYHATMOTION,
    SDL_JOYBUTTONDOWN,
    SDL_JOYBUTTONUP,
    SDL_JOYDEVICEADDED,
    SDL_JOYDEVICEREMOVED,

    SDL_CONTROLLERAXISMOTION            = 0x650,
    SDL_CONTROLLERBUTTONDOWN,
    SDL_CONTROLLERBUTTONUP,
    SDL_CONTROLLERDEVICEADDED,
    SDL_CONTROLLERDEVICEREMOVED,
    SDL_CONTROLLERDEVICEREMAPPED,

    SDL_FINGERDOWN                      = 0x700,
    SDL_FINGERUP,
    SDL_FINGERMOTION,

    SDL_DROPFILE                        = 0x1000,
    SDL_DROPTEXT,
    SDL_DROPBEGIN,
    SDL_DROPCOMPLETE,

    SDL_AUDIODEVICEADDED                = 0x1100,
    SDL_AUDIODEVICEREMOVED,

#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
    SDL_ACTIVEEVENT                     = 0x2000,
#endif

    SDL_USEREVENT                       = 0x8000,
    SDL_LASTEVENT                       = 0xFFFF
} SDL_EventType;

enum {
    SDL_HAT_CENTERED                    = 0x00,
    SDL_HAT_UP                          = 0x01,
    SDL_HAT_RIGHT                       = 0x02,
    SDL_HAT_DOWN                        = 0x04,
    SDL_HAT_LEFT                        = 0x08,
    SDL_HAT_RIGHTUP                     = (SDL_HAT_RIGHT | SDL_HAT_UP),
    SDL_HAT_RIGHTDOWN                   = (SDL_HAT_RIGHT | SDL_HAT_DOWN),
    SDL_HAT_LEFTUP                      = (SDL_HAT_LEFT | SDL_HAT_UP),
    SDL_HAT_LEFTDOWN                    = (SDL_HAT_LEFT | SDL_HAT_DOWN),
};

typedef struct SDL_KeyboardEvent {
    uint32_t type;
    uint32_t timestamp;
    uint32_t windowID;
    SDL_Keysym keysym;
    uint8_t state;
    uint8_t repeat;
} SDL_KeyboardEvent;

typedef int SDL_JoystickID;
typedef struct SDL_Joystick SDL_Joystick;
typedef struct SDL_JoyHatEvent {
    uint32_t type;
    uint32_t timestamp;
    SDL_JoystickID which;
    uint8_t hat;
    uint8_t value;
} SDL_JoyHatEvent;
typedef struct SDL_JoyAxisEvent {
    uint32_t type;
    uint32_t timestamp;
    SDL_JoystickID which;
    uint8_t axis;
    int16_t value;
} SDL_JoyAxisEvent;
typedef struct SDL_JoyBallEvent {
    uint32_t type;
    uint32_t timestamp;
    SDL_JoystickID which;
    uint8_t ball;
    int16_t xrel;
    int16_t yrel;
} SDL_JoyBallEvent;
typedef struct SDL_JoyButtonEvent {
    uint32_t type;
    uint32_t timestamp;
    SDL_JoystickID which;
    uint8_t button;
    uint8_t state;
} SDL_JoyButtonEvent;
typedef struct SDL_JoyDeviceEvent {
    uint32_t type;
    uint32_t timestamp;
    int32_t which;
} SDL_JoyDeviceEvent;
typedef struct SDL_MouseMotionEvent {
    uint32_t type;
    uint32_t timestamp;
    uint32_t windowID;
    uint32_t which;
    uint32_t state;
    int32_t x;
    int32_t y;
    int32_t xrel;
    int32_t yrel;
} SDL_MouseMotionEvent;
enum {
    SDL_BUTTON_LEFT,
    SDL_BUTTON_MIDDLE,
    SDL_BUTTON_RIGHT,
    SDL_BUTTON_X1,
    SDL_BUTTON_X2,
};
typedef struct SDL_MouseButtonEvent {
    uint32_t type;
    uint32_t timestamp;
    uint32_t windowID;
    uint32_t which;
    uint8_t button;
    uint8_t state;
    uint8_t clicks;
    int32_t x;
    int32_t y;
} SDL_MouseButtonEvent;
typedef struct SDL_MouseWheelEvent {
    uint32_t type;
    uint32_t timestamp;
    uint32_t windowID;
    uint32_t which;
    int32_t x;
    int32_t y;
    uint32_t direction;
} SDL_MouseWheelEvent;
typedef enum {
    SDL_WINDOWEVENT_NONE,
    SDL_WINDOWEVENT_SHOWN,
    SDL_WINDOWEVENT_HIDDEN,
    SDL_WINDOWEVENT_EXPOSED,
    SDL_WINDOWEVENT_MOVED,
    SDL_WINDOWEVENT_RESIZED,
    SDL_WINDOWEVENT_SIZE_CHANGED,
    SDL_WINDOWEVENT_MINIMIZED,
    SDL_WINDOWEVENT_MAXIMIZED,
    SDL_WINDOWEVENT_RESTORED,
    SDL_WINDOWEVENT_ENTER,
    SDL_WINDOWEVENT_LEAVE,
    SDL_WINDOWEVENT_FOCUS_GAINED,
    SDL_WINDOWEVENT_FOCUS_LOST,
    SDL_WINDOWEVENT_CLOSE,
} SDL_WindowEventID;
typedef struct SDL_WindowEvent {
    uint32_t type;
    uint32_t timestamp;
    uint32_t windowID;
    uint8_t event;
    int32_t data1;
    int32_t data2;
} SDL_WindowEvent;
#define SDL_TEXTINPUTEVENT_TEXT_SIZE        32
typedef struct SDL_TextInputEvent {
    uint32_t type;
    uint32_t timestamp;
    uint32_t windowID;
    char text[SDL_TEXTINPUTEVENT_TEXT_SIZE];
} SDL_TextInputEvent;
typedef struct SDL_DropEvent {
    uint32_t type;
    uint32_t timestamp;
    // file should be allocated by SDL_malloc
    //  user should call SDL_free to free file
    char *file;
    uint32_t windowID;
} SDL_DropEvent;

typedef struct SDL_Cursor SDL_Cursor;
typedef enum SDL_SystemCursor {
    SDL_SYSTEM_CURSOR_ARROW,
    SDL_NUM_SYSTEM_CURSORS,
} SDL_SystemCursor;

#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
enum {
    SDL_APPMOUSEFOCUS           = 1 << 0,
    SDL_APPINPUTFOCUS           = 1 << 1,
    SDL_APPACTIVE               = 1 << 2,
};
typedef struct SDL_ActiveEvent {
    uint8_t type;
    uint8_t gain;
    uint8_t state;
} SDL_ActiveEvent;
#endif

typedef union SDL_Event {
    uint32_t type;
    SDL_KeyboardEvent key;

    SDL_JoyAxisEvent jaxis;
    SDL_JoyBallEvent jball;
    SDL_JoyHatEvent jhat;
    SDL_JoyButtonEvent jbutton;
    SDL_JoyDeviceEvent jdevice;
    SDL_MouseMotionEvent motion;
    SDL_MouseButtonEvent button;
    SDL_MouseWheelEvent wheel;

    SDL_WindowEvent window;
    SDL_TextInputEvent text;
    SDL_DropEvent drop;

#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
    SDL_ActiveEvent active;
#endif
} SDL_Event;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

extern int SDL_PollEvent(SDL_Event * event);
extern int SDL_WaitEventTimeout(SDL_Event * event, int timeout);
extern void SDL_FlushEvent(uint32_t type);
extern uint8_t SDL_EventState(uint32_t type, int state);

#ifdef __cplusplus
}
#endif

#endif      // VSF_USE_SDL
#endif      // __VSF_SDL2_EVENTS_H__