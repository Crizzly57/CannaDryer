#pragma once

#include <memory>
#include "cJSON.h"

#include "mongoose_manager.hpp"

#include "config.hpp"
#include "FanControl/fan_manager.hpp"
#include "FanControl/ifan.hpp"

using namespace std;

constexpr char FAN_ENDPOINT[] = "/fan";
constexpr char FAN_MANAGER_ENDPOINT[] = "/fanManager";
constexpr char SCRIPTS_ENDPOINT[] = "/scripts.js";
constexpr char STYLES_ENDPOINT[] = "/styles.css";
constexpr char INDEX_PATH[] = "/spiffs/index.html";

/**
 * @class WebServer
 * @brief A class to handle web server operations.
 */
class WebServer {
    public:
        /**
         * @brief Constructs a WebServer object.
         * @param fanManager Reference to a FanManager object.
         * @param port The port number to listen on (default is "8000").
         */
        WebServer(FanManager& fanManager, const char *port = "8000");

        /**
         * @brief Starts the web server.
         */
        void start();

        /**
         * @brief Stops the web server.
         */
        void stop();

    private:
        static struct mg_mgr mgr; ///< Mongoose event manager.
        const char* _port; ///< Port number to listen on.
        FanManager& _fanManager; ///< Reference to the FanManager.
        bool _running; ///< Indicates if the server is running.

        /**
         * @brief Handles incoming HTTP requests.
         * @param connection Pointer to the connection.
         * @param event Event type.
         * @param event_data Event data.
         */
        static void handle_request(struct mg_connection *connection, int event, void *event_data);

        /**
         * @brief Handles fan data retrieval requests via HTTP GET.
         * 
         * This method processes incoming GET requests to retrieve data for a specific fan. 
         * It identifies the requested fan by name, validates the request parameters, and sends 
         * the corresponding fan data as an HTTP response. If the fan is not found or the request 
         * is invalid, an appropriate HTTP error response is sent.
         * 
         * @param http_message Pointer to the HTTP message containing the request details.
         * @param connection Pointer to the current HTTP connection.
         */
        void handleFanDataRequest(struct mg_connection* connection, struct mg_http_message* http_message);

        /**
         * @brief Handles updates to fan-related data via HTTP POST.
         * 
         * This method processes incoming POST requests to update fan parameters, such as speed.
         * It validates the request, extracts the required data, and applies the changes to the 
         * specified fan. If the request is invalid or the fan is not found, an appropriate 
         * HTTP response is sent.
         * 
         * @param connection Pointer to the current HTTP connection.
         * @param http_message Pointer to the HTTP message containing the POST data.
         */
        void handleFanDataUpdate(struct mg_connection* connection, struct mg_http_message* http_message);

        /**
         * @brief Serves a static file.
         * @param connection Pointer to the connection.
         * @param http_message Pointer to the HTTP message.
         * @param filePath Path to the static file.
         */
        void serveStaticFile(struct mg_connection* c, struct mg_http_message* http_message, const string& filePath);

        /**
         * @brief Handles fan manager data retrieval requests via HTTP GET.
         * 
         * This method processes incoming GET requests to retrieve data for the fan manager. 
         * It sends the fan manager data as an HTTP response in JSON format.
         * 
         * @param connection Pointer to the current HTTP connection.
         * @param http_message Pointer to the HTTP message containing the request details.
         */
        void handleFanManagerDataRequest(struct mg_connection* connection, struct mg_http_message* http_message);

        /**
         * @brief Handles updates to fan manager data via HTTP POST.
         * 
         * This method processes incoming POST requests to update fan manager parameters, such as interval.
         * 
         * @param connection Pointer to the current HTTP connection.
         * @param http_message Pointer to the HTTP message containing the POST data.
         */
        void handleFanManagerDataUpdate(struct mg_connection* connection, struct mg_http_message* http_message);

        /**
         * @brief Gets the MIME type of a file based on its path.
         * @param filePath Path to the file.
         * @return MIME type as a const char*.
         */
        const char* getMimeType(string_view filePath);

        /**
         * @brief Extracts a JSON parameter from an HTTP message.
         * 
         * This template function parses the JSON body of an HTTP message and retrieves the value 
         * associated with the specified key. The value is automatically cast to the type specified 
         * by the template parameter `T`. Supported types include `int`, `double`, and `std::string`.
         * 
         * @tparam T The expected type of the JSON value.
         * @param http_message Pointer to the HTTP message containing the JSON body.
         * @param key The key of the JSON parameter to retrieve.
         * @param value Reference where the extracted value will be stored.
         * @return True if the parameter was successfully retrieved and cast, false otherwise.
         */
        template<typename T>
        bool getJSONParam(const struct mg_http_message* http_message, const char* key, T& value);

        /**
         * @brief Gets a query parameter from the HTTP message.
         * @param http_message Pointer to the HTTP message.
         * @param key Key of the query parameter.
         * @param value Buffer to store the value.
         * @param valueSize Size of the buffer.
         * @return True if the parameter was found, false otherwise.
         */
        bool getQueryParam(const struct mg_http_message* http_message, const char* key, char* value, size_t valueSize);
};