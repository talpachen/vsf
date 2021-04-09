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

/*============================ INCLUDES ======================================*/

#include "./vsf_sdl2_cfg.h"

#if VSF_USE_SDL2 == ENABLED

#define __VSF_DISP_CLASS_INHERIT__
// for vsf_disp
#include "component/vsf_component.h"
#include "./include/SDL2/SDL.h"

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe111
#endif

/*============================ MACROS ========================================*/

#if VSF_INPUT_CFG_REGISTRATION_MECHANISM != ENABLED
#   error VSF_INPUT_CFG_REGISTRATION_MECHANISM is needed for vsf_sdl2
#endif

#if VSF_KERNEL_CFG_SUPPORT_THREAD != ENABLED
#   error vsf_sdl2 can only run in thread mode
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __vsf_sdl2_fast_copy(__pdst, __psrc, __num)                             \
    uint_fast32_t __temp = ((__num) + 3) / 4;                                   \
    if (!__num) { return; }                                                     \
    switch (__num % 4) {                                                        \
        case 0: do {    *__pdst++ = *__psrc++;                                  \
        case 3:         *__pdst++ = *__psrc++;                                  \
        case 2:         *__pdst++ = *__psrc++;                                  \
        case 1:         *__pdst++ = *__psrc++;                                  \
        } while (--__temp);                                                     \
    }

#define __vsf_sdl2_fast_set(__pdst, __value, __num)                             \
    uint_fast32_t __temp = ((__num) + 3) / 4;                                   \
    if (!__num) { return; }                                                     \
    switch (__num % 4) {                                                        \
        case 0: do {    *__pdst++ = __value;                                    \
        case 3:         *__pdst++ = __value;                                    \
        case 2:         *__pdst++ = __value;                                    \
        case 1:         *__pdst++ = __value;                                    \
        } while (--__temp);                                                     \
    }

/*============================ TYPES =========================================*/

struct SDL_Window {
    const char *title;
    SDL_Rect area;
    uint32_t flags;
    vk_disp_color_type_t format;
    SDL_Surface surface;
    uint32_t pixels[0] ALIGN(4);
};

struct SDL_Renderer {
    SDL_Window *window;
    uint32_t flags;
};

struct SDL_Texture {
    uint16_t w, h;
    SDL_PixelFormat *format;
    void *pixels;

    uint32_t __pixels[0] ALIGN(4);
};

typedef struct vsf_sdl2_t {
    vk_disp_t *disp;
    uint32_t init_flags;
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    uint32_t start_ms;
#endif
#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
    SDL_Surface *sdl1_screen;
#endif
    vk_input_notifier_t notifier;
    struct {
        char *text;
    } clipboard;
} vsf_sdl2_t;

/*============================ PROTOTYPES ====================================*/

extern void vsf_sdl2_pixel_copy(uint_fast16_t data_line_num, uint_fast32_t data_line_size,
                                uint8_t *pdst, uint_fast32_t dst_pitch,
                                uint8_t *psrc, uint_fast32_t src_pitch);
extern void vsf_sdl2_pixel_fill(uint_fast16_t data_line_num, uint_fast32_t pixel_line_size,
                                uint8_t *pbuf, uint_fast32_t dst_pitch,
                                uint32_t color, uint_fast8_t color_byte_size);

/*============================ LOCAL VARIABLES ===============================*/

NO_INIT vsf_sdl2_t __vsf_sdl2;

