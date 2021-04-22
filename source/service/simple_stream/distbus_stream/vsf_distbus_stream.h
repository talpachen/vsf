/***************************************************************************
 *   Copyright (C) 2009 - 2010 by Simon Qian <SimonQian@SimonQian.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __VSF_DISTBUS_STREAM_H__
#define __VSF_DISTBUS_STREAM_H__

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"

#if VSF_USE_SIMPLE_STREAM == ENABLED && VSF_USE_DISTBUS == ENABLED

#include "service/distbus/vsf_distbus.h"

#if     defined(__VSF_DISTBUS_STREAM_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#elif   defined(__VSF_DISTBUS_STREAM_CLASS_INHERIT__)
#   define __PLOOC_CLASS_INHERIT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#define __VSF_DISTBUS_STREAM_INIT(__NAME, __DISTBUS, __ADDR, __MTU, __IS_TX, __HANDLER)\
            .op                 = &vsf_distbus_stream_op,                       \
            .is_tx              = (__IS_TX),                                    \
            .distbus            = (__DISTBUS),                                  \
            .addr               = (__ADDR),                                     \
            .mtu                = (__MTU),                                      \
            .addr_range         = VSF_DISTBUS_STREAM_ADDR_RANGE,                \
            .handler            = (__HANDLER),                                  \
            .info               = (const vsf_distbus_service_info_t *)          \
                                    &(__NAME).use_as__vsf_distbus_service_info_t,
#define VSF_DISTBUS_STREAM_INIT(__NAME, __DISTBUS, __ADDR, __MTU, __IS_TX, __HANDLER)\
            __VSF_DISTBUS_STREAM_INIT(__NAME, (__DISTBUS), (__ADDR), (__MTU), (__IS_TX), (__HANDLER))

#define __vsf_distbus_stream_type(__name)   __name##_distbus_stream_t
#define vsf_distbus_stream_type(__name)     __vsf_distbus_stream_type(__name)

#define __declare_distbus_stream(__name)                                        \
            dcl_simple_class(vsf_distbus_stream_type(__name))

#define __define_distbus_stream(__name)                                         \
            def_simple_class(vsf_distbus_stream_type(__name)) {                 \
                public_member(                                                  \
                    implement(vsf_distbus_stream_t)                             \
                )                                                               \
            };

#define declare_distbus_stream(__name)                                          \
            __declare_distbus_stream(__name)
#define dcl_distbus_stream(__name)                                              \
            declare_distbus_stream(__name)
#define define_distbus_stream(__name)                                           \
            __define_distbus_stream(__name)
#define def_distbus_stream(__name)                                              \
            define_distbus_stream(__name)

#define __describe_distbus_stream(__name, __distbus, __addr, __mtu, __is_tx, __handler)\
            declare_distbus_stream(__name)                                      \
            define_distbus_stream(__name, (__size))                             \
            vsf_distbus_stream_type(__name) __name = {                          \
                VSF_DISTBUS_STREAM_INIT(__name, (__distbus), (__addr), (__mtu), (__is_tx), (__handler))\
            };

#define describe_distbus_stream(__name, __distbus, __addr, __mtu, __is_tx, __handler)\
            __describe_distbus_stream(__name, (__distbus), (__addr), (__mtu), (__is_tx), (__handler))

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

enum {
    VSF_DISTBUS_STREAM_CMD_CONNECT,
    VSF_DISTBUS_STREAM_CMD_DATA,
    VSF_DISTBUS_STREAM_ADDR_RANGE,
};

dcl_simple_class(vsf_distbus_stream_t)

def_simple_class(vsf_distbus_stream_t) {
    public_member(
        implement(vsf_stream_t)
        implement(vsf_distbus_service_info_t)
        implement(vsf_distbus_service_t)
        vsf_distbus_t *distbus;
        bool is_tx;
    )
    private_member(
        union {
            struct {
                vsf_distbus_msg_t *msg;
            };
            struct {
                vsf_slist_queue_t msgq;
            };
        };
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_stream_op_t vsf_distbus_stream_op;

/*============================ PROTOTYPES ====================================*/

#if     defined(__VSF_DISTBUS_STREAM_CLASS_IMPLEMENT)                           \
    ||  defined(__VSF_DISTBUS_STREAM_CLASS_INHERIT__)
bool __vsf_distbus_stream_msghandler(vsf_distbus_t *bus,
                        vsf_distbus_service_t *service, vsf_distbus_msg_t *msg);
#endif

#ifdef __cplusplus
}
#endif

#undef __VSF_DISTBUS_STREAM_CLASS_IMPLEMENT
#undef __VSF_DISTBUS_STREAM_CLASS_INHERIT__

#endif      // VSF_USE_SIMPLE_STREAM && VSF_USE_DISTBUS
#endif      // __VSF_DISTBUS_STREAM_H__
