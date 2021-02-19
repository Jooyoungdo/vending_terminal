#include <iostream>
#include "terminal.h"

// Door lock controll pin numbers.. (up to device setting)
#define LOCK_SENSOR 42
#define DOOR_SENSOR 39
#define LOCK_TRIGGER 12

#define TEST_DEBUG

/*
 * argument vector lists : sudo ./daemon_process [device_id] [cam count] [cam device numbers ....] [ip address]
 */
int main(int argc, char** argv) {
    // user argument parsing
#ifdef TEST_DEBUG
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
    
    

    //create terminal object and initialize mqtt, MySQl
    terminal T(broker_ip, 0, device_id, device_id, LOCK_SENSOR, DOOR_SENSOR, LOCK_TRIGGER);
    T.initialize_mqtt_client();
    T.initialize_MySQL_connector();
    T.start_daemon();

    //start vending terminal until q pressed
    std::cout << "||||||||||||[START BEYLESS VENDING TERMINAL]||||||||||||" << std::endl;
    std::cout << "press 'q' to exit" << std::endl;
    while(std::tolower(std::cin.get()) != 'q')
        ;

    return 0;
}