struct {
    uint32_t color;
    SDL_PixelFormat format;
} static const __vsf_sdl2_color[] = {
    {
        .color              = SDL_PIXELFORMAT_ARGB8888,
        .format             = {
            .format         = SDL_PIXELFORMAT_ARGB8888,
            .BitsPerPixel   = 32,
            .BytesPerPixel  = 4,
            .Rmask          = 0x00FF0000,
            .Gmask          = 0x0000FF00,
            .Bmask          = 0x000000FF,
            .Amask          = 0xFF000000,
            .Rshift         = 16,
            .Gshift         = 8,
            .Bshift         = 0,
            .Ashift         = 24,
            .Rloss          = 0,
            .Gloss          = 0,
            .Bloss          = 0,
            .Aloss          = 0,
        },
    },
    {
        .color              = SDL_PIXELFORMAT_RGB565,
        .format             = {
            .format         = SDL_PIXELFORMAT_RGB565,
            .BitsPerPixel   = 16,
            .BytesPerPixel  = 2,
            .Rmask          = 0xF800,
            .Gmask          = 0x07E0,
            .Bmask          = 0x001F,
            .Rshift         = 11,
            .Gshift         = 5,
            .Bshift         = 0,
            .Rloss          = 3,
            .Gloss          = 2,
            .Bloss          = 3,
        },
    },
    {
        .color              = SDL_PIXELFORMAT_RGB24,
        .format             = {
            .format         = SDL_PIXELFORMAT_RGB24,
            .BitsPerPixel   = 24,
            .BytesPerPixel  = 3,
            .Rmask          = 0x00FF0000,
            .Gmask          = 0x0000FF00,
            .Bmask          = 0x000000FF,
            .Rshift         = 16,
            .Gshift         = 8,
            .Bshift         = 0,
            .Rloss          = 0,
            .Gloss          = 0,
            .Bloss          = 0,
        },
    },
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

static void __vsf_sdl2_fast_memcpy8(uint8_t *pdst, uint8_t *psrc, uint_fast32_t num)
{
    __vsf_sdl2_fast_copy(pdst, psrc, num)
}

#if __IS_COMPILER_IAR__
//! statement is unreachable
#   pragma diag_suppress=pe177
#endif
// __vsf_sdl2_fast_memcpy16 and __vsf_sdl2_fast_memcpy32 are reserved for future use
static void __vsf_sdl2_fast_memcpy16(uint16_t *pdst, uint16_t *psrc, uint_fast32_t num)
{
    __vsf_sdl2_fast_copy(pdst, psrc, num)
}
static void __vsf_sdl2_fast_memcpy32(uint32_t *pdst, uint32_t *psrc, uint_fast32_t num)
{
    __vsf_sdl2_fast_copy(pdst, psrc, num)
}

static void __vsf_sdl2_fast_memset8(uint8_t *pdst, uint8_t value, uint_fast32_t num)
{
    __vsf_sdl2_fast_set(pdst, value, num)
}
static void __vsf_sdl2_fast_memset16(uint16_t *pdst, uint16_t value, uint_fast32_t num)
{
    __vsf_sdl2_fast_set(pdst, value, num)
}
static void __vsf_sdl2_fast_memset32(uint32_t *pdst, uint32_t value, uint_fast32_t num)
{
    __vsf_sdl2_fast_set(pdst, value, num)
}

void SDL_memset4(void *dst, int val, size_t dwords)
{
    __vsf_sdl2_fast_memset32(dst, (uint32_t)val, dwords);
}

#ifndef WEAK_SDL2_PIXEL_COPY
WEAK(vsf_sdl2_pixel_copy)
void vsf_sdl2_pixel_copy(   uint_fast16_t data_line_num, uint_fast32_t data_line_size,
                            uint8_t *pdst, uint_fast32_t dst_pitch,
                            uint8_t *psrc, uint_fast32_t src_pitch)
{
    for (uint_fast16_t i = 0; i < data_line_num; i++) {
        __vsf_sdl2_fast_memcpy8(pdst, psrc, data_line_size);
        pdst += dst_pitch;
        psrc += src_pitch;
    }
}
#endif

#ifndef WEAK_SDL2_PIXEL_FILL
WEAK(vsf_sdl2_pixel_fill)
void vsf_sdl2_pixel_fill(   uint_fast16_t data_line_num, uint_fast32_t pixel_line_size,
                            uint8_t *pbuf, uint_fast32_t dst_pitch,
                            uint32_t color, uint_fast8_t color_byte_size)
{
    for (uint_fast16_t i = 0; i < data_line_num; i++) {
        switch (color_byte_size) {
        case 1: __vsf_sdl2_fast_memset8((uint8_t *)pbuf, (uint8_t)color, pixel_line_size);      break;
        case 2: __vsf_sdl2_fast_memset16((uint16_t *)pbuf, (uint16_t)color, pixel_line_size);   break;
        case 4: __vsf_sdl2_fast_memset32((uint32_t *)pbuf, (uint32_t)color, pixel_line_size);   break;
        }
        pbuf += dst_pitch;
    }
}
#endif

static uint32_t __SDL_GetColorFromMask(uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
    for (uint_fast16_t i = 0; i < dimof(__vsf_sdl2_color); i++) {
        if (    __vsf_sdl2_color[i].format.Rmask == Rmask
            &&  __vsf_sdl2_color[i].format.Gmask == Gmask
            &&  __vsf_sdl2_color[i].format.Bmask == Bmask
            &&  __vsf_sdl2_color[i].format.Amask == Amask) {
            return __vsf_sdl2_color[i].color;
        }
    }

    int_fast8_t pixel_bitlen = vsf_msb(Rmask | Gmask | Bmask | Amask);
    if (pixel_bitlen < 0) {
        return SDL_PIXELFORMAT_UNKNOWN;
    }

    uint_fast8_t pixel_bytelen = (++pixel_bitlen + 7) >> 3;
    return VSF_DISP_COLOR_VALUE(SDL_PIXELFORMAT_BYMASK_IDX, pixel_bitlen, pixel_bytelen, Amask != 0);
}

static const SDL_PixelFormat * __SDL_GetFormatFromColor(uint32_t color)
{
    for (uint_fast16_t i = 0; i < dimof(__vsf_sdl2_color); i++) {
        if (__vsf_sdl2_color[i].color == color) {
            return &__vsf_sdl2_color[i].format;
        }
    }
    return NULL;
}

static void __SDL_InitFormatMask(SDL_PixelFormat *format, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
    uint_fast8_t tmp8;
    format->Rmask = Rmask;
    format->Gmask = Gmask;
    format->Bmask = Bmask;
    format->Amask = Amask;

    tmp8 = vsf_clz(Rmask);
    Rmask <<= tmp8;
    format->Rloss = vsf_clz(~Rmask);
    format->Rshift = 32 - (tmp8 + format->Rloss);
    format->Rloss = 8 - format->Rloss;

    tmp8 = vsf_clz(Gmask);
    Gmask <<= tmp8;
    format->Gloss = vsf_clz(~Gmask);
    format->Gshift = 32 - (tmp8 + format->Gloss);
    format->Gloss = 8 - format->Gloss;

    tmp8 = vsf_clz(Bmask);
    Bmask <<= tmp8;
    format->Bloss = vsf_clz(~Bmask);
    format->Bshift = 32 - (tmp8 + format->Bloss);
    format->Bloss = 8 - format->Bloss;

    tmp8 = vsf_clz(Amask);
    Amask <<= tmp8;
    format->Aloss = vsf_clz(~Amask);
    format->Ashift = 32 - (tmp8 + format->Aloss);
    format->Aloss = 8 - format->Aloss;
}

static void __SDL_BlendWithFormat(
                            uint_fast16_t h, uint_fast32_t w,
                            uint8_t *pdst, uint_fast32_t dst_pitch,
                            uint8_t *psrc, uint_fast32_t src_pitch,
                            SDL_PixelFormat * dst_fmt, SDL_PixelFormat * src_fmt)
{
    VSF_SDL2_ASSERT((dst_fmt != NULL) && (src_fmt != NULL));
    uint_fast8_t dst_pixel_size = vsf_disp_get_pixel_format_bytesize(dst_fmt->format);
    uint_fast8_t src_pixel_size = vsf_disp_get_pixel_format_bytesize(src_fmt->format);

    uint8_t dst_rshift  = dst_fmt->Rshift;
    uint8_t dst_gshift  = dst_fmt->Gshift;
    uint8_t dst_bshift  = dst_fmt->Bshift;
    uint8_t dst_ashift  = dst_fmt->Ashift;
    uint8_t dst_rloss  = dst_fmt->Rloss;
    uint8_t dst_gloss  = dst_fmt->Gloss;
    uint8_t dst_bloss  = dst_fmt->Bloss;
    uint8_t dst_aloss  = dst_fmt->Aloss;
    uint32_t dst_rmask  = dst_fmt->Rmask;
    uint32_t dst_gmask  = dst_fmt->Gmask;
    uint32_t dst_bmask  = dst_fmt->Bmask;
    uint32_t dst_amask  = dst_fmt->Amask;

    uint8_t src_rshift  = src_fmt->Rshift;
    uint8_t src_gshift  = src_fmt->Gshift;
    uint8_t src_bshift  = src_fmt->Bshift;
    uint8_t src_ashift  = src_fmt->Ashift;
    uint8_t src_rloss  = src_fmt->Rloss;
    uint8_t src_gloss  = src_fmt->Gloss;
    uint8_t src_bloss  = src_fmt->Bloss;
    uint8_t src_aloss  = src_fmt->Aloss;
    uint32_t src_rmask  = src_fmt->Rmask;
    uint32_t src_gmask  = src_fmt->Gmask;
    uint32_t src_bmask  = src_fmt->Bmask;
    uint32_t src_amask  = src_fmt->Amask;

    uint32_t color, dcolor, da;
    uint8_t a, r, g, b;
    uint8_t dr, dg, db;
    uint8_t *dst_tmp, *src_tmp;
    for (uint_fast16_t i = 0; i < h; i++) {
        dst_tmp = pdst;
        src_tmp = psrc;
        for (uint_fast16_t j = 0; j < w; j++) {
            switch (src_pixel_size) {
            case 1: color = *(uint8_t *)src_tmp;    src_tmp += 1;   break;
            case 2: color = *(uint16_t *)src_tmp;   src_tmp += 2;   break;
            case 3: color = *(uint32_t *)src_tmp;   src_tmp += 3;   break;
            case 4: color = *(uint32_t *)src_tmp;   src_tmp += 4;   break;
            default:VSF_SDL2_ASSERT(false);
            }
            r = ((color & src_rmask) >> src_rshift) << src_rloss;
            g = ((color & src_gmask) >> src_gshift) << src_gloss;
            b = ((color & src_bmask) >> src_bshift) << src_bloss;
            a = ((color & src_amask) >> src_ashift) << src_aloss;

            switch (dst_pixel_size) {
            case 1: dcolor = *(uint8_t *)dst_tmp;                   break;
            case 2: dcolor = *(uint16_t *)dst_tmp;                  break;
            case 3: dcolor = *(uint32_t *)dst_tmp;                  break;
            case 4: dcolor = *(uint32_t *)dst_tmp;                  break;
            default:VSF_SDL2_ASSERT(false);
            }
            dr = ((dcolor & dst_rmask) >> dst_rshift) << dst_rloss;
            dg = ((dcolor & dst_gmask) >> dst_gshift) << dst_gloss;
            db = ((dcolor & dst_bmask) >> dst_bshift) << dst_bloss;
            da = dcolor & dst_amask;

            r = (r * a + dr * (255 - a)) / 255;
            g = (g * a + dg * (255 - a)) / 255;
            b = (b * a + db * (255 - a)) / 255;
            color =     ((r >> dst_rloss) << dst_rshift)
                    |   ((g >> dst_gloss) << dst_gshift)
                    |   ((b >> dst_bloss) << dst_bshift)
                    |   da;
            switch (dst_pixel_size) {
            case 1: *(uint8_t *)dst_tmp = color;    dst_tmp += 1;   break;
            case 2: *(uint16_t *)dst_tmp = color;   dst_tmp += 2;   break;
            case 3: dst_tmp[0] = color & 0x0000FF;
                    dst_tmp[1] = color & 0x00FF00;
                    dst_tmp[2] = color & 0xFF0000;  dst_tmp += 3;   break;
            case 4: *(uint32_t *)dst_tmp = color;   dst_tmp += 4;   break;
            default:VSF_SDL2_ASSERT(false);
            }
        }
        pdst += dst_pitch;
        psrc += src_pitch;
    }
}

const SDL_version * SDL_Linked_Version(void)
{
    static const SDL_version __sdl2_version = {
        .major = SDL_MAJOR_VERSION,
        .minor = SDL_MINOR_VERSION,
        .patch = SDL_PATCHLEVEL,
    };
    return &__sdl2_version;
}

static void __vsf_sdl2_on_input(vk_input_type_t type, vk_input_evt_t *evt)
{
}

static void __vsf_sdl2_disp_on_ready(vk_disp_t *disp)
{
    vsf_eda_t *eda = __vsf_sdl2.disp->ui_data;
    VSF_SDL2_ASSERT(eda != NULL);
    vsf_eda_post_evt(eda, VSF_EVT_RETURN);
}

static inline bool __vsf_sdl2_cord_intersect(int *start_out, int *width_out, int start0, int width0, int start1, int width1)
{
    int start = max(start0, start1);
    int end0 = start0 + width0;
    int end1 = start1 + width1;
    int end = min(end0, end1);

    *start_out  = start;
    *width_out = end - start;
    return end > start;
}

static bool __vsf_sdl2_rect_intersect(SDL_Rect *rect_out, const SDL_Rect *rect0, const SDL_Rect *rect1)
{
    VSF_SDL2_ASSERT((rect_out != NULL) && (rect0 != NULL) && (rect1 != NULL));
    bool x_intersect = __vsf_sdl2_cord_intersect(&rect_out->x, &rect_out->w, rect0->x, rect0->w, rect1->x, rect1->w);
    bool y_intersect = __vsf_sdl2_cord_intersect(&rect_out->y, &rect_out->h, rect0->y, rect0->h, rect1->y, rect1->h);
    return x_intersect && y_intersect;
}

static void __vsf_sdl2_disp_refresh(vk_disp_area_t *area, void *pixels)
{
    __vsf_sdl2.disp->ui_data = vsf_eda_get_cur();
    vk_disp_refresh(__vsf_sdl2.disp, area, pixels);
    vsf_thread_wfe(VSF_EVT_RETURN);
}

void vsf_sdl2_init(vk_disp_t *disp)
{
    __vsf_sdl2.init_flags = 0;
    __vsf_sdl2.disp = disp;
    __vsf_sdl2.sdl1_screen = NULL;
}

int SDL_InitSubSystem(uint32_t flags)
{
    if (flags & SDL_INIT_GAMECONTROLLER) {
        flags |= SDL_INIT_JOYSTICK;
    }
    if (flags & (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
        flags |= SDL_INIT_EVENTS;
    }

    flags &= ~__vsf_sdl2.init_flags;
    __vsf_sdl2.init_flags = flags;
    if (flags & SDL_INIT_EVENTS) {
        __vsf_sdl2.notifier.mask = 1 << VSF_INPUT_TYPE_TOUCHSCREEN;
        __vsf_sdl2.notifier.on_evt = (vk_input_on_evt_t)__vsf_sdl2_on_input;
        vk_input_notifier_register(&__vsf_sdl2.notifier);
    }

    if (flags & SDL_INIT_VIDEO) {
        __vsf_sdl2.disp->ui_data = NULL;
        __vsf_sdl2.disp->ui_on_ready = __vsf_sdl2_disp_on_ready;
        vk_disp_init(__vsf_sdl2.disp);
    }
    return 0;
}

int SDL_Init(uint32_t flags)
{
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
    __vsf_sdl2.start_ms = vsf_systimer_get_ms();
#endif
    return SDL_InitSubSystem(flags);
}

void SDL_Quit(void)
{
    uint32_t flags = __vsf_sdl2.init_flags;
    if (flags & SDL_INIT_EVENTS) {
        vk_input_notifier_unregister(&__vsf_sdl2.notifier);
    }

    if (flags & SDL_INIT_VIDEO) {
        // TODO:
    }
}

const char * SDL_GetPlatform(void)
{
    return "vsf";
}

int SDL_GetDesktopDisplayMode(int display_index, SDL_DisplayMode *mode)
{
    if (NULL == __vsf_sdl2.disp) {
        return -1;
    }

    if (mode != NULL) {
        mode->format = vsf_disp_get_pixel_format(__vsf_sdl2.disp);
#ifdef VSF_SDL_CFG_DISPLAY_HEIGHT
        mode->h = VSF_SDL_CFG_DISPLAY_HEIGHT;
#else
        mode->h = __vsf_sdl2.disp->param.height;
#endif
#ifdef VSF_SDL_CFG_DISPLAY_WIDTH
        mode->w = VSF_SDL_CFG_DISPLAY_WIDTH;
#else
        mode->w = __vsf_sdl2.disp->param.width;
#endif
    }
    return 0;
}

int SDL_GetCurrentDisplayMode(int display_index, SDL_DisplayMode *mode)
{
    return SDL_GetDesktopDisplayMode(display_index, mode);
}

const char *SDL_GetError(void)
{
    return NULL;
}

int SDL_SetError(const char* fmt, ...)
{
    return 0;
}


SDL_RWops * SDL_RWFromFile(const char * file, const char * mode)
{
    return fopen(file, mode);
}
int64_t SDL_RWsize(SDL_RWops * context)
{
    fseek(context, 0, SEEK_END);
    off64_t off64 = ftello64(context);
    fseek(context, 0, SEEK_SET);
    return off64;
}
int SDL_RWclose(SDL_RWops * context)
{
    return fclose(context);
}
int64_t SDL_RWseek(SDL_RWops * context, int64_t offset, int whence)
{
    return fseek(context, offset, whence);
}
int64_t SDL_RWtell(SDL_RWops * context)
{
    return ftello64(context);
}
size_t SDL_RWread(SDL_RWops * context, void * ptr, size_t size, size_t maxnum)
{
    return fread(ptr, size, maxnum, context);
}
size_t SDL_RWwrite(SDL_RWops * context, const void * ptr, size_t size, size_t num)
{
    return fwrite(ptr, size, num, context);
}


SDL_Window * SDL_CreateWindow(const char *title, int x, int y, int w, int h, uint32_t flags)
{
    uint_fast8_t pixel_bit_size = vsf_disp_get_pixel_bitsize(__vsf_sdl2.disp);
    uint_fast8_t pixel_byte_size = vsf_disp_get_pixel_bytesize(__vsf_sdl2.disp);

#ifdef VSF_SDL_CFG_WINDOW_PTR
    static bool __is_allocated = false;
    VSF_SDL2_ASSERT(!__is_allocated);
    SDL_Window *window = __is_allocated ? NULL : (SDL_Window *)VSF_SDL_CFG_WINDOW_PTR;
    __is_allocated = true;
#else
    SDL_Window *window = vsf_heap_malloc(sizeof(struct SDL_Window) + pixel_byte_size * w * h);
#endif
    if (window != NULL) {
        window->title   = title;
        window->area.x  = x;
        window->area.y  = y;
        window->area.w  = w;
        window->area.h  = h;
        window->flags   = flags;
        window->format  = vsf_disp_get_pixel_format(__vsf_sdl2.disp);
        // TODO: initialize pixels according to format of disp

        SDL_Surface *surface = &window->surface;
        surface->format = (SDL_PixelFormat *)__SDL_GetFormatFromColor(window->format);
        // default windows formats MUST be supported, because here is no masks to generate format
        VSF_SDL2_ASSERT(surface->format != NULL);
        surface->w = w;
        surface->h = h;
        surface->pitch = w * pixel_byte_size;
        surface->pixels = window->pixels;
    }
    return window;
}

void SDL_DestroyWindow(SDL_Window *window)
{
    VSF_SDL2_ASSERT(window != NULL);
    vsf_heap_free(window);
}

void SDL_GetWindowPosition(SDL_Window * window, int * x, int * y)
{
    if (x != NULL) {
        *x = window->area.x;
    }
    if (y != NULL) {
        *y = window->area.y;
    }
}

SDL_Surface * SDL_GetWindowSurface(SDL_Window *window)
{
    return &window->surface;
}

void SDL_GetWindowSize(SDL_Window* window, int *w, int *h)
{
    if (w != NULL) {
        *w = window->area.w;
    }
    if (h != NULL) {
        *h = window->area.h;
    }
}

int SDL_UpdateWindowSurface(SDL_Window * window)
{
    vk_disp_area_t area = {
        .pos.x          = 0,
        .pos.y          = 0,
        .size.x         = window->area.w,
        .size.y         = window->area.h,
    };
    __vsf_sdl2_disp_refresh(&area, window->pixels);
    return 0;
}

int SDL_UpdateWindowSurfaceRects(SDL_Window *window, const SDL_Rect *rects, int numrects)
{
    // TODO: update area according to rects
    return SDL_UpdateWindowSurface(window);
}

void SDL_ShowWindow(SDL_Window *window) {}
void SDL_SetWindowIcon(SDL_Window * window, SDL_Surface * icon) {}
void SDL_SetWindowTitle(SDL_Window * window, const char * title) {}
int SDL_SetWindowFullscreen(SDL_Window * window, uint32_t flags) { return 0; }
void SDL_RestoreWindow(SDL_Window * window) {}
void SDL_MaximizeWindow(SDL_Window * window) {}
uint32_t SDL_GetWindowFlags(SDL_Window * window)
{
    return  SDL_WINDOW_FULLSCREEN
        |   SDL_WINDOW_SHOWN
        |   SDL_WINDOW_MAXIMIZED
        |   SDL_WINDOW_INPUT_GRABBED
        |   SDL_WINDOW_INPUT_FOCUS
        |   SDL_WINDOW_MOUSE_FOCUS
        |   SDL_WINDOW_MOUSE_CAPTURE
        |   SDL_WINDOW_ALWAYS_ON_TOP
        |   SDL_WINDOW_SKIP_TASKBAR;
}

SDL_Surface * __SDL_CreateRGBSurfaceWithFormat(int w, int h, int depth, uint32_t format, uint_fast8_t pixel_size)
{
    SDL_Surface *surface = vsf_heap_malloc(sizeof(SDL_Surface) + pixel_size * w * h);
    if (surface != NULL) {
        surface->__format.format    = format;
        surface->format             = &surface->__format;
        surface->w                  = w;
        surface->h                  = h;
    }
    return surface;
}

SDL_Surface * SDL_CreateRGBSurfaceWithFormat(uint32_t flags, int w, int h, int depth, uint32_t format)
{
    uint_fast8_t pixel_size = vsf_disp_get_pixel_format_bytesize(format);
    SDL_Surface *surface = __SDL_CreateRGBSurfaceWithFormat(w, h, depth, format, pixel_size);
    if (surface != NULL) {
        surface->pitch              = w * pixel_size;
        surface->pixels             = &surface->__pixels;
    }
    return surface;
}

SDL_Surface * SDL_CreateRGBSurfaceWithFormatFrom(void * pixels, int w, int h, int depth, int pitch, uint32_t format)
{
    SDL_Surface * surface = __SDL_CreateRGBSurfaceWithFormat(w, h, depth, format, 0);
    if (surface != NULL) {
        surface->flags              |= SDL_PREALLOC;
        surface->pitch              = pitch;
        surface->pixels             = pixels;
    }
    return surface;
}

static void __SDL_SurfaceInitFormat(SDL_Surface * surface, uint32_t color, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
    SDL_PixelFormat *format = (SDL_PixelFormat *)__SDL_GetFormatFromColor(color);
    if (format != NULL) {
        surface->format             = format;
    } else {
        __SDL_InitFormatMask(&surface->__format, Rmask, Gmask, Bmask, Amask);
    }
}

SDL_Surface * SDL_CreateRGBSurface(uint32_t flags, int w, int h, int depth, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
    uint32_t format = __SDL_GetColorFromMask(Rmask, Gmask, Bmask, Amask);
    VSF_SDL2_ASSERT(format != SDL_PIXELFORMAT_UNKNOWN);
    SDL_Surface * surface = SDL_CreateRGBSurfaceWithFormat(flags, w, h, depth, format);
    if (surface != NULL) {
        __SDL_SurfaceInitFormat(surface, format, Rmask, Gmask, Bmask, Amask);
    }
    return surface;
}

SDL_Surface * SDL_CreateRGBSurfaceFrom(void * pixels, int w, int h, int depth, int pitch, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
    uint32_t format = __SDL_GetColorFromMask(Rmask, Gmask, Bmask, Amask);
    VSF_SDL2_ASSERT(format != SDL_PIXELFORMAT_UNKNOWN);
    SDL_Surface * surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, w, h, depth, pitch, format);
    if (surface != NULL) {
        __SDL_SurfaceInitFormat(surface, format, Rmask, Gmask, Bmask, Amask);
    }
    return surface;
}

void SDL_FreeSurface(SDL_Surface *surface)
{
    VSF_SDL2_ASSERT(surface != NULL);
    vsf_heap_free(surface);
}

int SDL_LockSurface(SDL_Surface *surface)
{
    return 0;
}

void SDL_UnlockSurface(SDL_Surface *surface)
{
}

int SDL_SetSurfaceBlendMode(SDL_Surface * surface, SDL_BlendMode blendMode)
{
    surface->blend = blendMode;
    return 0;
}

int SDL_SetColorKey(SDL_Surface * surface, int flag, uint32_t key)
{
    return 0;
}

int SDL_FillRect(SDL_Surface * surface, const SDL_Rect * rect, uint32_t color)
{
    uint_fast8_t pixel_size = surface->format->BytesPerPixel;
    SDL_Rect surface_rect = {
        .w      = surface->w,
        .h      = surface->h,
    };
    SDL_Rect realrect;

    if (NULL == rect) {
        rect = &surface_rect;
    }
    if (!__vsf_sdl2_rect_intersect(&realrect, rect, &surface_rect)) {
        return 0;
    }
    vsf_sdl2_pixel_fill(rect->h, rect->w, surface->pixels, surface->pitch, color, pixel_size);
    return 0;
}

int SDL_BlitSurface(SDL_Surface * src, const SDL_Rect * srcrect, SDL_Surface * dst, SDL_Rect * dstrect)
{
    VSF_SDL2_ASSERT((src != NULL) && (dst != NULL));

    SDL_Rect src_real_area, dst_real_area;
    SDL_Rect src_area = {
        .w      = src->w,
        .h      = src->h,
    };
    SDL_Rect dst_area = {
        .w      = dst->w,
        .h      = dst->h,
    };

    if (srcrect != NULL) {
        if (!__vsf_sdl2_rect_intersect(&src_real_area, srcrect, &src_area)) {
            return -1;
        }
    } else {
        src_real_area = src_area;
    }
    if (dstrect != NULL) {
        if (!__vsf_sdl2_rect_intersect(&dst_real_area, dstrect, &dst_area)) {
            return -1;
        }
    } else {
        dst_real_area = dst_area;
    }
    dst_real_area.w = src_real_area.w = min(dst_real_area.w, src_real_area.w);
    dst_real_area.h = src_real_area.h = min(dst_real_area.h, src_real_area.h);

    uint_fast8_t dst_pixel_size = vsf_disp_get_pixel_format_bytesize(dst->format->format);
    uint_fast8_t src_pixel_size = vsf_disp_get_pixel_format_bytesize(src->format->format);
    __SDL_BlendWithFormat(src_real_area.h, src_real_area.w,
                (uint8_t *)dst->pixels + dst_pixel_size * (dst_real_area.y * dst_area.w + dst_real_area.x), dst->pitch,
                (uint8_t *)src->pixels + src_pixel_size * (src_real_area.y * src_area.w + src_real_area.x), src->pitch,
                dst->format, src->format);
    return 0;
}

SDL_Surface * SDL_ConvertSurfaceFormat(SDL_Surface * src, uint32_t format, uint32_t flags)
{
    uint_fast8_t depth = vsf_disp_get_pixel_format_bitsize(format);
    SDL_Surface * surface_new = SDL_CreateRGBSurfaceWithFormat(flags, src->w, src->h, depth, format);
    if (surface_new != NULL) {
        SDL_BlitSurface(src, NULL, surface_new, NULL);
    }
    return surface_new;
}

SDL_Renderer * SDL_CreateRenderer(SDL_Window *window, int index, uint32_t flags)
{
    SDL_Renderer *renderer;
    VSF_SDL2_ASSERT(window != NULL);
    renderer = vsf_heap_malloc(sizeof(struct SDL_Renderer));
    if (renderer != NULL) {
        renderer->window    = window;
        renderer->flags     = flags;
    }
    return renderer;
}

void SDL_DestroyRenderer(SDL_Renderer *renderer)
{
    VSF_SDL2_ASSERT(renderer != NULL);
    vsf_heap_free(renderer);
}

int SDL_RenderClear(SDL_Renderer *renderer)
{
    VSF_SDL2_ASSERT(renderer != NULL);
    SDL_Window *window = renderer->window;
    uint_fast8_t pixel_size = vsf_disp_get_pixel_format_bytesize(window->format);

    // TODO: set to default color instead of 0
    memset(window->pixels, 0, pixel_size * window->area.w * window->area.h);
    return 0;
}

int SDL_RenderCopy(SDL_Renderer *renderer, SDL_Texture *texture, const SDL_Rect *srcrect, const SDL_Rect *dstrect)
{
    VSF_SDL2_ASSERT((renderer != NULL) && (texture != NULL));

    SDL_Window *window = renderer->window;
    SDL_Rect src_area, dst_area;
    SDL_Rect texture_area = {
        .w = texture->w,
        .h = texture->h,
    };

    if (srcrect != NULL) {
        if (!__vsf_sdl2_rect_intersect(&src_area, srcrect, &texture_area)) {
            return -1;
        }
    } else {
        src_area = texture_area;
    }
    if (dstrect != NULL) {
        if (!__vsf_sdl2_rect_intersect(&dst_area, dstrect, &window->area)) {
            return -1;
        }
    } else {
        dst_area = window->area;
    }
    dst_area.w = src_area.w = min(dst_area.w, src_area.w);
    dst_area.h = src_area.h = min(dst_area.h, src_area.h);

    uint_fast8_t dst_pixel_size = vsf_disp_get_pixel_format_bytesize(renderer->window->format);
    uint_fast8_t src_pixel_size = vsf_disp_get_pixel_format_bytesize(texture->format->format);
    SDL_PixelFormat *dst_fmt = (SDL_PixelFormat *)__SDL_GetFormatFromColor(renderer->window->format);
    __SDL_BlendWithFormat(src_area.h, src_area.w,
                (uint8_t *)window->pixels + dst_pixel_size * (dst_area.y * window->area.w + dst_area.x), dst_pixel_size * window->area.w,
                (uint8_t *)texture->pixels + src_pixel_size * (src_area.y * texture->w + src_area.x), src_pixel_size * texture->w,
                dst_fmt, texture->format);
    return 0;
}

void SDL_RenderPresent(SDL_Renderer *renderer)
{
    vk_disp_area_t area = {
        .pos.x          = 0,
        .pos.y          = 0,
        .size.x         = renderer->window->area.w,
        .size.y         = renderer->window->area.h,
    };
    __vsf_sdl2_disp_refresh(&area, renderer->window->pixels);
}


SDL_Texture * SDL_CreateTexture(SDL_Renderer *renderer, uint32_t format, int access, int w, int h)
{
    uint_fast8_t pixel_size = vsf_disp_get_pixel_format_bytesize(format);
    SDL_Texture *texture = vsf_heap_malloc(sizeof(struct SDL_Renderer) + pixel_size * w * h);
    if (texture != NULL) {
        texture->format = (SDL_PixelFormat *)__SDL_GetFormatFromColor(format);
        VSF_SDL2_ASSERT(texture->format != NULL);
        texture->w      = w;
        texture->h      = h;
        texture->pixels = &texture->__pixels;
        // TODO: initialize pixels according to format
    }
    return texture;
}

SDL_Texture * SDL_CreateTextureFromSurface(SDL_Renderer *renderer, SDL_Surface *surface)
{
    SDL_Texture *texture = vsf_heap_malloc(sizeof(struct SDL_Renderer));
    if (texture != NULL) {
        texture->format = surface->format;
        texture->w      = surface->w;
        texture->h      = surface->h;
        texture->pixels = surface->pixels;
        // TODO: initialize pixels according to format
    }
    return texture;
}

void SDL_DestroyTexture(SDL_Texture *texture)
{
    VSF_SDL2_ASSERT(texture != NULL);
    vsf_heap_free(texture);
}

int SDL_UpdateTexture(SDL_Texture *texture, const SDL_Rect *rect, const void *pixels, int pitch)
{
    SDL_Rect area;
    VSF_SDL2_ASSERT(texture != NULL);

    if (rect != NULL) {
        area    = *rect;
    } else {
        area.x  = 0;
        area.y  = 0;
        area.w  = texture->w;
        area.h  = texture->h;
    }

    uint_fast8_t pixel_size = vsf_disp_get_pixel_format_bytesize(texture->format->format);
    vsf_sdl2_pixel_copy(area.h, pixel_size * area.w,
                (uint8_t *)texture->pixels + pixel_size * (area.y * texture->w + area.x), pixel_size * texture->w,
                (uint8_t *)pixels, pitch);
    return 0;
}



uint32_t SDL_MapRGB(const SDL_PixelFormat * format, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t color =    ((r >> format->Rloss) << format->Rshift)
                    |   ((g >> format->Gloss) << format->Gshift)
                    |   ((b >> format->Bloss) << format->Bshift);
    return color;
}

uint32_t SDL_MapRGBA(const SDL_PixelFormat * format, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    uint32_t color = SDL_MapRGB(format, r, g, b);
    if (format->Amask != 0) {
        color |= (a >> format->Aloss) << format->Ashift;
    }
    return color;
}




SDL_sem * SDL_CreateSemaphore(uint32_t initial_value)
{
    return NULL;
}
void SDL_DestroySemaphore(SDL_sem *sem)
{
}
int __vsf_sdl2_sem_wait(SDL_sem *sem, int32_t ms)
{
    return -1;
}
int SDL_SemPost(SDL_sem *sem)
{
    return -1;
}

// timer
#if VSF_KERNEL_CFG_EDA_SUPPORT_TIMER == ENABLED
void SDL_Delay(uint32_t ms)
{
    if (ms > 0) {
        vsf_teda_set_timer_ms(ms);
        vsf_thread_wfe(VSF_EVT_TIMER);
    }
}
uint32_t SDL_GetTicks(void)
{
    uint32_t cur_ms = vsf_systimer_get_ms();
    return cur_ms - __vsf_sdl2.start_ms;
}
#endif

int SDL_GetDisplayDPI(int displayIndex, float *ddpi, float *hdpi, float *vdpi)
{
    if (ddpi != NULL) {
        *ddpi = 1.0;
    }
    if (hdpi != NULL) {
        *hdpi = 1.0;
    }
    if (vdpi != NULL) {
        *vdpi = 1.0;
    }
    return 0;
}

void SDL_EnableScreenSaver(void)
{
}

SDL_bool SDL_HasClipboardText(void)
{
    return (NULL == __vsf_sdl2.clipboard.text) ? SDL_FALSE : SDL_TRUE;
}

int SDL_SetClipboardText(const char * text)
{
    if (__vsf_sdl2.clipboard.text != NULL) {
        SDL_free(__vsf_sdl2.clipboard.text);
        __vsf_sdl2.clipboard.text = NULL;
    }
    __vsf_sdl2.clipboard.text = SDL_malloc(strlen(text) + 1);
    if (__vsf_sdl2.clipboard.text != NULL) {
        strcpy(__vsf_sdl2.clipboard.text, text);
        return 0;
    }
    return -1;
}

char * SDL_GetClipboardText(void)
{
    return __vsf_sdl2.clipboard.text;
}

uint64_t SDL_GetPerformanceCounter(void)
{
    return vsf_systimer_get_tick();
}

uint64_t SDL_GetPerformanceFrequency(void)
{
    extern uint_fast32_t vsf_arch_req___systimer_resolution___from_usr(void);
    return vsf_arch_req___systimer_resolution___from_usr();
}

// audio
int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained)
{
    return -1;
}
void SDL_PauseAudio(int pause_on)
{
}
SDL_AudioStatus SDL_GetAudioStatus(void)
{
    return SDL_AUDIO_STOPPED;
}
void SDL_CloseAudio(void)
{
}

