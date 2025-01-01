#include "FanControl/fan_manager.hpp"

void FanManager::createFan(const FanConfig::Config& config) {
    _fans[config.name] = make_shared<Fan>(config);
}

void FanManager::initializeAllFans() {
    for (auto& [name, fan] : _fans) {
        fan->initPWM();
        fan->initTacho();
    }
}

void FanManager::runTask() {
    while (true) {
        startFans();

        vTaskDelay((_runtimeOfFans * 1000) / portTICK_PERIOD_MS);

        logFanSpeeds();

        stopFans();

        vTaskDelay((_interval * 1000) / portTICK_PERIOD_MS);
    }
}

void FanManager::logFanSpeeds() {
    for (auto& [name, fan] : _fans) {
        ESP_LOGI(TaskConfig::FAN_TASK.tag, "Fan Speed %s: %d", fan->getConfig().name, fan->getSpeed());
    }
}

void FanManager::stopFans() {
    for (auto& [name, fan] : _fans) {
        fan->setPower(0);
    }
}

void FanManager::startFans() {
    for (auto& [name, fan] : _fans) {
        fan->setPower(fan->getConfig().fanPower);
    }
}

void FanManager::setInterval(uint16_t new_interval) {
    _interval = new_interval;
}

uint16_t FanManager::getInterval() const {
    return _interval;
}

void FanManager::setRuntimeOfFans(uint16_t new_runtimeOfFans) {
    _runtimeOfFans = new_runtimeOfFans;
}

uint16_t FanManager::getRuntimeOfFans() const {
    return _runtimeOfFans;
}

optional<shared_ptr<IFan>> FanManager::getFan(const string& name) const {
    auto it = _fans.find(name);
    if (it != _fans.end()) {
        return static_pointer_cast<IFan>(it->second);
    }
    return nullopt;
}

unordered_map<string, shared_ptr<IFan>> FanManager::getFans() const {
    unordered_map<string, shared_ptr<IFan>> fans;
    for (const auto& [name, fan] : _fans) {
        fans[name] = static_pointer_cast<IFan>(fan);
    }
    return fans;
}