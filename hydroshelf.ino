#include "M5Atom.h"
#include "WiFi.h"
#include "config.hpp"
#include "src/helper.hpp"
#include "src/light_controller.hpp"
#include "src/matrix_display.hpp"
#include "src/time_manager.hpp"

using namespace hydroshelf;

constexpr schedule_t lightSchedule{{{00.0F, offLightLevel, EScheduleTransition::STEP},
                                    {07.5F, minLightLevel, EScheduleTransition::STEP},
                                    {08.0F, maxLightLevel, EScheduleTransition::LINEAR},
                                    {18.0F, maxLightLevel, EScheduleTransition::STEP},
                                    {19.0F, minLightLevel, EScheduleTransition::LINEAR},
                                    {23.0F, minLightLevel, EScheduleTransition::STEP},
                                    {24.0F, offLightLevel, EScheduleTransition::STEP}}};

CMatrixDisplay   disp{};
CTimedController lightCtrl{lightSchedule};
CTimeManager     timeMgr{};

void setup()
{
    M5.begin(true, true, true);

    disp.fillColor(ColorYellow);
    disp.show();
    disp.fillBlack();
}

void loop()
{
    M5.update();

    const float hourOfDay  = timeMgr.getTimeOfDayHour();
    const float lightLevel = lightCtrl.update(hourOfDay);

    disp.drawProgressBarTwoLines(ColorYellow, lightLevel, 0);
    disp.showWaitBar(ColorGreen, timeMgr.getTimeOfDaySec(), 4);
    disp.show();

    Serial.print("time = ");
    Serial.print(hourOfDay);
    Serial.print(", light = ");
    Serial.println(lightLevel);

    delay(300);
}