// cursor
SDL_Cursor * SDL_CreateCursor(const uint8_t *data, const uint8_t *mask,
                                            int w, int h, int hot_x, int hot_y)
{
    return NULL;
}
SDL_Cursor * SDL_CreateColorCursor(SDL_Surface *surface, int hot_x, int hot_y)
{
    return NULL;
}
SDL_Cursor * SDL_CreateSystemCursor(SDL_SystemCursor id)
{
    return NULL;
}
void SDL_SetCursor(SDL_Cursor *cursor)
{
}
SDL_Cursor * SDL_GetCursor(void)
{
    return NULL;
}
SDL_Cursor * SDL_GetDefaultCursor(void)
{
    return NULL;
}
void SDL_FreeCursor(SDL_Cursor *cursor)
{
}
int SDL_ShowCursor(int toggle)
{
    return -1;
}

#if VSF_SDL_CFG_V1_COMPATIBLE == ENABLED
// overlay
SDL_Overlay * SDL_CreateYUVOverlay(int width, int height, uint32_t format, SDL_Surface *display)
{
    return NULL;
}
void SDL_FreeYUVOverlay(SDL_Overlay *overlay)
{
}
int SDL_LockYUVOverlay(SDL_Overlay *overlay)
{
    return 0;
}
void SDL_UnlockYUVOverlay(SDL_Overlay *overlay)
{
}
int SDL_DisplayYUVOverlay(SDL_Overlay *overlay, SDL_Rect *dstrect)
{
    return -1;
}

