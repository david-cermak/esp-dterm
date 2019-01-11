#include <string>
#include "nvs.h"

class Commands 
{
public:
    Commands();
    void Process(std::string& cmd);
    void GetConfig(bool &mqtt_on, bool &forward_on);
    void SetConfig(bool mqtt_on, bool forward_on);
    bool GetCredentials(std::string& ssid, std::string& password);
    bool SaveCredentials(std::string& ssid, std::string& password);


    void Save();
private:
    bool SaveString(const char * key, std::string& str);
    bool GetString(const char * key, std::string& str);

    uint32_t m_config;
    nvs_handle m_handle;

    
};