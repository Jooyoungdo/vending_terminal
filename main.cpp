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
#ifdef DEBUG_BAIVE
    std::string device_id = std::string("20002");
    std::string broker_ip = std::string("ai0.beyless.com:1883");
#else
    if (argc < 3){
        std::cout << "not enough user argument." << std::endl <<
        "argument vector lists : sudo ./daemon_process [device_id] [cam count] [cam device numbers ....] [ip address]" << std::endl;
        exit(0);
    }
    std::string device_id = argv[1];
    std::string broker_ip = argv[argc-1];
#endif
    //get board info
    
    std::string target_board= get_device_name();
    std::string serial_number = get_device_serial_number();
    std::string project_verion = get_project_version();
    //create terminal object and initialize mqtt, MySQl
    terminal T(broker_ip, 0, device_id, device_id, target_board);
    T.initialize_mqtt_client();
#ifndef DEBUG_BAIVE    
    if(!T.initialize_MySQL_connector()){
        std::cout << "Can't initialize mysql, it is only for test" << std::endl;
    }
#endif
    
    
    T.start_daemon(serial_number, project_verion);
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