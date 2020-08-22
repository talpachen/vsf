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


//! \note User Level Application Configuration

#ifndef __VSF_USR_CFG_M484_H__
#define __VSF_USR_CFG_M484_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

// Application configure
#define APP_CFG_USE_LINUX_DEMO                          ENABLED
#   define APP_CFG_USE_LINUX_LIBUSB_DEMO                ENABLED
#   define APP_CFG_USE_LINUX_MOUNT_FILE_DEMO            ENABLED
#define APP_CFG_USE_USBH_DEMO                           ENABLED
#define APP_CFG_USE_USBD_DEMO                           ENABLED
#   define APP_CFG_USE_USBD_CDC_DEMO                    ENABLED
#   define APP_CFG_USE_USBD_MSC_DEMO                    ENABLED
#   define APP_CFG_USE_USBD_UVC_DEMO                    ENABLED
#define APP_CFG_USE_SCSI_DEMO                           ENABLED
//  todo: implement audio driver for m484
#define APP_CFG_USE_AUDIO_DEMO                          DISABLED
//  current tgui demo depends on VSF_USE_DISP_SDL2, which is only available on __WIN__
#define APP_CFG_USE_TGUI_DEMO                           DISABLED
//  current M484 hardware has no display
#define APP_CFG_USE_SDL2_DEMO                           DISABLED
//  TODO: need test for c++ support
#define APP_CFG_USE_CPP_DEMO                            ENABLED

// 3rd-party demos
//  awtk is LGPL, not convenient to implement in MCU
#define APP_CFG_USE_AWTK_DEMO                           DISABLED
#define APP_CFG_USE_NNOM_DEMO                           ENABLED
//  current M484 hardware has no display
#define APP_CFG_USE_LVGL_DEMO                           DISABLED
#define APP_CFG_USE_BTSTACK_DEMO                        ENABLED
#define APP_CFG_USE_VSFVM_DEMO                          DISABLED
// select one for tcpip stack
#define APP_CFG_USE_VSFIP_DEMO                          DISABLED
#define APP_CFG_USE_LWIP_DEMO                           ENABLED


// component configure
#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED
#   define VSF_HEAP_SIZE                                0x10000
#   define VSF_SYSTIMER_FREQ                            (192000000ul)

#define VSF_USE_VIDEO                                   ENABLED
#define VSF_USE_AUDIO                                   ENABLED
#   define VSF_USE_DECODER_WAV                          ENABLED
#   define VSF_AUDIO_CFG_USE_PLAY                       ENABLED
#   define VSF_AUDIO_CFG_USE_CATURE                     DISABLED

// UI runs in vsf_prio_0, other modules runs above vsf_prio_1
#if APP_CFG_USE_AWTK_DEMO == ENABLED || APP_CFG_USE_LVGL_DEMO == ENABLED || APP_CFG_USE_XBOOT_XUI_DEMO == ENABLED || APP_CFG_USE_TGUI_DEMO == ENABLED
#   define VSF_USBH_CFG_EDA_PRIORITY                    vsf_prio_1
#   define APP_CFG_USBH_HW_PRIO                         vsf_arch_prio_1
#endif

#define VSF_USE_INPUT                                   ENABLED
#   define VSF_USE_INPUT_HID                            ENABLED
#   define VSF_USE_INPUT_DS4                            ENABLED
#   define VSF_USE_INPUT_NSPRO                          ENABLED
#   define VSF_USE_INPUT_XB360                          ENABLED
#   define VSF_USE_INPUT_XB1                            ENABLED

// VSF_USE_USB_DEVICE will be enabled if target chip supports USBD
//#define VSF_USE_USB_DEVICE                              ENABLED
#   define VSF_USBD_CFG_USE_EDA                         ENABLED
#   define VSF_USE_USB_DEVICE_CDCACM                    ENABLED
#   define VSF_USE_USB_DEVICE_MSC                       ENABLED
#   define VSF_USE_USB_DEVICE_UVC                       ENABLED
#   define APP_CFG_USBD_VID                             0xA7A8
#   define APP_CFG_USBD_PID                             0x2348

#define VSF_USE_MAL                                     ENABLED
#   define VSF_USE_MEM_MAL                              ENABLED
#   define VSF_USE_FAKEFAT32_MAL                        ENABLED
#   define VSF_USE_SCSI_MAL                             ENABLED
#   define VSF_USE_FILE_MAL                             ENABLED

#define VSF_USE_SCSI                                    ENABLED
#   define VSF_USE_MAL_SCSI                             ENABLED

#define VSF_USE_FS                                      ENABLED
#   define VSF_USE_MEMFS                                ENABLED
#   define VSF_USE_FATFS                                ENABLED

#define VSF_USE_TRACE                                   ENABLED
#define USRAPP_CFG_STDIO_EN                             ENABLED

#define VSF_USE_LINUX                                   ENABLED
#   define VSF_USE_LINUX_LIBUSB                         VSF_USE_USB_HOST
#   define VSF_USE_LINUX_BUSYBOX                        ENABLED

#ifndef USRAPP_CFG_LINUX_TTY_DEBUT_STREAM
#   define USRAPP_CFG_LINUX_TTY_DEBUG_STREAM            0
#   define USRAPP_CFG_LINUX_TTY_UART                    1
#   define USRAPP_CFG_LINUX_TTY_CDC                     2
#endif
#   define USRAPP_CFG_LINUX_TTY                         USRAPP_CFG_LINUX_TTY_DEBUG_STREAM

#define VSF_USE_SERVICE_STREAM                          DISABLED
#define VSF_USE_SERVICE_VSFSTREAM                       ENABLED


#ifndef USRAPP_CFG_DCD_TYPE_DEFAULT
#   define USRAPP_CFG_DCD_TYPE_DEFAULT                  0
#   define USRAPP_CFG_DCD_TYPE_DWCOTG                   1
#   define USRAPP_CFG_DCD_TYPE_MUSB_FDRC                2
#endif

#define USRAPP_CFG_FAKEFAT32                            ENABLED

#define ASSERT(...)                                     if (!(__VA_ARGS__)) {while(1);};
//#define ASSERT(...)

#define VSF_DEBUGGER_CFG_CONSOLE                        VSF_DEBUGGER_CFG_CONSOLE_NULINK_NUCONSOLE

#if APP_CFG_USE_USBD_DEMO == ENABLED
#   define VSF_USE_USB_DEVICE                           ENABLED
#       define VSF_USBD_CFG_EDA_PRIORITY                vsf_prio_1
#       define VSF_USBD_CFG_HW_PRIORITY                 vsf_arch_prio_1
#       define USRAPP_CFG_USBD_SPEED                    USB_SPEED_HIGH
#       define USRAPP_CFG_CDC_NUM                       1
#       define USRAPP_CFG_CDC_TX_STREAM_SIZE            1024
#       define USRAPP_CFG_CDC_RX_STREAM_SIZE            512
#       define USRAPP_CFG_DCD_TYPE                      USRAPP_CFG_DCD_TYPE_DEFAULT
#       define USRAPP_CFG_STREAM_ALIGN                  1
#endif

#define VSF_USBH_CFG_ENABLE_ROOT_HUB                    ENABLED
#define VSF_USE_USB_HOST_HUB                            ENABLED
#define VSF_USE_USB_HOST_HCD_OHCI                       ENABLED

#define USRAPP_CFG_USBD_DEV                             VSF_USB_DC0

#define VSF_LINUX_CFG_STACKSIZE                         2048
#define VSF_TRACE_CFG_COLOR_EN                          DISABLED
#define VSH_HAS_COLOR                                   0

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USR_CFG_M484_H__
/* EOF */
