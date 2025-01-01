#pragma once

#include <cstring>

#include "esp_wifi.h"
#include "esp_log.h"

#include "config.hpp"

using namespace std;

/**
 * @class WiFiManager
 * @brief A class to manage Wi-Fi connection and provide IP address information.
 * 
 * This class handles the initialization of the Wi-Fi connection using ESP32's Wi-Fi APIs. 
 * It also provides methods to retrieve the IP address assigned to the device once connected 
 * to a Wi-Fi network.
 */
class WiFiManager {
    public:
        /**
         * @brief Initializes the Wi-Fi connection.
         * 
         * This method configures the Wi-Fi settings and attempts to connect to the 
         * specified network. It also registers the necessary event handlers for Wi-Fi events.
         */
        void init();

    private:
        /**
         * @brief Wi-Fi event handler for managing Wi-Fi connection events.
         * 
         * This static method is used as an event handler for Wi-Fi events such as connection success 
         * or failure. It helps in managing the state of the Wi-Fi connection.
         * 
         * @param arg A pointer to user-specific data (typically not used here).
         * @param event_base The event base identifier.
         * @param event_id The event identifier.
         * @param event_data Additional data associated with the event.
         */
        static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

        /**
         * @brief Static variable to keep track of the number of retry attempts.
         */
        static int retry_count;
};

