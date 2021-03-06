//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <atomic>
#include <cstring>
#include <sstream>
#include <algorithm>
#include "mqtt.h"
#include "commands.h"


#define MAX_LINE 1024

int read_line(char * line);
void write_line(const char * line, int len);

std::atomic<bool> run_read(false);
bool forward = true;
bool mqtt_on = true;

Mqtt *pClient = NULL;

void MqttOnReceive(std::string data)
{
  write_line(data.c_str(), data.length());
  std::cout << "Received:" << data << std::endl;
}

void read_thread()
{
  int len = 0;
  char data[MAX_LINE];
  while (run_read.load()) {
    len = read_line(data);
    // std::cout << "Reading:" << len << std::endl;
    if (len>0) {
      data[len] = '\0';
      std::stringstream sstream(data);
      std::string publish_item;
      while(std::getline(sstream, publish_item, '\n')){
        publish_item.erase(std::remove(publish_item.begin(), publish_item.end(), '\r'), publish_item.end());
        if (pClient) {
          pClient->Publish(publish_item);
          // std::cout << "Sending:" << publish_item << std::endl;
        }
        if (forward) {
          std::cout << publish_item << std::endl;
        }
      }
    }
  }
}

char my_password[50];
char my_ssid[50];

extern void wifi_creds(char * wifi_ssid, char * wifi_password);

void dterm_main()
{
    std::string ssid;
    std::string pwd;

    char line[MAX_LINE] = { 0 };
    run_read = true;
    std::thread uart_reading(read_thread);
    Commands cmd;
    cmd.GetConfig(mqtt_on, forward);
    if (cmd.GetCredentials(ssid, pwd)) {
      strcpy(my_ssid, ssid.c_str());
      strcpy(my_password, pwd.c_str());
          wifi_creds(my_ssid, my_password);
    }

    Mqtt client(CONFIG_ESP_BROKER, CONFIG_ESP_SUBSCRIBE, CONFIG_ESP_PUBLISH);
    if (mqtt_on) {
      client.Start();
      pClient  = &client;
      client.OnReceive(std::bind(&MqttOnReceive, std::placeholders::_1));
    }

    while (true) {
      if (std::cin.getline(line, MAX_LINE)) {
        std::cout <<  line << std::endl;
        if (strcmp(line, "forwardon") == 0) {
          forward = true;
        } else if (strcmp(line, "forwardoff") == 0) {
          forward = false;
        } else if (strncmp(line, "ssid", sizeof("ssid")-1) == 0) {
          strcpy(my_ssid, line+sizeof("ssid ")-1);
        } else if (strncmp(line, "password", sizeof("password")-1) == 0) {
          strcpy(my_password, line+sizeof("password ")-1);
          wifi_creds(my_ssid, my_password);
          ssid = std::string(my_ssid);
          pwd = std::string(my_password);
          cmd.SaveCredentials(ssid, pwd);
          
        } else if (strcmp(line, "mqtton") == 0) {
          mqtt_on = true;
        } else if (strcmp(line, "mqttoff") == 0) {
          mqtt_on = false;
        } else if (strcmp(line, "save") == 0) {
          cmd.SetConfig(mqtt_on, forward);
          cmd.Save();
        } else {
          // Forwarding only if not a command
          if (forward) {
            std::cout <<  "forwarding:" << line << std::endl;
            size_t len = strlen(line);
            line[len] = '\n';
            line[len+1] = '\0';
            write_line(line , len+1);
          }
        }
      }
    }
    run_read = false;
}
