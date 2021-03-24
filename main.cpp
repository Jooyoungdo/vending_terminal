#include <iostream>
#include "terminal.h"
#include "debug.h"
#include <stdlib.h>
#include "sound_manager.h"
#include "vendor_storage.h"

std::string get_target_board_name(){
    //TODO: 하드코딩된 부분을 디바이스 정보를 읽어서 구현하는 방식으로 해야 함
    return "DEEPTHINK";
}

std::string read_sn(){
    char data_0[128] = {0};
    rkvendor_read(VENDOR_SN_ID, data_0, sizeof(data_0)/sizeof(data_0[0]));
	fprintf(stderr, "DEMO-%d: read vendor sn: %s\n", __LINE__, data_0);
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
    std::string target_board= get_target_board_name();
    read_sn();
    //create terminal object and initialize mqtt, MySQl
    terminal T(broker_ip, 0, device_id, device_id, target_board);
    T.initialize_mqtt_client();
#ifndef DEBUG_BAIVE    
    T.initialize_MySQL_connector();
#endif

    T.start_daemon();

    //start vending terminal until q pressed
    //std::cout << "||||||||||||[START BEYLESS VENDING TERMINAL]||||||||||||" << std::endl;
    std::cout << "Start Baive Terminal Program..." << std::endl;
    std::cout << "press 'q' to exit" << std::endl;
    while(std::tolower(std::cin.get()) != 'q');
    T.exit_program(true);
    std::cout << "Exit Baive Terminal Program..." << std::endl;

    return 0;
}