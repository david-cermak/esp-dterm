#include <string>
#include "nvs.h"

class Commands 
{
public:
    Commands();
    void Process(std::string& cmd);
    void GetConfig(bool &mqtt_on, bool &forward_on);
    void SetConfig(bool mqtt_on, bool forward_on);
    void Save();
private:
    uint32_t m_config;
    nvs_handle m_handle;
};