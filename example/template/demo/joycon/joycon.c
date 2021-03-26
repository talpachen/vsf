/*****************************************************************************
 *   Copyright(C);2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");;          *
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
#include "./vsf.h"

/*============================ INCLUDES ======================================*/

#define __VSF_EDA_CLASS_INHERIT__


#include "./joycon.h"


#define SYS_GPB_MFPL_PB3MFP_UART1_TXD         (0x06UL<<SYS_GPB_MFPL_PB3MFP_Pos)  /*!< Data transmitter output pin for UART1. \hideinitializer */
#define SYS_GPB_MFPL_PB2MFP_UART1_RXD         (0x06UL<<SYS_GPB_MFPL_PB2MFP_Pos)  /*!< Data receiver input pin for UART1. \hideinitializer */

/*============================ MACROS ========================================*/

#define VSF_USART_CFG_INVALID_VALUE                                 255

#ifndef VSF_USART_CFG_REC_DATA_SIZE
#   define VSF_USART_CFG_REC_DATA_SIZE                              20
#endif // !VSF_USART_CFG_REC_DATA_SIZE

#ifndef VSF_USART_CFG_SEND_DATA_SIZE
#   define VSF_USART_CFG_SEND_DATA_SIZE                             20
#endif // !VSF_USART_CFG_REC_DATA_SIZE

#define joycon_cpy                                                  memcpy
/*============================ MACROFIED FUNCTIONS ===========================*/

#define console_to_joycon_data(__type)                                          \
    __console_to_joycon_data[(__type) - VSF_EVT_USER]
        
#define send_data_size(__type)                                                  \
    __console_to_joycon_size[(__type) - VSF_EVT_USER]
#define rcv_data_size(__type)                                                   \
        ___joycon_to_console_size[(__type) - VSF_EVT_USER]
/*============================ TYPES =========================================*/

typedef enum em_joycom_usart_evt_t {
    JOYCON_START_1M                = VSF_EVT_USER,
    JOYCON_GET_MAC_1M,
    JOYCON_DONT_KNOW_1M,
    JOYCON_SWITCH_TO_3_125M,
    JOYCON_DONT_KNOW_1_3_125M,
    JOYCON_DONT_KNOW_2_3_125M,
    JOYCON_REQUEST_STATUS_3_125M,
    
    JOYCON_MESSAGE_SEND,
    JOYCON_MESSAGE_RCV,
} em_joycom_usart_evt_t;
typedef struct user_usart_t {
    vsf_usart_t                     *usart;
    uint16_t                        rcv_type;//use em_joycom_usart_evt_t 's value
    char                            rcv_data[128];// keep last char alway is 0u
    uint8_t                         rcv_size;
    uint8_t                         send_idx;
    bool                            send_is_busy;
    vsf_teda_t                      teda;
}user_usart_t;
/*============================ GLOBAL VARIABLES ==============================*/
ROOT uint_fast32_t time_root[20][4];
user_usart_t __useapp_usart = {
    .usart = &vsf_usart1,
    .rcv_size = 0,
    .send_is_busy = false,
};
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

void __isr_handle(void *target_ptr, vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask);
/*============================ IMPLEMENTATION ================================*/
//void printf_time(void)
//{
//    static char i = 0;
//    time_root[i][0] = rcu_clock_freq_get(CK_SYS);
//    time_root[i][1] = rcu_clock_freq_get(CK_AHB);
//    time_root[i][2] = rcu_clock_freq_get(CK_APB1);
//    time_root[i++][3] = rcu_clock_freq_get(CK_APB2);
//
//
//}

