#include <iostream>
#include "terminal.h"
#include "debug.h"
#include <stdlib.h>
#include "sound_manager.h"
#include "vendor_storage.h"







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
    
    VendorStorage vendor_info;

    if (argc < 3){
        std::cout << "not enough user argument"<< std::endl;
        exit(1);
    }else{
        vendor_info.SetDeviceID(argv[1]);
        vendor_info.SetBrokerIP(argv[2]);
        //vendor_info.ParseDeviceInfo();
        device_id = vendor_info.GetDeviceID();
        broker_ip = vendor_info.GetBrokerIP();
    }
    

    //create terminal object and initialize mqtt, MySQl
    terminal T(broker_ip, 0, device_id, device_id,
               vendor_info.GetDeviceName(),
               vendor_info.GetSerialNumber(),
               get_project_version()
    );
    T.create_mqtt_connect_thread();
#ifndef DEBUG_BAIVE    
    if(!T.initialize_MySQL_connector()){
        std::cout << "Can't initialize mysql, it is only for test" << std::endl;
    }
#endif

    T.create_mqtt_message_thread();
    std::cout << "Start Baive Terminal Program..." << std::endl;
    std::cout << "press 'q' to exit" << std::endl;
    while(std::tolower(std::cin.get()) != 'q');
    T.stop_thread();
    std::cout << "Exit Baive Terminal Program..." << std::endl;

    return 0;
}