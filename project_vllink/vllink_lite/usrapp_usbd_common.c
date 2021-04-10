#if     VSF_USE_USB_DEVICE == ENABLED                                           \
    &&  (   VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED                               \
        ||  VSF_USBD_USE_DCD_DWCOTG == ENABLED                                  \
        ||  VSF_USBD_USE_DCD_USBIP == ENABLED)
        
typedef struct usrapp_usbd_common_const_t {
#if VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED
    vk_musb_fdrc_dcd_param_t musb_fdrc_dcd_param;
#endif
#if VSF_USBD_USE_DCD_DWCOTG == ENABLED
    vk_dwcotg_dcd_param_t dwcotg_dcd_param;
#endif
#if VSF_USBD_USE_DCD_USBIP == ENABLED
    vk_usbip_dcd_param_t usbip_dcd_param;
#endif
} usrapp_usbd_common_const_t;

typedef struct usrapp_usbd_common_t {
#if VSF_USBD_USE_DCD_USBIP == ENABLED
    vk_usbip_dcd_t usbip_dcd;
#endif
#if VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED
    vk_musb_fdrc_dcd_t musb_fdrc_dcd;
#endif
#if VSF_USBD_USE_DCD_DWCOTG == ENABLED
    vk_dwcotg_dcd_t dwcotg_dcd;
#endif
} usrapp_usbd_common_t;

const usrapp_usbd_common_const_t usrapp_usbd_common_const = {
#if VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED
    .musb_fdrc_dcd_param    = {
        .op                 = &VSF_USB_DC0_IP,
    },
#endif
#if VSF_USBD_USE_DCD_DWCOTG == ENABLED
    .dwcotg_dcd_param       = {
        .op                 = &VSF_USB_DC0_IP,
        .speed              = APP_CFG_USBD_SPEED,
    },
#endif
};

usrapp_usbd_common_t usrapp_usbd_common = {
#if VSF_USBD_USE_DCD_USBIP == ENABLED
    .usbip_dcd.param        = &usrapp_usbd_common_const.usbip_dcd_param,
#endif
#if VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED
    .musb_fdrc_dcd.param    = &usrapp_usbd_common_const.musb_fdrc_dcd_param,
#endif
#if VSF_USBD_USE_DCD_DWCOTG == ENABLED
    .dwcotg_dcd.param       = &usrapp_usbd_common_const.dwcotg_dcd_param,
#endif
};

#if VSF_USBD_USE_DCD_USBIP == ENABLED
vsf_usb_dc_from_usbip_ip(0, usrapp_usbd_common.usbip_dcd, VSF_USB_DC0)
#elif VSF_USBD_USE_DCD_MUSB_FDRC == ENABLED
vsf_usb_dc_from_musbfdrc_ip(0, usrapp_usbd_common.musb_fdrc_dcd, VSF_USB_DC0)
#elif VSF_USBD_USE_DCD_DWCOTG == ENABLED
vsf_usb_dc_from_dwcotg_ip(0, usrapp_usbd_common.dwcotg_dcd, VSF_USB_DC0)
#endif

#endif