static uint8_t console_to_joycon_0[] = {
    0xA1, 0xA2, 0xA3, 0xA4, 0x19, 0x01, 0x03, 0x07, 0x00, 0xA5, 0x02, 0x01, 0x7E, 0x00, 0x00, 0x00,
};
static uint8_t console_to_joycon_1[] = {
    0x19, 0x01, 0x03, 0x07, 0x00, 0x91, 0x01, 0x00, 0x00, 0x00, 0x00, 0x24,
};
static uint8_t console_to_joycon_2[] = {
    0x19, 0x01, 0x03, 0x0B, 0x00, 0x91, 0x02, 0x04, 0x00, 0x00, 0x38, 0xE6, 0x14, 0x00, 0x00, 0x00,
};
static uint8_t console_to_joycon_3[] = {
    0x19, 0x01, 0x03, 0x0F, 0x00, 0x91, 0x20, 0x08, 0x00, 0x00, 0xBD, 0xB1, 0xC0, 0xC6, 0x2D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
static uint8_t console_to_joycon_4[] = {
    0x19, 0x01, 0x03, 0x07, 0x00, 0x91, 0x11, 0x00, 0x00, 0x00, 0x00, 0x0e,
};
static uint8_t console_to_joycon_5[] = {
    0x19, 0x01, 0x03, 0x07, 0x00, 0x91, 0x10, 0x00, 0x00, 0x00, 0x00, 0x3d,
};
static uint8_t console_to_joycon_6[] = {
    0x19, 0x01, 0x03, 0x0b, 0x00, 0x91, 0x12, 0x04, 0x00, 0x00, 0x12, 0xa6, 0x0f, 0x00, 0x00, 0x00,
};
static uint8_t console_to_joycon_7[] = {
    0x19, 0x01, 0x03, 0x08, 0x00, 0x92, 0x00, 0x01, 0x00, 0x00, 0x69, 0x2d, 0x1f
};
static uint8_t *__console_to_joycon_data[] = {
    (uint8_t *)console_to_joycon_0, 
    (uint8_t *)console_to_joycon_1,
    (uint8_t *)console_to_joycon_2, 
    (uint8_t *)console_to_joycon_3,
    (uint8_t *)console_to_joycon_4, 
    (uint8_t *)console_to_joycon_5,
    (uint8_t *)console_to_joycon_6,
    (uint8_t *)console_to_joycon_7,
};
static uint8_t __console_to_joycon_size[] = {
    16, 12, 16, 21, 12, 12, 16, 13
};
static uint8_t ___joycon_to_console_size[] = {
    1,  1,  1,  1,  1,  1,  1,  1
};
static void vsf_usart_interrupt_enable(user_usart_t *user_usart)
{
//    nvic_irq_enable(USART0_IRQn, 0, 0);
//    usart_interrupt_enable(user_usart->usart->port_address, USART_INT_TBE);
//    usart_interrupt_enable(user_usart->usart->port_address, USART_INT_RBNE);
}

void vsf_post_joycon_usart_evt(user_usart_t *user_usart, em_usart_irq_mask_t evt)
{
    vsf_eda_post_evt((vsf_eda_t *)&user_usart->teda, (vsf_evt_t)evt);
}

static void __switch_to_3125000_bps(user_usart_t *user_usart)
{
    vsf_usart_t *usart = user_usart->usart;
    usart->cfg.baudrate = 3125000;
    vsf_usart_disable(usart);
    vsf_usart_init(usart, &usart->cfg);
    vsf_usart_enable(usart);
}

static void __vk_joycon_usart_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    user_usart_t *user_usart = container_of(eda, user_usart_t, teda);
    vsf_usart_t *usart = user_usart->usart;
    vsf_err_t err;
    fsm_rt_t fsm;
    switch (evt) {
    case VSF_EVT_INIT: {
        usart_cfg_t  cfg = {
            .mode =    USART_8_BIT_LENGTH | USART_1_STOPBIT | USART_NONE_PARITY | USART_4_BIT_FIFO | USART_TX_INVERTED,
            .baudrate = 1000000,
            .rx_timeout = 0,
            .isr = {
                .handler_fn = __isr_handle,
            },
        };
        err = vsf_usart_init(usart, &cfg);
        //
        SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk);
        SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB2MFP_UART1_RXD | SYS_GPB_MFPL_PB3MFP_UART1_TXD);
        //
        fsm = vsf_usart_enable(usart);
        if (VSF_ERR_NONE != err || fsm_rt_cpl != fsm) {
            goto error_handling;
        }
        user_usart->rcv_type = JOYCON_START_1M;
        usart->tx_buf = (void *)console_to_joycon_data(user_usart->rcv_type);
        user_usart->send_idx = 0;
    }
    case VSF_EVT_TIMER:
        vsf_trace_info("RCV_TYPE(%d)send_idx(%d)\n", user_usart->rcv_type, user_usart->send_idx);
    case JOYCON_MESSAGE_SEND:
        switch (user_usart->rcv_type) {
        case JOYCON_SWITCH_TO_3_125M:
            if (usart->cfg.baudrate != 3125000) {
                __switch_to_3125000_bps(user_usart);
                vsf_trace_info("__switch_to_3125000_bps\n");
                vsf_teda_cancel_timer();
                vsf_teda_set_timer_ms(200);
                break;
            }
        case JOYCON_START_1M:
        case JOYCON_GET_MAC_1M:
        case JOYCON_DONT_KNOW_1M:
        case JOYCON_DONT_KNOW_1_3_125M:
        case JOYCON_DONT_KNOW_2_3_125M:
        case JOYCON_REQUEST_STATUS_3_125M:
            if (user_usart->send_idx < send_data_size(user_usart->rcv_type)) {
                user_usart->send_idx += vsf_usart_fifo_write(user_usart->usart, // TX irq
                                                             (void *)((uint8_t *)(usart->tx_buf) + user_usart->send_idx),
                                                             send_data_size(user_usart->rcv_type) - user_usart->send_idx);
                vsf_trace_info("enabled TX(%d) %d\n", user_usart->send_idx, __LINE__);
                vsf_usart_irq_enable(usart, USART_IRQ_MASK_TX);
            } else {
                user_usart->send_is_busy = false;
                user_usart->send_idx = VSF_USART_CFG_INVALID_VALUE;
                vsf_trace_info("enabled RX %d\n", __LINE__);
                vsf_usart_irq_enable(usart, USART_IRQ_MASK_RX);                 // start wait RX irq
            }
            break;
        }
        break;
    case JOYCON_MESSAGE_RCV:
        vsf_trace_info("JOYCON__RCV %d\n", __LINE__);
