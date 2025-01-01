#include "Network/server.hpp"

#include "esp_spiffs.h"
#include <stdio.h>

WebServer::WebServer(FanManager& fanManager, const char* port) 
    : _port(port), 
      _fanManager(fanManager) {}

void WebServer::start() {
    MongooseManager mongooseManager;

    // Construct the URL for the web server, including IP address and port
    string url = "http://" + string(Network::STATIC_IP) + ":" + _port;

    const char* url_cstr = url.c_str();
    if (url_cstr == nullptr) {
        ESP_LOGE(TaskConfig::WEB_SERVER_TASK.tag, "URL construction failed!");
        return;
    }
    struct mg_connection *connection = mg_http_listen(&mongooseManager.getManager(), url_cstr, handle_request, this);
    if (connection == nullptr) {
        ESP_LOGE(TaskConfig::WEB_SERVER_TASK.tag, "Failed to create listener!");
        return;
    }
    connection->fn_data = this;

    ESP_LOGI(TaskConfig::WEB_SERVER_TASK.tag, "Web server started at %s", url_cstr);

    // Set the server to running and enter the event loop
    _running = true;
    while(_running) {
        mg_mgr_poll(&mongooseManager.getManager(), 1000);
    }
}

void WebServer::stop() {
    // Stop the server by setting the running flag to false
    _running = false;
}

void WebServer::handle_request(struct mg_connection *connection, int event, void *event_data) {
    WebServer* server = static_cast<WebServer*>(connection->fn_data);
    
    switch (event)
    {
    // Handle a new HTTP request
    case MG_EV_HTTP_MSG:
        // Extract the HTTP message
        struct mg_http_message *http_message = (struct mg_http_message *) event_data;

        // Check the URI and serve the appropriate response
        if (mg_match(http_message->uri, mg_str(FAN_ENDPOINT), nullptr)) {
            if (mg_strcmp(http_message->method, mg_str("GET")) == 0) {
                server->handleFanDataRequest(connection, http_message);
            } else if (mg_strcmp(http_message->method, mg_str("POST")) == 0) {
                server->handleFanDataUpdate(connection, http_message);
            } else {
                mg_http_reply(connection, 405, "", "Method not allowed\n");
            }
        }
        else if (mg_match(http_message->uri, mg_str(FAN_MANAGER_ENDPOINT), nullptr)) {
            if (mg_strcmp(http_message->method, mg_str("GET")) == 0) {
                server->handleFanManagerDataRequest(connection, http_message);
            } else if (mg_strcmp(http_message->method, mg_str("POST")) == 0) {
                server->handleFanManagerDataUpdate(connection, http_message);
            } else {
                mg_http_reply(connection, 405, "", "Method not allowed\n");
            }
        }
        else if (mg_match(http_message->uri, mg_str(SCRIPTS_ENDPOINT), nullptr)) {
            string fullPath = string(SPIFFSConfig::SPIFFS_BASE_PATH) + string(SCRIPTS_ENDPOINT);
            server->serveStaticFile(connection, http_message, fullPath);
        }
        else if (mg_match(http_message->uri, mg_str(STYLES_ENDPOINT), nullptr)) {
            string fullPath = string(SPIFFSConfig::SPIFFS_BASE_PATH) + string(STYLES_ENDPOINT);
            server->serveStaticFile(connection, http_message, fullPath);
        }
        else {
            server->serveStaticFile(connection, http_message, string(INDEX_PATH));
        }

        break;
    }
}

void WebServer::handleFanDataUpdate(struct mg_connection* connection, struct mg_http_message* http_message) {
    // Retrieve the fan name from the query parameters
    char fanName[32];
    if (!getQueryParam(http_message, "name", fanName, sizeof(fanName))) {
        mg_http_reply(connection, 400, "", "Missing 'name' query parameter\n");
        return;
    }
    
    // Retrieve the fan instance using the fan name
    auto fan = _fanManager.getFan(fanName);
    if (!fan) {
        mg_http_reply(connection, 404, "", "Fan not found\n");
        return;
    }

    int power;
    // If the power is provided in the request, set it for the fan
    if (getJSONParam(http_message, "power", power) && power >= 0 && power <= 100) {
        fan->get()->setPower(power);
        ESP_LOGI(TaskConfig::WEB_SERVER_TASK.tag, "Fan %s speed set to %d%%", fanName, power);
        mg_http_reply(connection, 200, "", "Power set successfully\n");
    } else {
        ESP_LOGE(TaskConfig::WEB_SERVER_TASK.tag, "Invalid 'power' parameter");
        mg_http_reply(connection, 400, "", "Invalid 'power' parameter\n");
    }
}

void WebServer::handleFanDataRequest(struct mg_connection* connection, struct mg_http_message* http_message) {
    // Create a JSON array
    cJSON* jsonArray = cJSON_CreateArray();

    // Iterate through all fans and add their data to the JSON array
    for (const auto& [name, fan] : _fanManager.getFans()) {
        cJSON* fanObject = cJSON_CreateObject();
        
        // Add fan data to the JSON object
        cJSON_AddStringToObject(fanObject, "name", name.c_str());
        cJSON_AddNumberToObject(fanObject, "speed", fan->getSpeed());
        cJSON_AddNumberToObject(fanObject, "power", fan->getConfig().fanPower);
        
        // Add the fan object to the JSON array
        cJSON_AddItemToArray(jsonArray, fanObject);
    }

    // Convert the JSON array to a string
    char* jsonString = cJSON_PrintUnformatted(jsonArray);

    // Send the JSON response
    mg_http_reply(connection, 200, "Content-Type: application/json\r\n", "%s", jsonString);

    // Clean up
    cJSON_Delete(jsonArray);
    free(jsonString);
}

