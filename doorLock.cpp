//
// Created by changseok on 20. 6. 12..
//

#include "doorLock.h"
#include "debug.h"
//default initializer (deprecated)
doorLock::doorLock(){
    log.print_log("default initiailizer called, this initializer used for debugging. please use another one!");

}

//door lock class initializer, gets GPIO pin number of lock, door sensors and door lock trigger
doorLock::doorLock(std::string target_board){
    log.print_log("door lock initialize ... ");
    this->target_board = target_board;
    
    if(target_board.compare("FIREFLY") == 0 ){

        init_firefly_doorlock_gpios();
    }else if(target_board.compare("DEEPTHINK") == 0 ){
        init_deepthink_doorlock_gpios();
    }
}

// doorLock class destroyer, release all pin FDs...
doorLock::~doorLock(){
    log.print_log("release resources ... ");
    this->door.close();
    this->lock.close();
    this->trigger.close();
    log.print_log("DONE");
}

// reconfigure all pin mappings, for debug only, deprecated
void doorLock::reset_pins(){
    try{
        this->door.close();
        this->lock.close();
        this->trigger.close();
    } catch(int err) {
        log.print_log("some pin's are already exist");
    };

    // create pin
    std::fstream pin_ctr;
    pin_ctr.open("/sys/class/gpio/export", std::ios::out);
    if (!pin_ctr.is_open()){
        log.print_log("\ncan not open file \"/sys/class/gpio/export\" please check again");
        std::exit(1);
    }

    pin_ctr << this->trigger_num;
    pin_ctr.seekg(0);
    pin_ctr << this->door_num;
    pin_ctr.seekg(0);
    pin_ctr << this->lock_num;
    pin_ctr.seekg(0);
    pin_ctr.close();

    // set direction
    char query[50];
    sprintf(query, "/sys/class/gpio/gpio%d/direction", this->trigger_num);
    pin_ctr.open(query);
    pin_ctr << "out";
    pin_ctr.close();

    query[0] = 0;
    sprintf(query, "/sys/class/gpio/gpio%d/direction", this->door_num);
    pin_ctr.open(query);
    pin_ctr << "in";
    pin_ctr.close();

    query[0] = 0;
    sprintf(query, "/sys/class/gpio/gpio%d/direction", this->lock_num);
    pin_ctr.open(query);
    pin_ctr << "in";
    pin_ctr.close();

    // assign pin
    query[0] = 0;
    sprintf(query, "/sys/class/gpio/gpio%d/value", this->trigger_num);
    this->trigger.open(query);
    query[0] = 0;
    sprintf(query, "/sys/class/gpio/gpio%d/value", this->door_num);
    this->door.open(query);
    query[0] = 0;
    sprintf(query, "/sys/class/gpio/gpio%d/value", this->lock_num);
    this->lock.open(query);

    // default state : lock
    this->trigger << "1";
    this->trigger.seekg(0);
}

// refresh all pin states and save it
void doorLock::get_states(){
    try{
        this->door >> this->door_value;
        this->lock >> this->lock_value;
        this->trigger >> this->trigger_value;

        this->door.seekg(0);
        this->lock.seekg(0);
        this->trigger.seekg(0);
        if(target_board.compare("FIREFLY") == 0){
            this->status = lock_status(4*this->lock_value + 2*this->door_value + this->trigger_value);
        }else if(target_board.compare("DEEPTHINK") == 0){
            this->status = lock_status(4*this->lock_value + 2*this->door_value + this->trigger_value+ 8);
        }
    } catch (int e) {
        log.print_log("DoorLock get state Error!");
    }

    return ;
}

// unlock doorLock trigger
bool doorLock::door_open(){
    try{
        if(target_board.compare("FIREFLY") == 0 ){
            log.print_log("UNLOCK");
            this->trigger << "0";
            this->trigger.seekg(0);    
        }else if(target_board.compare("DEEPTHINK") == 0 ){
            log.print_log("UNLOCK");
            this->trigger << "1";
            this->trigger.seekg(0);    
        }
    } catch (int e) {
        log.print_log("UNLOCK FAILURE");
        return false;
    }

    return true;
}

// lock doorLock trigger
bool doorLock::door_close(){
     try{
        if(target_board.compare("FIREFLY") == 0 ){
            log.print_log("LOCK");
            this->trigger << "1";
            this->trigger.seekg(0);    
        }else if(target_board.compare("DEEPTHINK") == 0 ){
            log.print_log("LOCK");
            this->trigger << "0";
            this->trigger.seekg(0);    
        }
    } catch (int e) {
        log.print_log("LOCK FAILURE");
        return false;
    }

    return true;
}

// wait until doorLock state is in UNLOCK and OPEN
bool doorLock::wait_open(){
    get_states();
    while (this->status != FIREFLY_UNLOCK_OPEN && this->status !=DEEPTHINK_UNLOCK_OPEN){
        get_states();
        usleep(1000);
        // log.print_log("wait_open");
        // log.print_log(std::to_string(this->status));
    }
    log.print_log("OPEN");
    return true;
}

// wait until doorLock state is in UNLOCK and CLOSE
bool doorLock::wait_close() {
    get_states();
    while (this->status != FIREFLY_UNLOCK_CLOSE && this->status !=DEEPTHINK_UNLOCK_CLOSE){
        get_states();
        usleep(1000);
        // log.print_log("wait_close");
        // log.print_log(std::to_string(this->status));
    }
    log.print_log("CLOSE");
    return true;
}

