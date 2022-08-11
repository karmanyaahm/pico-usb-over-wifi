#include <Arduino.h>
#include <WiFi.h>
#include "tusb.h"

#define SERVER_HOST "karmanyaahsarch.homenet.malhotra.cc"

auto w = WiFiClient();
int status = WL_IDLE_STATUS; // the Wifi radio's status

void setup()
{
  Serial1.begin(115200);
  Serial1.println("hiiiiii");
  WiFi.setHostname("robo-pico");

  Serial1.println("Initializing tusb");
  tusb_init();
  Serial1.println("HII, setup finished");
}

uint32_t count = 0;
long long loop_time = 0;

void loop()
{
  if (status = WiFi.status(); status == WL_CONNECTED || status == WL_CONNECTING)
  {
    status = WiFi.begin(WIFISSID, WIFIPASS);
    Serial1.printf("attempted reconnect %d %d %s %s", millis(), status, WIFISSID, WIFIPASS);
  }
  else
  {
    if (count % 10000 == 0) // every 10 s
    {
      auto myip = WiFi.localIP();
      Serial1.printf("--- Status: %d %s %d %d\n", status, myip.toString().c_str(), millis(), count);
    }

    if (!w.connected() && count % 100 == 0) // attempt every 100 ms
      w.connect(SERVER_HOST, 10000);
    if (count % 5000 == 0)
    { // eevry 3000ms
      w.write("HIIIIIIIII\n");
      w.println(count);
    }

    if (w.available()) // read one set in one loop, if there's too much data, read in next loop to avoid stalling everything
    {
      Serial1.println(micros());
      static char buf[1000];
      int n = w.read(buf, 1000);
      for (int i = min(100, n); i < 1000; i++)
        buf[i] = 0;
      Serial1.print(buf);
      Serial1.flush();
      Serial1.println(micros());
    }
  }

  do
  {
    tuh_task();
    tud_task();
  } while (loop_time + 1000 > micros());
  // run loop every 1000 us

  loop_time = micros();
  count++;
}

//CFG_TUSB_MEM_SECTION static char serial_in_buffer[64] = { 0 };

void tuh_mount_cb(uint8_t daddr)
{
  //Serial1.printf("Device attached, address = %d\r\n", daddr);
    //tuh_cdc_receive(daddr, serial_in_buffer, sizeof(serial_in_buffer), true); // schedule first transfer

}

void tuh_umount_cb(uint8_t daddr)
{
  //Serial1.printf("Device removed, address = %d\r\n", daddr);
}

// invoked ISR context
void tuh_cdc_xfer_isr(uint8_t dev_addr, xfer_result_t event, cdc_pipeid_t pipe_id, uint32_t xferred_bytes)
{
 // (void) event;
 // (void) pipe_id;
 // (void) xferred_bytes;

 // printf(serial_in_buffer);
 // tu_memclr(serial_in_buffer, sizeof(serial_in_buffer));

 // tuh_cdc_receive(dev_addr, serial_in_buffer, sizeof(serial_in_buffer), true); // waiting for next data
}
