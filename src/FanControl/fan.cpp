#include "FanControl/fan.hpp"

// Constructor to initialize pin variables
Fan::Fan(const FanConfig::Config& config)
    : config(config),
      _counterRPM(0),
      _lastRPM(0),
      _lastTachoMeasurement(0) {}

// Initialize PWM for fan control
void Fan::initPWM() {
    ledc_channel_config_t channel_config = {
        .gpio_num = config.pwmPin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = config.channel,
        .intr_type = LEDC_INTR_DISABLE,
        .duty = FanConfig::MAX_DUTY,
        .hpoint = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel_config));

    ledc_timer_config_t timer_config = {};
    timer_config = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = config.timer,
        .freq_hz = 25000,   // 25kHz
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));
}

// Initialize tachometer for RPM measurement
void Fan::initTacho() {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.pin_bit_mask = (1ULL << config.tachoPin);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    ESP_ERROR_CHECK(gpio_isr_handler_add(config.tachoPin, rpmFanISR, this));
    ESP_LOGI(TaskConfig::FAN_TASK.tag, "Tachometer initialized on GPIO %d", config.tachoPin);
};

// Interrupt Service Routine (ISR) for counting fan rotations (placed in IRAM)
void Fan::rpmFanISR(void* arg) {
    Fan* fanControl = static_cast<Fan*>(arg);
    fanControl->_counterRPM++;
};

uint16_t Fan::getSpeed() {
    unsigned long currentTime = esp_timer_get_time() / 1000;

    if ((currentTime - _lastTachoMeasurement) >= FanConfig::TACHO_UPDATE_CYCLE) {
        // Temporarily disable interrupts while calculating RPM
        gpio_intr_disable(config.tachoPin); 

        _lastRPM = _counterRPM * (60.0 / FanConfig::NUMBER_OF_INTERRUPS_IN_ONE_SINGLE_ROTATION) * (1000.0 / FanConfig::TACHO_UPDATE_CYCLE);

        _counterRPM = 0;
        _lastTachoMeasurement = currentTime;

        // Re-enable interrupts
        gpio_intr_enable(config.tachoPin);
    }

    return _lastRPM;
}

// Set fan speed using PWM duty cycle (0-255)
void Fan::setPower(uint8_t percent) {
    int dutyCycle = static_cast<int>(percent * static_cast<float>(FanConfig::MAX_DUTY) / 100.0f);

    // Update the current config
    config.fanPower = percent;

    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, config.channel, dutyCycle));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, config.channel));
}

// Returns the configuration of the fan
const FanConfig::Config& Fan::getConfig() {
    return config;
}
