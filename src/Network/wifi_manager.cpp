#include "Network/wifi_manager.hpp"

int WiFiManager::retry_count = 0;

void WiFiManager::init() {
    // Initialize network interface
    esp_netif_init();
    esp_event_loop_create_default();

    // Create default WiFi station (STA) interface
    esp_netif_t* wifi_sta_netif = esp_netif_create_default_wifi_sta();

    // Initialize WiFi with default configuration
    wifi_init_config_t wifiConfig = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifiConfig));

    // Register event handlers for WiFi and IP events
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    wifi_config_t wifi_config = {};
    // Copy SSID and Password from the configuration
    strncpy((char*)wifi_config.sta.ssid, Network::WIFI_SSID, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, Network::WIFI_PASS, sizeof(wifi_config.sta.password) - 1);
    // Ensure the strings are null-terminated
    wifi_config.sta.ssid[sizeof(wifi_config.sta.ssid) - 1] = '\0';
    wifi_config.sta.password[sizeof(wifi_config.sta.password) - 1] = '\0';

    // Set WiFi mode to station (STA) and configure WiFi
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Connect to the WiFi network
    ESP_ERROR_CHECK(esp_wifi_connect());

    // Setup static IP configuration
    esp_netif_ip_info_t static_ip_info;
    ESP_ERROR_CHECK(esp_netif_str_to_ip4(Network::STATIC_IP, &static_ip_info.ip));
    ESP_ERROR_CHECK(esp_netif_str_to_ip4(Network::GATEWAY, &static_ip_info.gw));
    ESP_ERROR_CHECK(esp_netif_str_to_ip4(Network::SUBNET, &static_ip_info.netmask));

    // Stop DHCP and assign static IP
    ESP_ERROR_CHECK(esp_netif_dhcpc_stop(wifi_sta_netif));
    ESP_ERROR_CHECK(esp_netif_set_ip_info(wifi_sta_netif, &static_ip_info));
}

void WiFiManager::wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    // Handle different WiFi and IP events
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TaskConfig::WIFI_TASK.tag, "Connecting...");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        // Reset retry count
        retry_count = 0;
        ESP_LOGI(TaskConfig::WIFI_TASK.tag, "Connected");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TaskConfig::WIFI_TASK.tag, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        retry_count++;
        int delay_time = (1 << retry_count) * 1000;
        delay_time = delay_time > 30000 ? 30000 : delay_time; // Max. 30s
        
        ESP_LOGW(TaskConfig::WIFI_TASK.tag, "Disconnected. Retrying...");
        vTaskDelay(delay_time / portTICK_PERIOD_MS);
        esp_wifi_connect();
    }
}
