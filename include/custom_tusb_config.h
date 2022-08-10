/*
    The MIT License (MIT)
    Copyright (c) 2018, hathach for Adafruit
*/

#define _TUSB_CONFIG_RP2040_H_

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------

// Enable device stack
#define CFG_TUD_ENABLED 0

#ifndef CFG_TUSB_MCU
#define CFG_TUSB_MCU OPT_MCU_RP2040
#endif
#define CFG_TUSB_OS OPT_OS_PICO

#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG 0
#endif

#if CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG_PRINTF serial1_printf
extern int serial1_printf(const char *__restrict __format, ...);
#endif

#define CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_ALIGN TU_ATTR_ALIGNED(4)

// Device Configuration

// Don't want this, but need this to prevent SerialUSB errors
#define CFG_TUSB_RHPORT1_MODE OPT_MODE_DEVICE
#define CFG_TUD_CDC 1

#define CFG_TUD_CDC_RX_BUFSIZE 256
#define CFG_TUD_CDC_TX_BUFSIZE 256

//--------------------------------------------------------------------
// Host Configuration
//--------------------------------------------------------------------

#define CFG_TUH_ENABLE 1

#define CFG_TUSB_RHPORT0_MODE OPT_MODE_HOST | OPT_MODE_LOW_SPEED

#define CFG_TUH_API_EDPT_XFER       1
#define CFG_TUH_ENUMERATION_BUFSIZE 256


#define CFG_TUH_HUB                 1
#define CFG_TUH_CDC                 2
#define CFG_TUH_HID                 0
#define CFG_TUH_MSC                 0
#define CFG_TUH_VENDOR              0

// max device support (excluding hub device)
#define CFG_TUH_DEVICE_MAX          (CFG_TUH_HUB ? 4 : 1) // hub typically has 4 ports


#ifdef __cplusplus
}
#endif
