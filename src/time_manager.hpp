#ifndef HYDROSHELF_TIME_MANAGER_HPP
#define HYDROSHELF_TIME_MANAGER_HPP

#include "../config.hpp"
#include "WiFi.h"
#include "time.h"

namespace hydroshelf
{
class CTimeManager
{
  public:
    CTimeManager()  = default;
    ~CTimeManager() = default;

    int32_t getTimeOfDaySec()
    {
        const bool wasTimeWrapped    = (millis() < m_millisLastSync);
        const bool wasTimeEverSynced = (m_timeOfDayAtSync_s > -1);

        const int32_t timeSinceSync_s = (millis() - m_millisLastSync) / 1000;
        const bool    isAfterInterval = (timeSinceSync_s > config::ntpUpdateInterval_s);

        if (!wasTimeEverSynced || wasTimeWrapped || isAfterInterval)
        {
            doNtpSync();
            return m_timeOfDayAtSync_s;
        }

        return (m_timeOfDayAtSync_s + timeSinceSync_s) % 86400;
    }

    float getTimeOfDayHour()
    {
        return static_cast<float>(getTimeOfDaySec()) / 3600.F;
    }

  private:
    bool doNtpSync()
    {
        const bool isWifiConnected = (WiFi.status() == WL_CONNECTED);
        if (!isWifiConnected)
        {
            do
            {
                // workaround to properly reconnect and set hostname:
                // https://github.com/espressif/arduino-esp32/issues/2501
                // https://github.com/espressif/arduino-esp32/issues/2537
                WiFi.disconnect(true);
                WiFi.begin();
                delay(500);
                WiFi.disconnect(true);
                delay(500);
                WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
                WiFi.setHostname(config::wifiHostname);
                WiFi.begin(config::wifiNetwork, config::wifiPassword);
                delay(500);
                Serial.println(".");
            } while (WiFi.status() != WL_CONNECTED);
        }

        configTzTime(config::timeZone, config::ntpServer);

        struct tm timeinfo;
        if (getLocalTime(&timeinfo))
        {
            m_millisLastSync    = millis();
            m_timeOfDayAtSync_s = timeinfo.tm_sec + 60 * timeinfo.tm_min + 3600 * timeinfo.tm_hour;

            Serial.print("NTP sync successful: ");
            Serial.println(asctime(&timeinfo));
            return true;
        }
        else
        {
            Serial.print("NTP sync failed: isWifiConnected=");
            Serial.print(isWifiConnected);
            Serial.println(" ");
            return false;
        }
    }

    unsigned long m_millisLastSync{};
    int32_t       m_timeOfDayAtSync_s{-1};
    struct tm     m_timeAtSync;
};

} // namespace hydroshelf

#endif // HYDROSHELF_TIME_MANAGER_HPP