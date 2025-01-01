#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <string>

#include "freertos/FreeRTOS.h"

#include "config.hpp"
#include "ifan.hpp"
#include "FanControl/fan.hpp"

using namespace std;

/**
 * @class FanManager
 * @brief Manages the creation, initialization, and control of fans.
 * 
 * The FanManager class is responsible for managing multiple fans, including their creation,
 * initialization, and runtime control. It provides methods to start, stop, and log fan speeds,
 * as well as to set intervals and runtime durations for the fans.
 */
class FanManager {
    public:
        uint16_t getInterval() const;

        uint16_t getRuntimeOfFans() const;

        /**
         * @brief Creates a fan with the given configuration.
         * 
         * @param config The configuration settings for the fan.
         */
        void createFan(const FanConfig::Config& config);

        /**
         * @brief Initializes all fans managed by this FanManager.
         */
        void initializeAllFans();

        /**
         * @brief Runs the main task for managing fans.
         */
        void runTask();

        /**
         * @brief Retrieves a fan by its name.
         * 
         * @param name The name of the fan to retrieve.
         * @return An optional shared pointer to the fan, or null if not found.
         */
        optional<shared_ptr<IFan>> getFan(const string& name) const;

        /**
         * @brief Retrieves a map of fans.
         * 
         * This method returns an unordered map containing the fans, where the key is the fan name 
         * (as a string) and the value is a shared pointer to an object implementing the IFan interface.
         * 
         * @return An unordered map of fan names (keys) and corresponding shared pointers to IFan objects (values).
         */
        unordered_map<string, shared_ptr<IFan>> getFans() const;

        /**
         * @brief Sets the interval for fan operations.
         * 
         * @param new_interval The new interval value in milliseconds.
         */
        void setInterval(uint16_t new_interval);

        /**
         * @brief Sets the runtime duration for all fans.
         * 
         * @param new_runtimeOfFans The new runtime duration in milliseconds.
         */
        void setRuntimeOfFans(uint16_t new_runtimeOfFans);

    private:
        unordered_map<string, shared_ptr<Fan>> _fans;           ///< A map of fan names to fan objects.
        uint16_t _interval;                                     ///< The interval for fan operations in milliseconds.
        uint16_t _runtimeOfFans;                                ///< The runtime duration for all fans in milliseconds.

        /**
         * @brief Logs the speeds of all fans.
         */
        void logFanSpeeds();

        /**
         * @brief Stops all fans.
         */
        void stopFans();

        /**
         * @brief Starts all fans.
         */
        void startFans();
};
