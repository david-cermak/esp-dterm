#include "mqtt_client.h"
#include <string>
#include <functional>


class Mqtt {
    public:
        Mqtt(std::string broker, std::string subscribe, std::string publish);

        void Publish(std::string& data);

        void Start();

        void OnReceive(std::function<void(std::string)> callback);
        
    private:
        static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);

        esp_mqtt_client_handle_t m_client;
        std::string m_publish;
        std::string m_broker;
        std::string m_subscribe;
};