// wait until doorLock state is in READY STATE (LOCK CLOSE)
bool doorLock::is_ready() {
    get_states();
    if (this->status == FIREFLY_WAIT || this->status ==DEEPTHINK_WAIT)
        return true;
    else{
        log.print_log("door already open");
        log.print_log(std::to_string(this->status));
        return false;
    }

}

bool doorLock::init_deepthink_doorlock_gpios(){
    //set deepthink doorlock gpio numbers
    this->door_num = DEEPTHINK_DOOR_SENSOR_GPIO;
    this->lock_num = DEEPTHINK_LOCK_SENSOR_GPIO;
    this->trigger_num = DEEPTHINK_LOCK_TRIGGER_GPIO;
    this->door_power_value = DEEPTHINK_POWER_GPIO;

    // create pin
    std::fstream pin_ctr;
    pin_ctr.open("/sys/class/gpio/export", std::ios::out);
    if (!pin_ctr.is_open()){
        log.print_log("\ncan not open file \"/sys/class/gpio/export\" please check again");
#ifdef DEBUG_BAIVE
        log.print_log("DEBUG_BAIVE ");
        return true;        
#else
        std::exit(1);
#endif        
    }

    pin_ctr << this->trigger_num;
    pin_ctr.seekg(0);
    pin_ctr << this->door_num;
    pin_ctr.seekg(0);
    pin_ctr << this->lock_num;
    pin_ctr.seekg(0);
    pin_ctr << this->door_power_num;
    pin_ctr.seekg(0);
    pin_ctr.close();

    // set direction
    char query[50];
    sprintf(query, "/sys/class/gpio/gpio%d/direction", this->trigger_num);
    pin_ctr.open(query);
    pin_ctr << "out";
    pin_ctr.close();

    query[50];
    sprintf(query, "/sys/class/gpio/gpio%d/direction", this->door_power_num);
    pin_ctr.open(query);
    pin_ctr << "out";
    pin_ctr.close();

    query[0] = 0;
    sprintf(query, "/sys/class/gpio/gpio%d/direction", this->door_num);
    pin_ctr.open(query);
    pin_ctr << "in";
    pin_ctr.close();

    query[0] = 0;
    sprintf(query, "/sys/class/gpio/gpio%d/direction", this->lock_num);
    pin_ctr.open(query);
    pin_ctr << "in";
    pin_ctr.close();

    // assign pin
    query[0] = 0;
    sprintf(query, "/sys/class/gpio/gpio%d/value", this->trigger_num);
    this->trigger.open(query);
    query[0] = 0;
    sprintf(query, "/sys/class/gpio/gpio%d/value", this->door_power_num);
    this->door_power.open(query);
    query[0] = 0;
    sprintf(query, "/sys/class/gpio/gpio%d/value", this->door_num);
    this->door.open(query);
    query[0] = 0;
    sprintf(query, "/sys/class/gpio/gpio%d/value", this->lock_num);
    this->lock.open(query);

    log.print_log("DONE");

    // default state : lock
    this->trigger << "0";
    this->trigger.seekg(0);
    this->door_power << "1";
    this->door_power.seekg(0);

    return true;
}

bool doorLock::init_firefly_doorlock_gpios(){

    //set firefly doorlock gpio numbers
    this->door_num = FIREFLY_DOOR_SENSOR_GPIO;
    this->lock_num = FIREFLY_LOCK_SENSOR_GPIO;
    this->trigger_num = FIREFLY_LOCK_TRIGGER_GPIO;

    // create pin
    std::fstream pin_ctr;
    pin_ctr.open("/sys/class/gpio/export", std::ios::out);
    if (!pin_ctr.is_open()){
        log.print_log("\ncan not open file \"/sys/class/gpio/export\" please check again");
#ifdef DEBUG_BAIVE
        log.print_log("DEBUG_BAIVE ");
        return true;       
#else
        std::exit(1);
#endif        
    }

    pin_ctr << this->trigger_num;
    pin_ctr.seekg(0);
    pin_ctr << this->door_num;
    pin_ctr.seekg(0);
    pin_ctr << this->lock_num;
    pin_ctr.seekg(0);
    pin_ctr.close();

    // set direction
    char query[50];
    sprintf(query, "/sys/class/gpio/gpio%d/direction", this->trigger_num);
    pin_ctr.open(query);
    pin_ctr << "out";
    pin_ctr.close();

    query[0] = 0;
    sprintf(query, "/sys/class/gpio/gpio%d/direction", this->door_num);
    pin_ctr.open(query);
    pin_ctr << "in";
    pin_ctr.close();

    query[0] = 0;
    sprintf(query, "/sys/class/gpio/gpio%d/direction", this->lock_num);
    pin_ctr.open(query);
    pin_ctr << "in";
    pin_ctr.close();

    // assign pin
    query[0] = 0;
    sprintf(query, "/sys/class/gpio/gpio%d/value", this->trigger_num);
    this->trigger.open(query);
    query[0] = 0;
    sprintf(query, "/sys/class/gpio/gpio%d/value", this->door_num);
    this->door.open(query);
    query[0] = 0;
    sprintf(query, "/sys/class/gpio/gpio%d/value", this->lock_num);
    this->lock.open(query);

    log.print_log("DONE");

    // default state : lock
    this->trigger << "1";
    this->trigger.seekg(0);

    return true;
}