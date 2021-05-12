//
// Created by changseok on 20. 6. 12..
//

#ifndef DAEMON_PROCESS_DOORLOCK_H
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <csignal>
#include <thread>
#include <chrono>
#include <ctime>
#include "debug.h"
#include "logger.h"
#include <memory.h>
#define DAEMON_PROCESS_DOORLOCK_H



/*
 * class doorLock manipulates door lock states and provide doorlock access APIs
 * enum lock_status
 * follows linux file auth system
 * lock|door|triger = 2^2|2^1|2^0 = 4|2|1
 */
enum lock_status{
    FIREFLY_UNLOCK_CLOSE = 0,
    FIREFLY_TRY_UNLOCK,
    FIREFLY_UNLOCK_OPEN,
    FIREFLY_TRIGGER_ERROR,
    FIREFLY_TRY_LOCK,
    FIREFLY_WAIT,
    FIREFLY_TRY_LOCK_ERROR,
    FIREFLY_LOCK_OPEN,
    DEEPTHINK_UNLOCK_CLOSE,
    DEEPTHINK_TRY_UNLOCK,
    DEEPTHINK_WAIT,
    DEEPTHINK_TRIGGER_ERROR,
    DEEPTHINK_UNLOCK_OPEN,
    DEEPTHINK_TRY_LOCK,
    DEEPTHINK_LOCK_OPEN,
    DEEPTHINK_TRY_LOCK_ERROR
    
};

class doorLock{
protected:
    // three integer values below indicates GPIO pin number
    int lock_num;
    int door_num;
    int trigger_num;
    int door_power_num;

    // three integer values below stores GPIO current value
    int lock_value;
    int door_value;
    int trigger_value;
    int door_power_value;

    // three integer values below stores actual pin fd
    std::fstream lock;
    std::fstream door;
    std::fstream trigger;
    std::fstream door_power;
    lock_status status;

    logger log = logger("DOOR");
    std::string target_board;
    const int FIREFLY_LOCK_SENSOR_GPIO = 42;
    const int FIREFLY_DOOR_SENSOR_GPIO = 39;
    const int FIREFLY_LOCK_TRIGGER_GPIO = 12;

    const int DEEPTHINK_LOCK_SENSOR_GPIO = 41;
    const int DEEPTHINK_DOOR_SENSOR_GPIO = 45; 
    const int DEEPTHINK_LOCK_TRIGGER_GPIO = 54;
    const int DEEPTHINK_POWER_GPIO = 132;

public:
    // deprecated initializer
    doorLock();
    // class initializer
    doorLock(std::string target_baord);
    // class destroyer
    ~doorLock();

    // reconf all pin mapping, deprecated
    void reset_pins();
    // refresh all pin states

    /**
     * @brief Get the states to status variable
     * 
     */
    void get_states();
    
    /**
     * @brief open the door
     * 
     * @return true - success
     * @return false - fail
     */
    bool door_open();
    
    /**
     * @brief close the door 
     * 
     * @return true - success
     * @return false - fail
     */
    bool door_close();
    
    /**
     * @brief wait until the door get open phsically  
     * 
     * @return true, if door is opend
     * @return false, if door is not opened
     */
    bool wait_open();
    
    /**
     * @brief wait until the door get close physically
     * 
     * @return true, if door is closed
     * @return false, if dooe is not closed
     */
    bool wait_close();
    // check doorLock state in WAIT(LOCK CLOSE)

    /**
     * @brief check door lock is ok to control
     * 
     * @return true, if door lock staus is LOCK_CLOSE
     * @return false, if door lock staus is not LOCK_CLOSE
     */
    bool is_ready();

    /**
     * @brief init deepthink board door lock gpio settings
     * 
     * @return true, success
     * @return false, fail
     */
    bool init_deepthink_doorlock_gpios();

    /**
     * @brief init firefly board door lock gpio settings
     * 
     * @return true 
     * @return false 
     */
    bool init_firefly_doorlock_gpios();
};
#endif //DAEMON_PROCESS_DOORLOCK_H
