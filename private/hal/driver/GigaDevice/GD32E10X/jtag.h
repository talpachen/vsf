#ifndef __HAL_DRIVER_GIGADEVICE_GD32E10X_JTAG_H__
#define __HAL_DRIVER_GIGADEVICE_GD32E10X_JTAG_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#include "__device.h"

/*============================ MACROS ========================================*/

#ifndef JTAG0_ENABLE
#   define JTAG0_ENABLE             0
#endif

#define JTAG_COUNT                  (0 + JTAG0_ENABLE)

#define JTAG_SYNC
//#define JTAG_ASYNC

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

#if JTAG_COUNT
void vsfhal_jtag_init(int32_t int_priority);
void vsfhal_jtag_fini(void);
void vsfhal_jtag_io_reconfig(void);
void vsfhal_jtag_config(uint16_t kHz, uint16_t retry, uint8_t idle);
#if defined(JTAG_SYNC)
void vsfhal_jtag_raw(uint32_t bitlen, uint8_t *tms, uint8_t *tdi, uint8_t *tdo);
void vsfhal_jtag_ir(uint32_t ir, uint32_t lr_length, uint32_t ir_before, uint32_t ir_after);
uint32_t vsfhal_jtag_dr(uint32_t request, uint32_t dr, uint32_t dr_before, uint32_t dr_after, uint8_t *data);
#elif defined(JTAG_ASYNC)
#define VSFHAL_JTAG_RAW_RETURN_PREVIOUS_ACK_MASK    0x000000ff
uint32_t vsfhal_jtag_raw(uint8_t ack_pos, uint8_t bitlen, uint8_t *tms, uint8_t *tdi, uint8_t *tdo);
uint32_t vsfhal_jtag_raw_less_8bit(uint32_t bitlen, uint32_t tms, uint32_t tdi);
uint32_t vsfhal_jtag_raw_1bit(uint32_t tms, uint32_t tdi);
uint32_t vsfhal_jtag_wait(void);
void vsfhal_jtag_clear(void);
#endif  // JTAG_ASYNC
#if TIMESTAMP_CLOCK
uint32_t vsfhal_jtag_get_timestamp(void);
#endif  // TIMESTAMP_CLOCK
#endif  // JTAG_COUNT

#endif
/* EOF */
