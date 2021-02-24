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
#define DAEMON_PROCESS_DOORLOCK_H
/*
 * class doorLock manipulates door lock states and provide doorlock access APIs
 * enum lock_status
 * follows linux file auth system
 * lock|door|triger = 2^2|2^1|2^0 = 4|2|1
 */
enum lock_status{
    UNLOCK_CLOSE = 0,
    TRY_UNLOCK,
    UNLOCK_OPEN,
    TRIGGER_ERROR,
    TRY_LOCK,
    WAIT,
    TRY_LOCK_ERROR,
    LOCK_OPEN
};

class doorLock{
protected:
    // three integer values below indicates GPIO pin number
    int lock_num;
    int door_num;
    int trigger_num;

    // three integer values below stores GPIO current value
    int lock_value;
    int door_value;
    int trigger_value;

    // three integer values below stores actual pin fd
    std::fstream lock;
    std::fstream door;
    std::fstream trigger;
    lock_status status;

    logger log = logger("DOOR LOCK");

public:
    // deprecated initializer
    doorLock();
    // class initializer
    doorLock(int lock, int door, int trigger);
    // class destroyer
    ~doorLock();

    // reconf all pin mapping, deprecated
    void reset_pins();
    // refresh all pin states
    void get_states();
    // unlock
    bool door_open();
    // lock
    bool door_close();
    // wait until door open
    bool wait_open();
    // wait until door close
    bool wait_close();
    // check doorLock state in WAIT(LOCK CLOSE)
    bool is_ready();
};
#endif //DAEMON_PROCESS_DOORLOCK_H
