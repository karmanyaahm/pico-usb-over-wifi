//#include <SerialUART.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../lib/tinyusb/hw/bsp/board.h"
#include "tusb.h"
#include "pio_usb.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>

// English
#define LANGUAGE_ID 0x0409
#define BUF_COUNT 4

tusb_desc_device_t desc_device;
#define HOST "karmanyaahsarch.homenet.malhotra.cc"
WiFiClient client = WiFiClient();

queue_t q1_bc;
queue_t q1_cb;

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+
void led_blinking_task(void);

extern void cdc_task(void);

CFG_TUSB_MEM_SECTION static char serial_in_buffer[512] = {0};
// 128 long just seems to print nothing at all TODO find out wtf is wrong

void tuh_cdc_xfer_cb(tuh_xfer_t *xfer)
{
  // Serial1.printf("xfer_cb buflen: %d daddr %d ep addr %d result %d user data %s\n", xfer->buflen, xfer->daddr, xfer->ep_addr, xfer->result, (char *)(xfer->user_data));
}

/*------------- MAIN -------------*/
int count = 0;
void setup1()
{
  set_sys_clock_khz(120000, true);
  // board_init();

  Serial1.begin(115200);
  Serial1.printf("TinyUSB Bare API Example\r\n");

  queue_init(&q1_bc, 1, 10000);
  queue_init(&q1_cb, 1, 10000);
  sleep_ms(10);

  pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
  pio_cfg.pin_dp = 3;
  pio_cfg.sm_tx = 3;
  pio_cfg.sm_rx = 2;
  pio_cfg.sm_eop = 3;
  pio_cfg.pio_rx_num = 0;
  pio_cfg.pio_tx_num = 1;
  pio_cfg.tx_ch = 9;

  tuh_configure(1, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &pio_cfg);

  // init host stack on configured roothub port
  tuh_init(1); // TODO, run this line in core 1

  while (1)
  {
    // tinyusb host task
    tuh_task();
    cdc_task();

    if (count % 1900000 == 0)
    {
      // Serial1.printf("%d 0, 1, 2, 3: %d %d %d %d %d\n", count, tuh_mounted(0), tuh_mounted(1), tuh_mounted(2), tuh_cdc_serial_is_mounted(1));
      // Serial1.printf("%d %d %d, %d %d %d, %d \n", cdch_data[0].itf_num, cdch_data[0].ep_in, cdch_data[0].ep_out, cdch_data[1].itf_num, cdch_data[1].ep_in, cdch_data[1].ep_out);
      //  tuh_cdc_receive(1, serial_in_buffer, sizeof(serial_in_buffer), true)); // waiting for next data
    }
    if (count % 1900000 == 0 && count / 1900000 == 3)
      tuh_cdc_connect(1, tuh_cdc_xfer_cb);
    count++;
  }
}
void setup()
{
  int count1 = 0;
  sleep_ms(2000);

  Serial1.printf("Setting up wifi...0\n");
  // already called by arduino stuffs, maybe double call is causing stucking????
  // if (cyw43_arch_init())
  //{
  //   Serial1.printf("failed to initialize wifi\n");
  //   return ;
  // }

  Serial1.printf("Setting up wifi...\n");
  // cyw43_arch_enable_sta_mode();
  WiFi.setHostname("robo-pico");
  client.keepAlive(5, 3, 2);
  // client.setDefaultNoDelay(true);
  int status = WiFi.begin(WIFISSID, WIFIPASS);
  Serial1.printf("Wifi status %d\n", status);

  while (1)
  {
    // cyw43_arch_poll();
    char ch;
    int count = 0; // read many chars, but only 512 in one loop
    while (queue_try_remove(&q1_bc, &ch) && count < 512)
    {
      client.write(ch);
      count++;
    }
    count = 0;
    int chh = client.read();
    while (chh > 0 && count < 512)
    {
      Serial1.printf(queue_try_add(&q1_cb, &chh) ? "" : "q1_cb cannot add\n");
      chh = client.read();
      count++;
    }
    client.flush(20);

    if (count1 % (int)1e4 == 0 && !client.connected())
      Serial1.printf("retrying connect TCP status: %d\n", client.connect(HOST, 10001));
    sleep_ms(1);
    count1++;
  }
}
void loop(){};

// invoked ISR context
void tuh_cdc_xfer_isr(uint8_t dev_addr, xfer_result_t event, cdc_pipeid_t pipe_id, uint32_t xferred_bytes)
{

  // for (int i = 0; i < xferred_bytes; i++)
  //   Serial1.printf("%c", serial_in_buffer[i]);

  for (int i = 0; i < xferred_bytes; i++)
    Serial1.printf(queue_try_add(&q1_bc, &(serial_in_buffer[i])) ? "" : "q1_bc cannot add\n");


  tu_memclr(serial_in_buffer, sizeof(serial_in_buffer));

  // tuh_cdc_receive(dev_addr, serial_in_buffer, sizeof(serial_in_buffer), true); // waiting for next data
}

void cdc_task(void)
{
  // if (count % 10 == 0)
  tuh_cdc_receive(1, serial_in_buffer, sizeof(serial_in_buffer), true);

  // reading bytes 1 by 1 for now, but TODO read a bunch at once
  // int ch = client.read();
  // if (ch < 0)
  //  ch = Serial1.read();
  char ch;
  if (queue_try_remove(&q1_cb, &ch))
  {

    Serial1.printf("COMPUTER->BRAIN received char: %02x\n", ch);
    tuh_cdc_send(1, &ch, 1, false);
  }
}

/*------------- TinyUSB Callbacks -------------*/

// Invoked when device is mounted (configured)
void tuh_mount_cb(uint8_t daddr)
{
  Serial1.printf("Device attached, address = %d %d\r\n", daddr, cdch_data[daddr].ep_in);

  // tuh_cdc_receive(daddr, serial_in_buffer, sizeof(serial_in_buffer), true)); // waiting for next data
  // Get Device Descriptor
  // TODO: invoking control transfer now has issue with mounting hub with multiple devices attached, fix later
  // tuh_descriptor_get_device(daddr, &desc_device, 18, print_device_descriptor, 0);
}

/// Invoked when device is unmounted (bus reset/unplugged)
void tuh_umount_cb(uint8_t daddr)
{
  Serial1.printf("Device removed, address = %d\r\n", daddr);
}

//
int serial1_printf(const char *__restrict format, ...)
{
  char buf[256];
  int len;
  va_list ap;
  va_start(ap, format);
  len = vsnprintf(buf, sizeof(buf), format, ap);
  Serial1.write(buf);
  va_end(ap);
  return len;
}