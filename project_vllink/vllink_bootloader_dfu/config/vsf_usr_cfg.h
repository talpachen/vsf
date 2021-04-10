//! \note Top Level Application Configuration 

#ifndef __TOP_APP_CFG_H__
#define __TOP_APP_CFG_H__

#if 0   // Debug mode
//  Need set '--no_unaligned_access'
#   define __VSF_DEBUG__
#   define __OOC_DEBUG__
#   define VSF_ASSERT(...)              if (!(__VA_ARGS__)) {while(1);};
#else
#   define __VSF_RELEASE__
//#   define __OOC_RELEASE__
#   define __OOC_DEBUG__
#   define VSF_ASSERT(...)
#endif

#ifdef __GNUC__
#	include "compile_definitions.h"
#endif

#define VSF_CFG_USER_HEADER             "vsf_private.h"
#define VSF_OSA_DRIVER_HEADER           "./driver.h"        // fake include

// software independent components, if not used, compiler will optimize
#define VSF_USE_FIFO                                    ENABLED
#define VSF_USE_JSON                                    DISABLED

#define VSF_OS_CFG_PRIORITY_NUM                         1
#define VSF_OS_CFG_MAIN_MODE                            VSF_OS_CFG_MAIN_MODE_IDLE

#define VSF_KERNEL_CFG_SUPPORT_SYNC                     DISABLED
#   define VSF_KERNEL_CFG_SUPPORT_SYNC_IRQ              DISABLED
#   define VSF_KERNEL_CFG_SUPPORT_BITMAP_EVENT          DISABLED
#   define VSF_KERNEL_CFG_SUPPORT_SLIST_QUEUE           DISABLED
#   define VSF_KERNEL_CFG_SUPPORT_MSG_QUEUE             DISABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_PT                   DISABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_TIMER                DISABLED
#   define VSF_KERNEL_CFG_CALLBACK_TIMER                DISABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL             DISABLED
#define VSF_KERNEL_CFG_EDA_SUPPORT_FSM                  DISABLED

#define VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED     ENABLED
#   define VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY      DISABLED
#   define VSF_OS_CFG_EVTQ_BITSIZE                      4

#define VSF_USE_HEAP                                    DISABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    DISABLED
#   define VSF_HEAP_SIZE                                0x4000

#define VSF_USE_KERNEL_SIMPLE_SHELL                     ENABLED
#define VSF_KERNEL_CFG_SUPPORT_THREAD                   DISABLED

//#define VSF_OS_CFG_EVTQ_POOL_SIZE                     16
//#define VSF_OS_CFG_DEFAULT_TASK_FRAME_POOL_SIZE       16
//#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                   DISABLED
//#define VSF_OS_CFG_MAIN_STACK_SIZE                    2048

#define VSF_USE_USB_DEVICE                              ENABLED
#   define VSF_USBD_CFG_USE_EDA                         DISABLED
#   define VSF_USBD_CFG_HW_PRIORITY                     vsf_arch_prio_0
#   define VSF_USBD_CFG_EDA_PRIORITY                    vsf_prio_0
#   define VSF_USBD_CFG_RAW_MODE                        ENABLED
#   define VSF_USBD_CFG_EP0_ONLY                        ENABLED
#   define VSF_USBD_CFG_STREAM_EN                       DISABLED

#define VSF_USE_TRACE                                   DISABLED
#define VSF_USE_SERVICE_VSFSTREAM                       DISABLED


/*============================ INCLUDES ======================================*/

#include "proj_cfg.h"
#include "brd_cfg.h"

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif
/* EOF */
