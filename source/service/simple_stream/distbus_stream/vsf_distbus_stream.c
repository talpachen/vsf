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
#include "service/vsf_service_cfg.h"

#if VSF_USE_SIMPLE_STREAM == ENABLED && VSF_USE_DISTBUS == ENABLED

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#define __VSF_DISTBUS_CLASS_INHERIT__
#define __VSF_DISTBUS_STREAM_CLASS_IMPLEMENT
#include "../vsf_simple_stream.h"
#include "./vsf_distbus_stream.h"

#include "hal/arch/vsf_arch.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static void __vsf_distbus_stream_init(vsf_stream_t *stream);
static uint_fast32_t __vsf_distbus_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t __vsf_distbus_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t __vsf_distbus_stream_get_buff_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_distbus_stream_get_data_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_distbus_stream_get_avail_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_distbus_stream_get_wbuf(vsf_stream_t *stream, uint8_t **ptr);
static uint_fast32_t __vsf_distbus_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

const vsf_stream_op_t vsf_distbus_stream_op = {
    .init               = __vsf_distbus_stream_init,
    .fini               = __vsf_distbus_stream_init,
    .write              = __vsf_distbus_stream_write,
    .read               = __vsf_distbus_stream_read,
    .get_buff_length    = __vsf_distbus_stream_get_buff_length,
    .get_data_length    = __vsf_distbus_stream_get_data_length,
    .get_avail_length   = __vsf_distbus_stream_get_avail_length,
    .get_wbuf           = __vsf_distbus_stream_get_wbuf,
    .get_rbuf           = __vsf_distbus_stream_get_rbuf,
};

/*============================ IMPLEMENTATION ================================*/

bool __vsf_distbus_stream_msghandler(vsf_distbus_t *bus,
                        vsf_distbus_service_t *service, vsf_distbus_msg_t *msg)
{
    vsf_distbus_stream_t *distbus_stream =
        container_of(service, vsf_distbus_stream_t, use_as__vsf_distbus_service_t);
    bool retain_msg = false;

    switch (msg->header.addr) {
    case VSF_DISTBUS_STREAM_CMD_CONNECT:
        // connect remote side
        if (distbus_stream->is_tx) {
            vsf_stream_connect_rx(&distbus_stream->use_as__vsf_stream_t);
        } else {
            vsf_stream_connect_tx(&distbus_stream->use_as__vsf_stream_t);
        }
        break;
    case VSF_DISTBUS_STREAM_CMD_DATA: {
            VSF_SERVICE_ASSERT(!distbus_stream->is_tx);
            vsf_protect_t orig = vsf_protect_int();
                vsf_slist_queue_enqueue(vsf_distbus_msg_t, node, &distbus_stream->msgq, msg);
            vsf_unprotect_int(orig);
            retain_msg = true;
            __vsf_stream_on_write(&distbus_stream->use_as__vsf_stream_t);
        }
        break;
    default:
        VSF_SERVICE_ASSERT(false);
        break;
    }
    return retain_msg;
}

static void __vsf_distbus_stream_init(vsf_stream_t *stream)
{
    vsf_distbus_stream_t *distbus_stream = (vsf_distbus_stream_t *)stream;
    if (distbus_stream->is_tx) {
        distbus_stream->msg = NULL;
    } else {
        vsf_slist_queue_init(&distbus_stream->msgq);
    }
    if (NULL == distbus_stream->handler) {
        distbus_stream->handler = __vsf_distbus_stream_msghandler;
    }
    vsf_distbus_register_service(distbus_stream->distbus, &distbus_stream->use_as__vsf_distbus_service_t);

    vsf_distbus_msg_t *msg = vsf_distbus_alloc_msg(distbus_stream->distbus, 0, NULL);
    VSF_SERVICE_ASSERT(msg != NULL);
    msg->header.addr = VSF_DISTBUS_STREAM_CMD_CONNECT;
    vsf_distbus_send_msg(distbus_stream->distbus, &distbus_stream->use_as__vsf_distbus_service_t, msg);
}

static uint_fast32_t __vsf_distbus_stream_get_buff_length(vsf_stream_t *stream)
{
    vsf_distbus_stream_t *distbus_stream = (vsf_distbus_stream_t *)stream;
    return distbus_stream->mtu;
}

