//
// Created by root on 20. 6. 19..
//

#include "logger.h"
#include <iostream>
#include <fstream>

// default initializer with log host
logger::logger(std::string _log_host) {
    this->log_host = _log_host;
    //this->log_file_name_ = LOGFILE_ROOT_PATH + "log/" + currentDateTime() + "_log.txt";
    this->cfg_file_name_ = LOGFILE_ROOT_PATH + "log/setting.json";
    get_savelog_config(this->cfg_file_name_);
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
    if(this->save_log_){
        save_log_to_file(out_log);
    } 
}

void logger::save_log_to_file(const char *log){
    std::ofstream log_stream;
    log_stream.open(log_file_name_,std::ios_base::out | std::ios_base::app);
    log_stream << log << std::endl;
    log_stream.close();
}

void logger::get_savelog_config(std::string cfg_file_name){
    std::ifstream file_stream(cfg_file_name);
    if(file_stream.is_open()){
        rapidjson::IStreamWrapper stream_wrapper(file_stream);
        rapidjson::Document json_doc;
        json_doc.ParseStream(stream_wrapper);
        for (auto& json_data : json_doc.GetArray()){
            try {
                this->save_log_ = json_data["log_to_file"].GetBool();
                this->log_file_name_ = json_data["log_file_name"].GetString();
            } catch (int e) {
                std::cout << "parsing json error!!" << std::endl;
                this->save_log_ = false;
                this->log_file_name_ ="";
            }
        }
    }else{
        this->save_log_ = false;
        return;
    }
    
}
