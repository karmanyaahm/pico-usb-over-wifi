// Simple WiFi network scanner application
// Released to the public domain in 2022 by Earle F. Philhower, III
#include <Arduino.h>
#include <WiFi.h>
//#include <Adafruit_TinyUSB.h>

#define SERVER_HOST "karmanyaahsarch.homenet.malhotra.cc"

//#ifndef WIFISSID
//#define WIFISSID ""
//#define WIFIPASS ""
//#endif

auto w = WiFiClient();

int status = WL_IDLE_STATUS; // the Wifi radio's status

// Adafruit_USBH_Host USBHost;
void setup()
{
  Serial1.begin(115200);
  WiFi.setHostname("robo-pico");
}

uint32_t count = 0;
long long loop_time = 0;

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    status = WiFi.begin(WIFISSID, WIFIPASS);
    Serial1.printf("reconnecting %d %d %s %s", millis(), status, WIFISSID, WIFIPASS);
    if (status != WL_CONNECTED)
    {
      delay(1000);
    }
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
      static char buf[10000];
      int n = w.read(buf, 10000);
      for (int i = min(100, n); i < 10000; i++) buf[i] = 0;
      Serial1.print(buf);
      Serial1.flush();
      Serial1.println(micros());
    }
  }
  
  delayMicroseconds(max(0, loop_time + 1000 - ((long long)micros()) )); // loops run every 1000us
  loop_time = micros();
  count++;
}
