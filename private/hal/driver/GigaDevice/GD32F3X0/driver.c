/*============================ INCLUDES ======================================*/

#include "./driver.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
typedef void(*pFunc)(void);
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
extern const pFunc __VECTOR_TABLE[];
/*============================ IMPLEMENTATION ================================*/

static vsfhal_clk_info_t vsfhal_clk_info = {
    .clken = CHIP_CLKEN,
    .hclksrc = CHIP_HCLKSRC,
    .pllsrc = CHIP_PLLSRC,
    .usbsrc = CHIP_USBSRC,
    .lse_freq_hz = CHIP_LSE_FREQ_HZ,
    .hse_freq_hz = CHIP_HSE_FREQ_HZ,
    .pll_freq_hz = CHIP_PLL_FREQ_HZ,
    .ahb_freq_hz = CHIP_AHB_FREQ_HZ,
    .apb1_freq_hz = CHIP_APB1_FREQ_HZ,
    .apb2_freq_hz = CHIP_APB2_FREQ_HZ,
};

static void scs_delay(void)
{
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
    __ASM("NOP");
}

static void clk_init(vsfhal_clk_info_t *info)
{
#ifndef PROJ_CFG_CORE_INIT_TINY
    uint32_t tmp32;

    VSF_HAL_ASSERT(info && (info->pllsrc <= GD32F3X0_PLLSRC_HSI48M));

    RCU_CTL0 |= RCU_CTL0_IRC8MEN;
    while (!(RCU_CTL0 & RCU_CTL0_IRC8MSTB));
    RCU_CFG0 &= ~RCU_CFG0_SCS;
    scs_delay();

    if (info->clken & GD32F3X0_CLKEN_HSI48M) {
        RCU_ADDCTL |= RCU_ADDCTL_IRC48MEN;
        while (!(RCU_ADDCTL & RCU_ADDCTL_IRC48MSTB));
    }

    if (info->clken & GD32F3X0_CLKEN_HSE) {
        RCU_CTL0 |= RCU_CTL0_HXTALEN;
        while (!(RCU_CTL0 & RCU_CTL0_HXTALSTB));
    }

    RCU_CTL0 &= ~RCU_CTL0_PLLEN;
    if (info->clken & GD32F3X0_CLKEN_PLL) {
        if (info->pllsrc == GD32F3X0_PLLSRC_HSI8M_D2) {
            RCU_CFG0 &= RCU_CFG0_PLLSEL;
        } else if (info->pllsrc == GD32F3X0_PLLSRC_HSE) {
            RCU_CFG1 &= RCU_CFG1_PLLPRESEL | RCU_CFG1_PREDV;
            RCU_CFG1 |= info->hse_freq_hz / 4000000 - 1;
            RCU_CFG0 |= RCU_CFG0_PLLSEL;
        } else if (info->pllsrc == GD32F3X0_PLLSRC_HSI48M) {
            RCU_CFG1 &= RCU_CFG1_PLLPRESEL | RCU_CFG1_PREDV;
            RCU_CFG1 |= RCU_CFG1_PLLPRESEL | (48000000 / 4000000 - 1);
            RCU_CFG0 |= RCU_CFG0_PLLSEL;
        }
        
        tmp32 = info->pll_freq_hz / 4000000;
        if (tmp32 < 2)
            tmp32 = 0;
        else if (tmp32 > 64)
            tmp32 = 63;
        else if (tmp32 <= 16)
            tmp32 -= 2;
        else
            tmp32 -= 1;
        RCU_CFG0 &= ~RCU_CFG0_PLLMF;
        RCU_CFG1 &= ~RCU_CFG1_PLLMF5;
        RCU_CFG0 |= ((tmp32 & 0xf) << 18) | ((tmp32 & 0x10) << 23);
        RCU_CFG1 |= (tmp32 & 0x20) << 26;

        RCU_CTL0 |= RCU_CTL0_PLLEN;
        while (!(RCU_CTL0 & RCU_CTL0_PLLSTB));
    }

    RCU_CFG0 &= ~(RCU_CFG0_AHBPSC | RCU_CFG0_APB1PSC | RCU_CFG0_APB2PSC);
    tmp32 = info->ahb_freq_hz / info->apb1_freq_hz;
    if (tmp32 == 2)
        RCU_CFG0 |= RCU_APB1_CKAHB_DIV2;
    else if (tmp32 == 4)
        RCU_CFG0 |= RCU_APB1_CKAHB_DIV4;
    else if (tmp32 == 8)
        RCU_CFG0 |= RCU_APB1_CKAHB_DIV8;
    else if (tmp32 > 8)
        RCU_CFG0 |= RCU_APB1_CKAHB_DIV16;
    tmp32 = info->ahb_freq_hz / info->apb2_freq_hz;
    if (tmp32 == 2)
        RCU_CFG0 |= RCU_APB2_CKAHB_DIV2;
    else if (tmp32 == 4)
        RCU_CFG0 |= RCU_APB2_CKAHB_DIV4;
    else if (tmp32 == 8)
        RCU_CFG0 |= RCU_APB2_CKAHB_DIV8;
    else if (tmp32 > 8)
        RCU_CFG0 |= RCU_APB2_CKAHB_DIV16;
    if (info->hclksrc == GD32F3X0_HCLKSRC_HSE)
        tmp32 = info->hse_freq_hz;
    else if (info->hclksrc == GD32F3X0_HCLKSRC_PLL)
        tmp32 = info->pll_freq_hz;
    else
        tmp32 = 8000000;
    tmp32 = tmp32 / info->ahb_freq_hz;
    if (tmp32 == 2)
        RCU_CFG0 |= RCU_AHB_CKSYS_DIV2;
    else if (tmp32 == 4)
        RCU_CFG0 |= RCU_AHB_CKSYS_DIV4;
    else if (tmp32 == 8)
        RCU_CFG0 |= RCU_AHB_CKSYS_DIV8;
    else if (tmp32 == 16)
        RCU_CFG0 |= RCU_AHB_CKSYS_DIV16;
    else if (tmp32 == 64)
        RCU_CFG0 |= RCU_AHB_CKSYS_DIV64;
    else if (tmp32 == 128)
        RCU_CFG0 |= RCU_AHB_CKSYS_DIV128;
    else if (tmp32 == 256)
        RCU_CFG0 |= RCU_AHB_CKSYS_DIV256;
    else if (tmp32 > 256)
        RCU_CFG0 |= RCU_AHB_CKSYS_DIV512;

    if (info->hclksrc == GD32F3X0_HCLKSRC_HSE) {
        RCU_CFG0 |= RCU_CKSYSSRC_HXTAL;
        while ((RCU_CFG0 & RCU_SCSS_HXTAL) != RCU_SCSS_HXTAL);
    } else if (info->hclksrc == GD32F3X0_HCLKSRC_PLL) {
        RCU_CFG0 |= RCU_CKSYSSRC_PLL;
        while ((RCU_CFG0 & RCU_SCSS_PLL) != RCU_SCSS_PLL);
    } else {
        RCU_CFG0 &= ~RCU_CFG0_SCS;
        while ((RCU_CFG0 & RCU_SCSS_IRC8M) != RCU_SCSS_IRC8M);
    }
    scs_delay();
    
    if (!(info->clken & GD32F3X0_CLKEN_HSI))
        RCU_CTL0 &= ~RCU_CTL0_IRC8MEN;
#else
    // select irc8m
    RCU_CTL0 |= RCU_CTL0_IRC8MEN;
    while (!(RCU_CTL0 & RCU_CTL0_IRC8MSTB));
    RCU_CFG0 &= ~RCU_CFG0_SCS;

    RCU_ADDCTL |= RCU_ADDCTL_IRC48MEN;
    while (!(RCU_ADDCTL & RCU_ADDCTL_IRC48MSTB));

    RCU_CTL0 &= ~RCU_CTL0_PLLEN;

#   if defined(PROJ_CFG_GD32F3X0_HSI48M_USB_PLL_108M)
    RCU_CFG1 &= RCU_CFG1_PLLPRESEL | RCU_CFG1_PREDV;
    RCU_CFG1 |= RCU_CFG1_PLLPRESEL | 11;    // 48M / 12 = 4M
    RCU_CFG0 |= RCU_CFG0_PLLSEL;

    RCU_CFG0 &= ~RCU_CFG0_PLLMF;
    RCU_CFG1 &= ~RCU_CFG1_PLLMF5;
    RCU_CFG0 |= (((27 - 1) & 0xf) << 18) | (((27 - 1) & 0x10) << 23);    // 4M * 27 = 108M
    RCU_CFG1 |= ((27 - 1) & 0x20) << 26;

    RCU_CTL0 |= RCU_CTL0_PLLEN;
    while (!(RCU_CTL0 & RCU_CTL0_PLLSTB));
#   elif defined(PROJ_CFG_GD32F3X0_HSI48M_USB_PLL_72M)
    RCU_CFG1 &= RCU_CFG1_PLLPRESEL | RCU_CFG1_PREDV;
    RCU_CFG1 |= RCU_CFG1_PLLPRESEL | 11;    // 48M / 12 = 4M
    RCU_CFG0 |= RCU_CFG0_PLLSEL;

    RCU_CFG0 &= ~RCU_CFG0_PLLMF;
    RCU_CFG1 &= ~RCU_CFG1_PLLMF5;
    RCU_CFG0 |= (((18 - 1) & 0xf) << 18) | (((18 - 1) & 0x10) << 23);    // 4M * 18 = 72M
    RCU_CFG1 |= ((18 - 1) & 0x20) << 26;

    RCU_CTL0 |= RCU_CTL0_PLLEN;
    while (!(RCU_CTL0 & RCU_CTL0_PLLSTB));
#   else
#       error "Not Support!"
#   endif

    // config ahb apb1 apb2
    RCU_CFG0 &= ~(RCU_CFG0_AHBPSC | RCU_CFG0_APB1PSC | RCU_CFG0_APB2PSC);
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV1 | RCU_APB1_CKAHB_DIV2 | RCU_APB2_CKAHB_DIV2;

    RCU_CFG0 |= RCU_CKSYSSRC_PLL;
    while ((RCU_CFG0 & RCU_SCSS_PLL) != RCU_SCSS_PLL);

    RCU_CTL0 &= ~RCU_CTL0_IRC8MEN;
#endif

    RCU_AHBEN |= RCU_AHBEN_DMAEN;
}