void WebServer::handleFanManagerDataRequest(struct mg_connection* connection, struct mg_http_message* http_message) {
    // Create a JSON object
    cJSON* jsonObject = cJSON_CreateObject();

    // Add fan manager data to the JSON object
    cJSON_AddNumberToObject(jsonObject, "interval", _fanManager.getInterval());
    cJSON_AddNumberToObject(jsonObject, "runtimeOfFans", _fanManager.getRuntimeOfFans());

    // Convert the JSON object to a string
    char* jsonString = cJSON_PrintUnformatted(jsonObject);

    // Send the JSON response
    mg_http_reply(connection, 200, "Content-Type: application/json\r\n", "%s", jsonString);

    // Clean up
    cJSON_Delete(jsonObject);
    free(jsonString);
}

void WebServer::handleFanManagerDataUpdate(struct mg_connection* connection, struct mg_http_message* http_message) {
    // Retrieve the interval and runtime-of-fans values from the JSON body
    float interval, runtimeOfFans;

    if (!getJSONParam(http_message, "interval", interval) || !getJSONParam(http_message, "runtimeOfFans", runtimeOfFans)) {
        mg_http_reply(connection, 400, "", "Invalid JSON body\n");
        return;
    }

    // Update the fan manager with the new values
    _fanManager.setInterval(static_cast<uint16_t>(interval));
    _fanManager.setRuntimeOfFans(static_cast<uint16_t>(runtimeOfFans));

    // Send a success response
    mg_http_reply(connection, 200, "", "Fan manager updated successfully\n");
}

void WebServer::serveStaticFile(struct mg_connection* connection, struct mg_http_message* http_message, const string& filePath) {
    FILE* file = fopen(filePath.c_str(), "rb");
    if (!file) {
        ESP_LOGE(TaskConfig::WEB_SERVER_TASK.tag, "Failed to open file: %s", filePath.c_str());
        mg_http_reply(connection, 404, "Content-Type: text/plain\r\n", "File read error\n");
        return;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read file contents into a buffer
    char* buffer = new char[size + 1];  // Allocate space for null-terminator
    size_t bytesRead = fread(buffer, 1, size, file);
    buffer[bytesRead] = '\0';  // Null-terminate the buffer

    if (bytesRead != size) {
        ESP_LOGE(TaskConfig::WEB_SERVER_TASK.tag, "Read %zu bytes instead of expected %zu bytes", bytesRead, size);
        mg_http_reply(connection, 500, "Content-Type: text/plain\r\n", "File read error\n");
    } else {
        string headers = "Content-Type: " + string(getMimeType(filePath.c_str())) /*+ "\r\nContent-Length: " + to_string(size)*/ + "\r\n";
        mg_http_reply(connection, 200, headers.c_str(), buffer, bytesRead);
    }

    // Clean up
    delete[] buffer;
    fclose(file);
}

bool WebServer::getQueryParam(const struct mg_http_message* http_message, const char* key, char* value, size_t valueSize) {
    // Get the value of a query parameter from the HTTP message
    if (mg_http_get_var(&http_message->query, key, value, valueSize) <= 0) {
        return false;
    }
    return true;
}

template<typename T>
bool WebServer::getJSONParam(const struct mg_http_message* http_message, const char* key, T& value) {
    // Parse JSON body with RAII for cleanup
    auto jsonDeleter = [](cJSON* json) { cJSON_Delete(json); };
    unique_ptr<cJSON, decltype(jsonDeleter)> json(
        cJSON_ParseWithLength(http_message->body.buf, http_message->body.len),
        jsonDeleter
    );

    if (!json) {
        ESP_LOGE(TaskConfig::WEB_SERVER_TASK.tag, "Failed to parse JSON body");
        return false;
    }

    // Get the JSON value for the given key
    cJSON* parsedValue = cJSON_GetObjectItemCaseSensitive(json.get(), key);
    if (!parsedValue) {
        ESP_LOGE(TaskConfig::WEB_SERVER_TASK.tag, "Missing JSON value for key: %s", key);
        return false;
    }

    // Type-specific handling
    if constexpr (is_same_v<T, int>) {
        if (!cJSON_IsNumber(parsedValue)) {
            ESP_LOGE(TaskConfig::WEB_SERVER_TASK.tag, "Expected number for key: %s", key);
            return false;
        }
        value = parsedValue->valueint;
    } else if constexpr (is_same_v<T, float>) {
        if (!cJSON_IsNumber(parsedValue)) {
            ESP_LOGE(TaskConfig::WEB_SERVER_TASK.tag, "Expected number for key: %s", key);
            return false;
        }
        value = static_cast<T>(parsedValue->valuedouble);
    } else {
        ESP_LOGE(TaskConfig::WEB_SERVER_TASK.tag, "Unsupported type for key: %s", key);
        return false;
    }

    return true;
}

const char* WebServer::getMimeType(string_view path) {
    // Determine the MIME type based on the file extension
    if (path.ends_with(".js")) return "application/javascript";
    if (path.ends_with(".css")) return "text/css";
    if (path.ends_with(".html")) return "text/html";
    return "text/plain";
}