//        if (user_usart->rcv_size <= rcv_data_size(user_usart->rcv_type)) {
//            vsf_usart_irq_enable(usart, USART_IRQ_MASK_RX);
//            user_usart->rcv_size += vsf_usart_fifo_read(user_usart->usart, user_usart->rcv_data, rcv_data_size(user_usart->rcv_type));
//            break;
//        }
        if (!user_usart->send_is_busy) {
            user_usart->rcv_type += 1;
            if (user_usart->rcv_type > JOYCON_REQUEST_STATUS_3_125M) {
                user_usart->rcv_type = JOYCON_REQUEST_STATUS_3_125M;
            }
            usart->tx_buf = (void *)console_to_joycon_data(user_usart->rcv_type);
            user_usart->send_idx = 0;
            vsf_teda_cancel_timer();
            vsf_teda_set_timer_ms(5);
        }
    default:
error_handling:
        //TODO:
    break;

    }
}

vsf_err_t vk_usart_init(user_usart_t *user_usart)
{
    user_usart->teda.use_as__vsf_eda_t.fn.evthandler = __vk_joycon_usart_evthandler;
    return vsf_teda_init(&user_usart->teda, VSF_USART_CFG_EDA_PRIORITY, false);
}

int default_usart_main(void)
{
    if (VSF_ERR_NONE != vk_usart_init(&__useapp_usart)) {
        return -1;
    }
    return 0;
}

void __isr_handle(void *target_ptr, vsf_usart_t *usart_ptr, em_usart_irq_mask_t irq_mask)
{
    if (irq_mask & USART_IRQ_MASK_RX) {
        vsf_eda_post_evt((vsf_eda_t *)&__useapp_usart.teda, JOYCON_MESSAGE_RCV);
        vsf_usart_irq_disable(usart_ptr, USART_IRQ_MASK);
        vsf_trace_info("disabled RX %d\n", __LINE__);
    }
    if (irq_mask & USART_IRQ_MASK_TX) {
        vsf_eda_post_evt((vsf_eda_t *)&__useapp_usart.teda, JOYCON_MESSAGE_SEND);
        vsf_usart_irq_disable(usart_ptr, USART_IRQ_MASK);
        vsf_trace_info("disabled TX %d\n", __LINE__);
    }
}