WEAK(vsf_driver_init_usrapp)
bool vsf_driver_init_usrapp(void) 
{
    return true;
}

bool vsf_driver_init(void)
{
    NVIC_SetPriorityGrouping(3);    // 4 bits for pre-emption priority
    SCB->VTOR = (uint32_t)__VECTOR_TABLE;
    ENABLE_GLOBAL_INTERRUPT();
    
    vsf_driver_init_usrapp();

    clk_init(&vsfhal_clk_info);
    return true;
}

vsfhal_clk_info_t *vsfhal_clk_info_get(void)
{
    return &vsfhal_clk_info;
}

void vsf_arch_sleep(uint_fast32_t mode)
{
    UNUSED_PARAM(mode);
    //__WFE();
}

#ifdef PROJ_CFG_GD32F3X0_AHP_APB_UNFIXED
void vsfhal_clk_reconfig_apb(uint32_t apb_freq_hz)
{
    uint32_t temp;

    if (vsfhal_clk_info.apb1_freq_hz != vsfhal_clk_info.apb2_freq_hz)
        return;

    if ((vsfhal_clk_info.apb1_freq_hz == 64000000) && (apb_freq_hz == 48000000)) {
        vsf_gint_state_t gint_state = vsf_disable_interrupt(); 

        RCU_CTL0 |= RCU_CTL0_IRC8MEN;
        while (!(RCU_CTL0 & RCU_CTL0_IRC8MSTB));
        RCU_CFG0 &= ~RCU_CFG0_SCS;
        scs_delay();
        
        RCU_CTL0 &= ~RCU_CTL0_PLLEN;
        RCU_CFG1 &= RCU_CFG1_PLLPRESEL | RCU_CFG1_PREDV;
        RCU_CFG1 |= RCU_CFG1_PLLPRESEL | (48000000 / 4000000 - 1);
        RCU_CFG0 |= RCU_CFG0_PLLSEL;

        RCU_CFG0 &= ~RCU_CFG0_PLLMF;
        RCU_CFG1 &= ~RCU_CFG1_PLLMF5;
        RCU_CFG0 |= (((24 - 1) & 0xf) << 18) | (((24 - 1) & 0x10) << 23);    // 4M * 24 = 96M
        RCU_CFG1 |= ((24 - 1) & 0x20) << 26;

        RCU_CTL0 |= RCU_CTL0_PLLEN;
        while (!(RCU_CTL0 & RCU_CTL0_PLLSTB));

        // config usart
        if (USART_CTL0(USART0) & USART_CTL0_UEN) {
            temp = 64000000 / USART_BAUD(USART0);
            USART_CTL0(USART0) &= ~USART_CTL0_UEN;
            USART_BAUD(USART0) = 48000000 / temp;
            USART_CTL0(USART0) |= USART_CTL0_UEN;
        }
        if (USART_CTL0(USART1) & USART_CTL0_UEN) {
            temp = 64000000 / USART_BAUD(USART1);
            USART_CTL0(USART1) &= ~USART_CTL0_UEN;
            USART_BAUD(USART1) = 48000000 / temp;
            USART_CTL0(USART1) |= USART_CTL0_UEN;
        }

        // config timer
        temp = 64000000 / 2 / (TIMER_PSC(TIMER5) + 1);
        TIMER_PSC(TIMER5) = 48000000 / 2 / temp - 1;
        
        temp = 64000000 / 2 / (TIMER_PSC(TIMER13) + 1);
        TIMER_PSC(TIMER13) = 48000000 / 2 / temp - 1;
        
        RCU_CFG0 |= RCU_CKSYSSRC_PLL;
        while ((RCU_CFG0 & RCU_SCSS_PLL) != RCU_SCSS_PLL);

        RCU_CTL0 &= ~RCU_CTL0_IRC8MEN;

        vsfhal_clk_info.apb1_freq_hz = apb_freq_hz;
        vsfhal_clk_info.apb2_freq_hz = apb_freq_hz;
        vsfhal_clk_info.pll_freq_hz = apb_freq_hz * 2;
        vsfhal_clk_info.ahb_freq_hz = apb_freq_hz * 2;
        vsf_set_interrupt(gint_state);
    } else if ((vsfhal_clk_info.apb1_freq_hz == 48000000) && (apb_freq_hz == 64000000)) {
        vsf_gint_state_t gint_state = vsf_disable_interrupt(); 
        
        RCU_CTL0 |= RCU_CTL0_IRC8MEN;
        while (!(RCU_CTL0 & RCU_CTL0_IRC8MSTB));
        RCU_CFG0 &= ~RCU_CFG0_SCS;
        scs_delay();
        
        RCU_CTL0 &= ~RCU_CTL0_PLLEN;
        RCU_CFG1 &= RCU_CFG1_PLLPRESEL | RCU_CFG1_PREDV;
        RCU_CFG1 |= RCU_CFG1_PLLPRESEL | (48000000 / 4000000 - 1);
        RCU_CFG0 |= RCU_CFG0_PLLSEL;

        RCU_CFG0 &= ~RCU_CFG0_PLLMF;
        RCU_CFG1 &= ~RCU_CFG1_PLLMF5;
        RCU_CFG0 |= (((24 - 1) & 0xf) << 18) | (((24 - 1) & 0x10) << 23);    // 4M * 24 = 96M
        RCU_CFG1 |= ((24 - 1) & 0x20) << 26;

        RCU_CTL0 |= RCU_CTL0_PLLEN;
        while (!(RCU_CTL0 & RCU_CTL0_PLLSTB));

        // config usart
        if (USART_CTL0(USART0) & USART_CTL0_UEN) {
            temp = 48000000 / USART_BAUD(USART0);
            USART_CTL0(USART0) &= ~USART_CTL0_UEN;
            USART_BAUD(USART0) = 64000000 / temp;
            USART_CTL0(USART0) |= USART_CTL0_UEN;
        }
        if (USART_CTL0(USART1) & USART_CTL0_UEN) {
            temp = 48000000 / USART_BAUD(USART1);
            USART_CTL0(USART1) &= ~USART_CTL0_UEN;
            USART_BAUD(USART1) = 64000000 / temp;
            USART_CTL0(USART1) |= USART_CTL0_UEN;
        }

        // config timer
        temp = 48000000 / 2 / (TIMER_PSC(TIMER5) + 1);
        TIMER_PSC(TIMER5) = 64000000 / 2 / temp - 1;
        
        temp = 48000000 / 2 / (TIMER_PSC(TIMER13) + 1);
        TIMER_PSC(TIMER13) = 64000000 / 2 / temp - 1;

        RCU_CFG0 |= RCU_CKSYSSRC_PLL;
        while ((RCU_CFG0 & RCU_SCSS_PLL) != RCU_SCSS_PLL);

        RCU_CTL0 &= ~RCU_CTL0_IRC8MEN;

        vsfhal_clk_info.apb1_freq_hz = apb_freq_hz;
        vsfhal_clk_info.apb2_freq_hz = apb_freq_hz;
        vsfhal_clk_info.pll_freq_hz = apb_freq_hz * 2;
        vsfhal_clk_info.ahb_freq_hz = apb_freq_hz * 2;
        vsf_set_interrupt(gint_state);
    }
}
#endif

uint32_t vsfhal_uid_read(uint8_t *buffer, uint32_t size)
{
    size = min(size, 12);   // 96 bit max
    memcpy(buffer, (uint8_t *)0x1FFFF7AC, size);
    return size;
}

/* EOF */