SDL_Surface * SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags)
{
    SDL_Surface *surface = NULL;

    if (__vsf_sdl2.sdl1_screen != NULL) {
        vsf_heap_free(__vsf_sdl2.sdl1_screen);
        __vsf_sdl2.sdl1_screen = NULL;
    }

    int bytes_per_pixel = vsf_disp_get_pixel_bytesize(__vsf_sdl2.disp);
    int bits_per_pixel = vsf_disp_get_pixel_bitsize(__vsf_sdl2.disp);
    if (bpp != 0) {
        VSF_SDL2_ASSERT(bpp == bits_per_pixel);
    }

    surface = vsf_heap_malloc(sizeof(SDL_Surface) + (width * height * bytes_per_pixel));
    if (surface != NULL) {
        surface->w = width;
        surface->h = height;
        surface->pitch = width * bytes_per_pixel;

        surface->__format.BitsPerPixel = bits_per_pixel;
        surface->__format.BytesPerPixel = bytes_per_pixel;
        switch (vsf_disp_get_pixel_format(__vsf_sdl2.disp)) {
        case VSF_DISP_COLOR_RGB565:
            surface->__format.Rmask = ((1UL << 5) - 1) << 0;
            surface->__format.Gmask = ((1UL << 6) - 1) << 5;
            surface->__format.Bmask = ((1UL << 5) - 1) << 11;
            surface->__format.Amask = 0;
            break;
        case VSF_DISP_COLOR_ARGB8888:
            surface->__format.Rmask = ((1UL << 8) - 1) << 0;
            surface->__format.Gmask = ((1UL << 8) - 1) << 8;
            surface->__format.Bmask = ((1UL << 8) - 1) << 16;
            surface->__format.Amask = ((1UL << 8) - 1) << 24;
            break;
        default:
            VSF_SDL2_ASSERT(false);
            vsf_heap_free(surface);
            return NULL;
        }
        surface->format = &surface->__format;
    }
    __vsf_sdl2.sdl1_screen = surface;
    return surface;
}
int SDL_Flip(SDL_Surface *screen)
{
    vk_disp_area_t area = {
        .pos.x  = 0,
        .pos.y  = 0,
        .size.x = screen->w,
        .size.y = screen->h,
    };
    __vsf_sdl2_disp_refresh(&area, screen->pixels);
    return 0;
}