static uint_fast32_t __vsf_distbus_stream_get_data_length(vsf_stream_t *stream)
{
    vsf_distbus_stream_t *distbus_stream = (vsf_distbus_stream_t *)stream;
    VSF_SERVICE_ASSERT(!distbus_stream->is_tx);
    return distbus_stream->mtu;
}

static uint_fast32_t __vsf_distbus_stream_get_avail_length(vsf_stream_t *stream)
{
    vsf_distbus_stream_t *distbus_stream = (vsf_distbus_stream_t *)stream;
    VSF_SERVICE_ASSERT(distbus_stream->is_tx);
    return distbus_stream->mtu;
}

static uint_fast32_t __vsf_distbus_stream_get_wbuf(vsf_stream_t *stream, uint8_t **ptr)
{
    vsf_distbus_stream_t *distbus_stream = (vsf_distbus_stream_t *)stream;
    VSF_SERVICE_ASSERT(distbus_stream->is_tx);
    VSF_SERVICE_ASSERT(NULL == distbus_stream->msg);
    distbus_stream->msg = vsf_distbus_alloc_msg(distbus_stream->distbus, distbus_stream->mtu, ptr);
    return distbus_stream->mtu;
}

static uint_fast32_t __vsf_distbus_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr)
{
    vsf_distbus_stream_t *distbus_stream = (vsf_distbus_stream_t *)stream;
    VSF_SERVICE_ASSERT(!distbus_stream->is_tx);

    if (vsf_slist_queue_is_empty(&distbus_stream->msgq)) {
        return 0;
    }

    vsf_distbus_msg_t *msg;
    vsf_protect_t orig = vsf_protect_int();
        vsf_slist_queue_peek(vsf_distbus_msg_t, node, &distbus_stream->msgq, msg);
    vsf_unprotect_int(orig);
    if (ptr != NULL) {
        *ptr = (uint8_t *)&msg->header + sizeof(msg->header) + msg->pos;
    }
    return msg->header.datalen - msg->pos;
}

static uint_fast32_t __vsf_distbus_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    vsf_distbus_stream_t *distbus_stream = (vsf_distbus_stream_t *)stream;
    vsf_distbus_msg_t *msg;

    if (buf != NULL) {
        uint8_t *ptr;
        msg = vsf_distbus_alloc_msg(distbus_stream->distbus, size, &ptr);
    } else {
        msg = distbus_stream->msg;
    }
    if (NULL == msg) {
        return 0;
    }

    msg->header.datalen = size;
    vsf_distbus_send_msg(distbus_stream->distbus, &distbus_stream->use_as__vsf_distbus_service_t, msg);
    return size;
}

static uint_fast32_t __vsf_distbus_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    vsf_distbus_stream_t *distbus_stream = (vsf_distbus_stream_t *)stream;
    uint_fast32_t realsize = 0;
    vsf_distbus_msg_t *msg;
    vsf_protect_t orig;

    if (buf != NULL) {
        uint8_t *ptr;
        uint_fast32_t cursize;

        while (realsize < size) {
            cursize = __vsf_distbus_stream_get_rbuf(stream, &ptr);
            if (0 == cursize) {
                break;
            }

            cursize = min(cursize, size - realsize);
            memcpy(buf, ptr, cursize);
            realsize += cursize;
            __vsf_distbus_stream_read(stream, NULL, cursize);
        }
    } else {
        orig = vsf_protect_int();
            vsf_slist_queue_peek(vsf_distbus_msg_t, node, &distbus_stream->msgq, msg);
        vsf_unprotect_int(orig);

        VSF_SERVICE_ASSERT(msg->pos + size <= msg->header.datalen);
        msg->pos += size;
        realsize = size;
        if (msg->pos == msg->header.datalen) {
            orig = vsf_protect_int();
                vsf_slist_queue_dequeue(vsf_distbus_msg_t, node, &distbus_stream->msgq, msg);
            vsf_unprotect_int(orig);

            vsf_distbus_free_msg(distbus_stream->distbus, msg);
        }
    }
    return realsize;
}

#endif      // VSF_USE_SIMPLE_STREAM && VSF_USE_DISTBUS
