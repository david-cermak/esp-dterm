#include "mqtt.h"

static std::function<void(std::string)> onmessage = NULL;
static Mqtt *instance = NULL;

esp_err_t Mqtt::mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    // esp_mqtt_client_handle_t client = event->client;
    // your_context_t *context = event->context;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            if (instance) {
                esp_mqtt_client_subscribe(instance->m_client, instance->m_subscribe.c_str(), 0);
                printf("Subscibing to topic: %s\n",instance->m_subscribe.c_str() );
            }
            break;
        case MQTT_EVENT_DISCONNECTED:
            break;
        case MQTT_EVENT_SUBSCRIBED:
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            break;
        case MQTT_EVENT_PUBLISHED:
            break;
        case MQTT_EVENT_DATA:
            {
                printf("DATA=%.*s\r\n", event->data_len, event->data);
                std::string data(event->data, event->data_len);
                onmessage(data);
            }
            break;
        case MQTT_EVENT_ERROR:
            break;
    }
    return ESP_OK;
}


Mqtt::Mqtt(std::string broker, std::string subscribe, std::string publish)
:m_publish(publish), m_broker(broker), m_subscribe(subscribe)
{
}


void Mqtt::Start()
{
    esp_mqtt_client_config_t cfg;

    memset(&cfg, 0, sizeof(cfg));
    cfg.uri = m_broker.c_str();
    cfg.event_handle = mqtt_event_handler;

    m_client = esp_mqtt_client_init(&cfg);
    esp_mqtt_client_start(m_client);
    instance = this;
}


void Mqtt::Publish(std::string& data)
{
    esp_mqtt_client_publish(m_client, m_publish.c_str(),  data.c_str(), data.length(), 0, 0);
}

void Mqtt::OnReceive(std::function<void(std::string)> callback)
{
    onmessage = callback;
}
