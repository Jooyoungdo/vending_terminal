//
// Created by root on 20. 6. 19..
//

#ifndef DAEMON_PROCESS_LOGGER_H
#include <iostream>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#define DAEMON_PROCESS_LOGGER_H


class logger {
private:
    std::string log_host; // log host

    //const std::string LOGFILE_ROOT_PATH = "/mnt/d/Beyless/0.project/2.firefly_rk3399/src/beyless_vending_terminal/";
    const std::string LOGFILE_ROOT_PATH = "/home/firefly/beyless_vending_terminal/";
    std::string log_file_name_;
    std::string cfg_file_name_;
    void save_log_to_file(const char *log);
    void get_savelog_config(std::string cfg_file_name);
    bool save_log_;
public:
    logger(std::string _log_host); // default initializer with log host

    // Get current date/time, format is YYYY-MM-DD.HH:mm:ss
    const std::string currentDateTime();

   	// print log with [log host] and [current time]
    void print_log(std::string log_body);

};


#endif //DAEMON_PROCESS_LOGGER_H
