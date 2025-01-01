#pragma once

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#include "config_secrets.hpp"

namespace FanConfig {
    struct Config {
        const char* name; 
        gpio_num_t pwmPin;
        gpio_num_t tachoPin;
        ledc_channel_t channel; 
        ledc_timer_t timer;
        uint8_t fanPower;          // Max. Fan power in percent (0 - 100)
    };

    constexpr Config FAN_FRONT = {
            .name = "Front",
            .pwmPin = GPIO_NUM_27, 
            .tachoPin = GPIO_NUM_14, 
            .channel = LEDC_CHANNEL_0, 
            .timer = LEDC_TIMER_0,
            .fanPower = 60
        }; 

    constexpr Config FAN_BACK = {
            .name = "Back",
            .pwmPin = GPIO_NUM_32, 
            .tachoPin = GPIO_NUM_33, 
            .channel = LEDC_CHANNEL_1, 
            .timer = LEDC_TIMER_1,
            .fanPower = 70
        }; 

    constexpr uint8_t MAX_DUTY = 255;

    // How often should the fans run in seconds
    constexpr uint16_t INTERVAL = 600;

    // Duration how long the fans should run in seconds
    constexpr uint16_t RUNTIME_OF_FANS = 600;    

    // how often tacho speed shall be determined, in milliseconds
    constexpr uint16_t TACHO_UPDATE_CYCLE = 1000; 

    // Number of interrupts ESP32 sees on tacho signal on a single fan rotation. All the fans I've seen trigger two interrups.
    constexpr uint8_t NUMBER_OF_INTERRUPS_IN_ONE_SINGLE_ROTATION = 2;    
}

namespace Network {
    // Static IP-Data
    constexpr const char* STATIC_IP = "192.168.178.35";   // Static IP-Address
    constexpr const char* GATEWAY = "192.168.178.1";      // Gateway
    constexpr const char* SUBNET = "255.255.255.0";       // Subnetzmask
    constexpr const char* DNS = "192.168.178.100";        // DNS-Server

    constexpr const char* WIFI_SSID = WIFI_SSID_SECRET; // SSID from config_secrets.hpp
    constexpr const char* WIFI_PASS = WIFI_PASS_SECRET; // Password from config_secrets.hpp
}

namespace TaskConfig {
    // Struktur für Task-Konfigurationen
    struct TaskConfig {
        size_t stackSize;  // Stack-Größe
        UBaseType_t priority;  // Priorität
        const char* tag;  // Tag für den Task
    };

    constexpr TaskConfig WIFI_TASK = {
        .stackSize = 4096,
        .priority = 3,
        .tag = "WiFiManager"
    };

    constexpr TaskConfig FAN_TASK = {
        .stackSize = 4096,
        .priority = 4,
        .tag = "FanControl"
    };

    constexpr TaskConfig WEB_SERVER_TASK = {
        .stackSize = 4096,
        .priority = 5,
        .tag = "WebServer"
    };
}

namespace SPIFFSConfig {
    constexpr char SPIFFS_BASE_PATH[] = "/spiffs";
}