void SDL_WM_SetCaption(const char *title, const char *icon)
{
}
#endif


// event
// joysticks
int SDL_NumJoysticks(void)
{
    return 0;
}
SDL_Joystick * SDL_JoystickOpen(int device_index)
{
    return NULL;
}
int SDL_JoystickEventState(int state)
{
    return -1;
}
int SDL_JoystickNumButtons(SDL_Joystick *joystick)
{
    return 0;
}
int SDL_JoystickNumAxes(SDL_Joystick *joystick)
{
    return 0;
}
int SDL_JoystickNumBalls(SDL_Joystick *joystick)
{
    return 0;
}
int SDL_JoystickNumHats(SDL_Joystick *joystick)
{
    return 0;
}

int SDL_CaptureMouse(SDL_bool enabled)
{
    return 0;
}

uint32_t SDL_GetGlobalMouseState(int * x, int * y)
{
    // TODO: get mouse position and button state
    if (x != NULL) {
        *x = 0;
    }
    if (y != NULL) {
        *y = 0;
    }
    return 0;
}

int SDL_PollEvent(SDL_Event *event)
{
    VSF_SDL2_ASSERT(event != NULL);


    return 0;
}

int SDL_WaitEventTimeout(SDL_Event * event, int timeout)
{
    // todo:
    vsf_thread_wfe(VSF_EVT_RETURN);
    return 0;
}

void SDL_FlushEvent(uint32_t type)
{

}

uint8_t SDL_EventState(uint32_t type, int state)
{
    // TODO: do any thing man
    return SDL_DISABLE;
}

const char * SDL_GetKeyName(SDL_Keycode key)
{
    return "unknown";
}

#endif      // VSF_USE_SDL2
