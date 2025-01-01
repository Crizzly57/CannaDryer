#pragma once

/**
 * @class IFan
 * @brief Interface for controlling a fan.
 * 
 * This interface defines the basic operations for controlling a fan, including methods to 
 * set and get the fan speed. Any class that implements this interface should provide 
 * the logic for controlling the fan's speed and retrieving the current speed.
 */
class IFan{
    public:
        virtual ~IFan() = default;

        /**
         * @brief Sets the power of the fan.
         * 
         * This method allows setting the fan power as a percentage (0-100%) where 0% is off 
         * and 100% is full power.
         * 
         * @param percent The desired fan power as a percentage (0-100%).
         */
        virtual void setPower(uint8_t percent) = 0;

        /**
         * @brief Gets the current speed of the fan.
         * 
         * This method returns the current fan speed in RPM.
         * 
         * @return The current fan speed.
         */
        virtual uint16_t getSpeed() = 0;

        /**
         * @brief Returns the configuration settings for the fan.
         *
         * This method retrieves the current configuration for the fan, including the PWM and tachometer settings.
         * 
         * @return The fan's configuration.
         */
        virtual const FanConfig::Config& getConfig() = 0;
};

