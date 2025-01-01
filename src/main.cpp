#include "esp_spiffs.h"
#include "nvs_flash.h"

#include "FanControl/fan_manager.hpp"
#include "Network/server.hpp"
#include "Network/wifi_manager.hpp"
#include "config.hpp"

FanManager fanManager;

void fanTask(void* pvParameters) {
    fanManager.createFan(FanConfig::FAN_FRONT);
    fanManager.createFan(FanConfig::FAN_BACK);
    fanManager.setInterval(FanConfig::INTERVAL);
    fanManager.setRuntimeOfFans(FanConfig::RUNTIME_OF_FANS);
    fanManager.initializeAllFans();
    fanManager.runTask();
}

void wifiManagerTask(void* pvParameters) {
    WiFiManager wifiManager;
    wifiManager.init();
    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void webServerTask(void* pvParameters) {
    // Initialize SPIFFS
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = nullptr,
        .max_files = 5,
        .format_if_mount_failed = false
    };
    
    if (esp_vfs_spiffs_register(&conf) != ESP_OK) {
        ESP_LOGE(TaskConfig::WEB_SERVER_TASK.tag, "Error mounting SPIFFS filesystem");
        return;
    }

    WebServer server(fanManager);
    server.start();
}

void setup() {
    // Install ISR-Service
    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    xTaskCreate(wifiManagerTask, TaskConfig::WIFI_TASK.tag, TaskConfig::WIFI_TASK.stackSize, nullptr, TaskConfig::WIFI_TASK.priority, nullptr);
    xTaskCreate(fanTask, TaskConfig::FAN_TASK.tag, TaskConfig::FAN_TASK.stackSize, nullptr, TaskConfig::FAN_TASK.priority, nullptr);
    xTaskCreate(webServerTask, TaskConfig::WEB_SERVER_TASK.tag, TaskConfig::WEB_SERVER_TASK.stackSize, nullptr, TaskConfig::WEB_SERVER_TASK.priority, nullptr);
}

extern "C" void app_main() {
    setup();
}