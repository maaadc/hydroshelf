#ifndef HYDROSHELF_LIGHT_CONTROLLER_HPP
#define HYDROSHELF_LIGHT_CONTROLLER_HPP

#include <array>
#include <utility>

#include "../config.hpp"

namespace hydroshelf
{
using hour_t   = float;
using output_t = float;

constexpr output_t offLightLevel = 0.F;
constexpr output_t minLightLevel = .08F;
constexpr output_t maxLightLevel = 1.0F;

enum EScheduleTransition : int
{
    STEP = 0,
    LINEAR
};

class CScheduleStep
{
  public:
    constexpr CScheduleStep() {}

    constexpr CScheduleStep(hour_t f_time, output_t f_output, EScheduleTransition f_type)
        : m_time(f_time), m_output(f_output), m_type(f_type)
    {
    }

    hour_t getStartTime() const
    {
        return m_time;
    }

    output_t getOutput() const
    {
        return m_output;
    }

    EScheduleTransition getTransitionType() const
    {
        return m_type;
    }

  private:
    hour_t              m_time{};
    output_t            m_output{};
    EScheduleTransition m_type{};
};

using schedule_t = std::array<CScheduleStep, 16>;

enum class MODE : int
{
    AUTOMATIC = -1,
    OFF       = +0,
    DIMMED    = +1,
    MAX       = +2
};

class CTimedController
{
  public:
    CTimedController(schedule_t f_schedule) : m_schedule(f_schedule) {}

    void begin()
    {
        ledcSetup(m_pwmChannel, m_pwmFrequencyHz, m_pwmResolutionBit);
        ledcAttachPin(m_gpioPin, m_pwmChannel);

        m_wasInitialized = true;
    }

    float update(hour_t f_timeofDay)
    {
        if (!m_wasInitialized)
        {
            begin();
        }

        assert(m_pwmResolutionBit == 10);
        constexpr float pwmMax = 1024.F;  // actually 2^m_pwmResolutionBit

        float val = getValueFromSchedule(f_timeofDay);

        // according to the datasheet of the MeanWell
        // "Min. dimming level is about 6% and the output current is not defined when 0% < Iout < 6%."
        if (val < minLightLevel)
        {
            val = 0.F;
        }

        const float valAfterInv = (m_invertOutput) ? (1.0F - val) : val;

        const auto pwmValue = static_cast<uint32_t>(pwmMax * valAfterInv);
        ledcWrite(m_pwmChannel, pwmValue);

        return val;
    }

    output_t getValueFromSchedule(hour_t f_timeofDay)
    //< Returns output quantity according to the schedule for the given time
    {
        for (int k = 0; k < m_schedule.size() - 1; ++k)
        {
            const auto startTime   = m_schedule[k].getStartTime();
            const auto startOutput = m_schedule[k].getOutput();

            const auto endTime   = m_schedule[k + 1].getStartTime();
            const auto endOutput = m_schedule[k + 1].getOutput();

            const bool isInInterval = ((startTime <= f_timeofDay) && (endTime > f_timeofDay));
            if (isInInterval)
            {
                switch (m_schedule[k + 1].getTransitionType())
                {
                    case EScheduleTransition::STEP:
                        return startOutput;

                    case EScheduleTransition::LINEAR:
                        return computeLinearInterpolation(f_timeofDay, startTime, endTime, startOutput, endOutput);

                    default:
                        break;
                }
            }
        }

        return static_cast<output_t>(0.F);
    }

  private:
    template <typename InputType, typename OutputType>
    inline OutputType computeLinearInterpolation(const InputType f_value, const InputType f_inStart,
                                                 const InputType f_inEnd, const OutputType f_outStart,
                                                 const OutputType f_outEnd)
    {
        assert(f_inEnd != f_inStart);
        const float      progress = (f_value - f_inStart) / (f_inEnd - f_inStart);
        const OutputType output   = f_outStart * (1.0F - progress) + f_outEnd * progress;
        return output;
    }

    schedule_t m_schedule;

    bool       m_wasInitialized{false};
    const int  m_pwmChannel{0};
    const int  m_pwmFrequencyHz{100};
    const int  m_pwmResolutionBit{10};
    const int  m_gpioPin{32};
    const bool m_invertOutput{1};
};

}  // namespace hydroshelf

#endif  // HYDROSHELF_LIGHT_CONTROLLER_HPP