#pragma once

#include "esp_timer.h"
#include "esp_attr.h"
#include "esp_log.h"

#include "config.hpp"
#include "IFan.hpp"

/**
 * @class Fan
 * @brief A class to control a fan using PWM and measure its RPM via a tachometer.
 *
 * This class provides an interface to control the speed of a fan using PWM signals and 
 * read the fan's RPM using a tachometer. It manages the initialization of the PWM and tachometer,
 * and provides methods to set the fan speed and update the RPM.
 */
class Fan : public IFan {
    public:
        /**
         * @brief Constructs a Fan object with specified pins and LEDC configurations.
         *
         * This constructor initializes the fan with the given configuration for the PWM control pin, 
         * tachometer pin, and LEDC PWM channel and timer.
         * 
         * @param config The configuration settings for the fan, including PWM and tachometer pin setup.
         */
        Fan(const FanConfig::Config& config);

        /**
         * @brief Initializes the PWM configuration for controlling the fan speed.
         * 
         * This method sets up the PWM (Pulse Width Modulation) using the specified GPIO pin, channel,
         * and timer. The fan speed can be adjusted using the `setSpeed()` method.
         */
        void initPWM();

        /**
         * @brief Initializes the tachometer to measure the fan's RPM.
         * 
         * This method configures the GPIO pin for the tachometer input and sets up an interrupt service 
         * routine (ISR) to count fan rotations. The RPM can be calculated using the `updateTacho()` method.
         */
        void initTacho();

        /**
         * @brief Returns the configuration settings for the fan.
         *
         * This method retrieves the current configuration for the fan, including the PWM and tachometer settings.
         * 
         * @return The fan's configuration.
         */
        const FanConfig::Config& getConfig() override;

       /**
         * @brief Sets the power of the fan by adjusting the PWM duty cycle.
         * 
         * This method adjusts the fan power by setting the duty cycle of the PWM signal. 
         * The duty cycle corresponds to the desired power in percentage, where 0% is off and 100% is full power.
         * 
         * @param percent The desired power as a percentage (0-100%) to control the fan speed.
         */
        void setPower(uint8_t percent) override;


        /**
         * @brief Returns the current fan speed.
         * 
         * This method retrieves the current speed of the fan, which corresponds to the RPM value measured 
         * by the tachometer.
         * 
         * @return The current speed of the fan.
         */
        uint16_t getSpeed() override;

    private:
        FanConfig::Config config;                 ///< Fan configuration settings.
        uint16_t _counterRPM;                     ///< Counter to track RPM pulses from the tachometer.
        uint16_t _lastRPM;                        ///< Last measured RPM value.
        unsigned long _lastTachoMeasurement;      ///< Timestamp of the last tachometer update.

        /**
         * @brief Interrupt Service Routine (ISR) for counting fan rotations.
         * 
         * This ISR is triggered by the tachometer GPIO pin. It increments the RPM counter
         * each time a falling edge is detected on the tachometer pin, representing one fan rotation.
         *
         * @param arg Pointer to the `Fan` instance for accessing instance-specific variables.
         */
        IRAM_ATTR static void rpmFanISR(void* arg);
};
