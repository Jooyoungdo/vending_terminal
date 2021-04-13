#include <iostream>
#include "terminal.h"
#include "debug.h"
#include <stdlib.h>
#include "sound_manager.h"


std::string get_device_name(){
    //TODO: 하드코딩된 부분을 디바이스 정보를 읽어서 구현하는 방식으로 해야 함
    return "DEEPTHINK";
}

std::string get_device_serial_number(){

    //TODO: get sn 
    return "BAIVE_DT_00.20002";   
}

std::string get_broker_ip(){

    //TODO: get broker ip 
    //TODO: how?
    return "ai0.beyless.com:1883";   
}


std::string get_device_id(){
    std::string serial_number = get_device_serial_number();
    
    int previous =0;
    int current=0;
    std::vector<std::string> parse_string;
    parse_string.clear();
   
    current= serial_number.find('.');
    
    while (current != std::string::npos){
        std::string substring = serial_number.substr(previous, current - previous);
        parse_string.push_back(substring);
        previous = current + 1;
        current = serial_number.find(',', previous);
    }
    
    parse_string.push_back(serial_number.substr(previous,current-previous));
    if(parse_string.size() > 0){
        return parse_string.back();   
    }else{
        std::cout << "can't find device id" << std::endl;
        return "";
    }
    
}


std::string get_project_version(){
    std::string major_ver = std::to_string(PROJECT_VERSION_MAJOR);
    std::string minor_ver = std::to_string(PROJECT_VERSION_MINOR);
    std::string patch_ver = std::to_string(PROJECT_VERSION_PATCH);
    std::string project_version = major_ver + "." + minor_ver + "." + patch_ver;

    std::cout << project_version << std::endl;

    return project_version;
}



// Door lock controll pin numbers.. (up to device setting)

/*
 * argument vector lists : sudo ./daemon_process [device_id] [cam count] [cam device numbers ....] [ip address]
 */
int main(int argc, char** argv) {
    // user argument parsing
    std::string device_id ;
    std::string broker_ip ;

    if (argc < 3){
        device_id = get_device_id();
        broker_ip = get_broker_ip();
        std::cout << "not enough user argument"<< std::endl;
        std::cout << "read device id from serial number :" << device_id << std::endl;
        std::cout << "read broker ip fromn saved info :" << broker_ip << std::endl;
    }else{
        device_id = argv[1];
        broker_ip = argv[argc - 1];
    }
    

    //create terminal object and initialize mqtt, MySQl
    terminal T(broker_ip, 0, device_id, device_id,
               get_device_name(),
               get_device_serial_number(),
               get_project_version()
    );
    T.initialize_mqtt_client();
#ifndef DEBUG_BAIVE    
    if(!T.initialize_MySQL_connector()){
        std::cout << "Can't initialize mysql, it is only for test" << std::endl;
    }
#endif
    
    
    T.start_daemon();
    //T.update_device_info(,get_project_version());
    //start vending terminal until q pressed
    //std::cout << "||||||||||||[START BEYLESS VENDING TERMINAL]||||||||||||" << std::endl;
    std::cout << "Start Baive Terminal Program..." << std::endl;
    std::cout << "press 'q' to exit" << std::endl;
    while(std::tolower(std::cin.get()) != 'q');
    T.stop_daemon();
    std::cout << "Exit Baive Terminal Program..." << std::endl;

    return 0;
}