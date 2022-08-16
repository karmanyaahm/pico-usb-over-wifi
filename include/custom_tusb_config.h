#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

//--------------------------------------------------------------------+
// Board Specific Configuration
//--------------------------------------------------------------------+

// RHPort number used for host can be defined by board.mk, default to port 0
#ifndef BOARD_TUH_RHPORT
#define BOARD_TUH_RHPORT      1
#endif

// RHPort max operational speed can defined by board.mk
#ifndef BOARD_TUH_MAX_SPEED
#define BOARD_TUH_MAX_SPEED   OPT_MODE_DEFAULT_SPEED
#endif

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------

// defined by compiler flags for flexibility
#ifndef CFG_TUSB_MCU
#error CFG_TUSB_MCU must be defined
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS           OPT_OS_NONE
#endif

//#define PICO_CYW43_ARCH_THREADSAFE_BACKGROUND

#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG 2
#define DEBUG_RP2040_PORT Serial1
#endif

#if CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG_PRINTF serial1_printf
#define CYW43_ARCH_DEBUG serial1_printf
extern int serial1_printf(const char *__restrict __format, ...);
#endif
// Enable Host stack
#define CFG_TUH_ENABLED       1

// Default is max speed that hardware controller could support with on-chip PHY
#define CFG_TUH_MAX_SPEED     BOARD_TUH_MAX_SPEED

/* USB DMA on some MCUs can only access a specific SRAM region with restriction on alignment.
 * Tinyusb use follows macros to declare transferring memory so that they can be put
 * into those specific section.
 * e.g
 * - CFG_TUSB_MEM SECTION : __attribute__ (( section(".usb_ram") ))
 * - CFG_TUSB_MEM_ALIGN   : __attribute__ ((aligned(4)))
 */
#ifndef CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_SECTION
#endif

#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN          __attribute__ ((aligned(4)))
#endif

//--------------------------------------------------------------------
// CONFIGURATION
//--------------------------------------------------------------------
#define CFG_TUH_RPI_PIO_USB 1

// Size of buffer to hold descriptors and other data used for enumeration
#define CFG_TUH_ENUMERATION_BUFSIZE 256

// only hub class is enabled
#define CFG_TUH_HUB                 0
#define CFG_TUH_CDC                 2

// max device support (excluding hub device)
// 1 hub typically has 4 ports
#define CFG_TUH_DEVICE_MAX          (CFG_TUH_HUB ? 4 : 2)

// Max endpoint per device
#define CFG_TUH_ENDPOINT_MAX        8

// Enable tuh_edpt_xfer() API
#define CFG_TUH_API_EDPT_XFER       1


#define CFG_TUD_CDC_RX_BUFSIZE   512
#define CFG_TUD_CDC_TX_BUFSIZE   512
#define CFG_TUD_CDC_EP_BUFSIZE   512


#ifdef __cplusplus
 }
#endif

#endif /* _TUSB_CONFIG_H_ */

