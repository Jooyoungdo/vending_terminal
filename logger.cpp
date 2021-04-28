//
// Created by root on 20. 6. 19..
//

#include "logger.h"
#include <iostream>
#include <fstream>

// default initializer with log host
logger::logger(std::string _log_host) {
    this->log_host = _log_host;
    this->log_file_name_ = LOGFILE_ROOT_PATH + "log/" + currentDateTime() + "_log.txt";
    this->cfg_file_name_ = LOGFILE_ROOT_PATH + "log/log_enable.cfg";
    this->save_log = get_savelog_config();
}


// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string logger::currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);

    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

// print log with [log host] and [current time]
void logger::print_log(std::string log_body) {
    char out_log[1000];
    sprintf(out_log, "[%s][%s] %s", currentDateTime().c_str(), this->log_host.c_str(), log_body.c_str());
    std::cout << out_log << std::endl;
    if(this->save_log){
        save_log_to_file(out_log);
    } 
}

void logger::save_log_to_file(const char *log){
    std::ofstream log_stream;
    log_stream.open(log_file_name_,std::ios_base::out | std::ios_base::app);
    log_stream << log << std::endl;
    log_stream.close();
}

bool logger::get_savelog_config(){
    std::ifstream file_stream(this->cfg_file_name_);
    return file_stream.is_open();
}
