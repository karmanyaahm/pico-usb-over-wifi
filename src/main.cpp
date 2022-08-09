// Simple WiFi network scanner application
// Released to the public domain in 2022 by Earle F. Philhower, III
#include <Arduino.h>
#include <WiFi.h>

#define SERVER_HOST "karmanyaahsarch.homenet.malhotra.cc"

#ifndef WIFISSID
#define WIFISSID ""
#define WIFIPASS ""
#endif

auto w = WiFiClient();

int status = WL_IDLE_STATUS; // the Wifi radio's status

void setup()
{
  Serial1.begin(115200);
  WiFi.setHostname("robo-pico");
}

uint8_t count = 0;
uint64_t loop_time = 0;

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    status = WiFi.begin(WIFISSID, WIFIPASS);
    Serial1.printf("Not connected, reconnecting %d %d %s %s", millis(), status, WIFISSID, WIFIPASS);
  }
  else
  {

    if (count % 9000 == 0) // every 9 s
    {
      auto myip = WiFi.localIP();
      Serial1.printf("--- Status: %d %s\n", status, myip.toString().c_str());
    }

    if (!w.connected() && count % 100 == 0) // attempt every 100 ms
      w.connect(SERVER_HOST, 10000);
    if (count % 3000 == 0)
    { // eevry 3000ms
      w.write("HIIIIIIIII\n");
      w.print(count);
    }

    if (w.available()) // read one set in one loop, if there's too much data, read in next loop to avoid stalling everything
    {
      static char buf[1000];
      int n = w.read(buf, 1000);
      buf[n] = '\0'; // terminate string there
      Serial1.print(buf);
    }
  }
  count++;
  delayMicroseconds(loop_time + 1000 - micros()); // loops run every 1000us
}
