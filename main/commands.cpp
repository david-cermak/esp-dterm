#include "commands.h"
#include "nvs_flash.h"

#include <iostream>


bool Commands::GetString(const char * key, std::string& str)
{
    size_t len;
    esp_err_t err = nvs_get_str(m_handle, key, NULL, &len);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        std::cout << "Key: " << key << " not found in NVS!"  << std::endl;
    } else if (err != ESP_OK) {
        std::cout << "Error:" <<  esp_err_to_name(err) <<  "saving the config word!" << std::endl;
    } else {
        str.reserve(len);
        err = nvs_get_str(m_handle, key, (char*) str.c_str(), &len);
        if (err == ESP_OK) {
            std::cout  << key <<  "=" << str << std::endl;
            return true;
        } else {
            std::cout << "Error:" <<  esp_err_to_name(err) <<  "saving the config word!" << std::endl;
        }
    }
    return false;
}

Commands::Commands()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
    std::cout << "Opening Non-Volatile Storage (NVS) handle... " << std::endl;
    err = nvs_open("storage", NVS_READWRITE, &m_handle);
    if (err != ESP_OK) {
        std::cout << "Error:" <<  esp_err_to_name(err) <<  "opening NVS handle!" << std::endl;
        // Storage not available, continue with configuration from io
    } else {
        std::cout << "Done"  << std::endl;

        // reading basic configuration
        err = nvs_get_u32(m_handle, "config", &m_config);
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            std::cout << "Config not initialized yet!\n"  << std::endl;
        } else if (err != ESP_OK) {
            std::cout << "Error:" <<  esp_err_to_name(err) <<  "saving the config word!" << std::endl;
        }
    }
}


bool Commands::GetCredentials(std::string& ssid, std::string& password)
{
    if (GetString("ssid", ssid) && GetString("password", password)) {
        return true;
    }
    return false;
}

void Commands::GetConfig(bool &mqtt_on, bool &forward_on)
{
    mqtt_on = m_config&0x1;
    forward_on = m_config&0x2;
}

void Commands::SetConfig(bool mqtt_on, bool forward_on)
{
    m_config = mqtt_on?1:0;
    m_config |= forward_on?2:0;
}

void Commands::Save()
{
    esp_err_t err = nvs_set_u32(m_handle, "config", m_config);
    if (err != ESP_OK) {
        std::cout << "Error:" <<  esp_err_to_name(err) <<  "saving the config word!" << std::endl;
    }
}

void Commands::Process(std::string& cmd)
{

}