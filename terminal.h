//
// Created by changseok on 20. 6. 15..
//
#ifndef DAEMON_PROCESS_TERMINAL_H

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <cstring>
#include <cctype>
#include <thread>
#include <chrono>
#include "mqtt/async_client.h"
#include "mqtt/topic.h"
#include <regex>

#include "camera.h"
#include "doorLock.h"
#include "logger.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include <curl/curl.h>
//#include </usr/include/mysql/mysql.h>
#include <mysql/mysql.h>
#include <pthread.h>
#include <iostream>
#include <queue>

#include "sound_manager.h"

#include <openssl/md5.h>
#include <unistd.h>

#define DAEMON_PROCESS_TERMINAL_H

class terminal : public camera, public doorLock
{
private:
    terminal(const terminal &terminal, 
    //mqtt::topic pub1,mqtt::topic pub2, 
    mqtt::topic sub2, mqtt::topic sub3, mqtt::topic sub1);

    std::string serial_number_;
    std::string project_ver_;
// mqtt broker address with port number
    const std::string SERVER_ADDRESS;
    // mqtt client optional parameter
    const bool NO_LOCAL = true;
    
    // this topic is defined at https://ai0.beyless.com/yona/jangwook.oh/BAIVE/post/20#
    const std::string MQTT_CLIENT_TOPIC_DEVICE_IMAGE_UPLOAD = "device_image_upload";
    const std::string MQTT_CLIENT_TOPIC_DEVICE_OPERATION = "device_operation";
    const std::string MQTT_CLIENT_TOPIC_DEVICE_UPDATE = "device_update";
    const std::string MQTT_CLIENT_TOPIC_DEVICE_INFO = "device_info";
    const std::string MQTT_CLIENT_TOPIC_DEVICE_REMOTE = "device_REMOTE";

    const std::string MQTT_SERVER_TOPIC_DEVICE_PREFIX = "DEVICE_";
    const std::string MQTT_SERVER_TOPIC_UPDATER_PREFIX = "UPDATER_";
    const std::string MQTT_SERVER_TOPIC_REMOTE_PREFIX = "REMOTE_";
    const std::string MQTT_SERVER_TOPIC_AI_PREFIX = "AI_";
    
    const std::string MQTT_MESSAGE_TYPE_OPEN_DOOR = "open_door";
    const std::string MQTT_MESSAGE_TYPE_COLLECT_DATASET = "collect_dataset";
    const std::string MQTT_MESSAGE_TYPE_GRAB_IMAGE = "grab_image";
    const std::string MQTT_MESSAGE_TYPE_CAMERA_MODULE_SET = "camera_module_set";
    const std::string MQTT_MESSAGE_TYPE_CAMERA_MODULE_GET = "camera_module_get";
    const std::string MQTT_MESSAGE_TYPE_CUSTOMER_ATTRIBUTE = "customer_attribute";
    const std::string MQTT_MESSAGE_TYPE_GOODBYE = "goodbye_data";
    const std::string MQTT_MESSAGE_TYPE_TERMINATE = "terminate";
    const std::string MQTT_MESSAGE_TYPE_SET_SOUND = "set_sound";
    const std::string MQTT_MESSAGE_TYPE_DEVICE_FILE_DOWNLOAD = "file_download";

    const int CONNECT_CHECK_TIME_SEC = 5 ; // connect check interval
    //std::string client_topics[]={};

    // MQTT client QOS (will be modified)
    int QOS;
    // MQTT client options...
    mqtt::connect_options connOpts;
    std::string user_id;
    std::string topic;
    
    // MQTT client object
    mqtt::async_client cli;
    // MQTT client publish, subscribe objects
    mqtt::topic sub1;
    mqtt::topic sub2;
    mqtt::topic sub3;
    mqtt::topic sub4;
    std::vector<mqtt::topic> subjects;
    // mqtt::topic pub1;
    // mqtt::topic pub2;

    // logger for SYSTEM terminal
    logger log = logger("SYS");

    // MYSQL connector object and return handler
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    // MYSQL options 

    // old mysql server
    //char *server = "192.168.10.69";
    // current mysql server
    const std::string MYSQL_SERVER_IP = "192.168.10.144";
    const std::string MYSQL_USER = "root";
    const std::string MYSQL_PASSWORD = "return123";
    const std::string MYSQL_DB = "test";

    // std::string event = "NONE";
    // std::string event_payload = "NONE";

    pthread_cond_t mqtt_connect_cond_;
    pthread_mutex_t mqtt_connect_mutex_;

    pthread_cond_t cond;
    pthread_mutex_t mutex;
    bool terminate_program = false;

    std::queue<std::pair<std::string, std::string>> received_events;
    std::string get_image_path();
public:
    // terminal class initiaizer, also initialize all inherited classes.
    terminal(std::string _SERVER_ADDRESS, int _QOS, std::string _user_id, std::string _topic,std::string target_board ,std::string serial_number, std::string project_ver);
    ~terminal();
    
    /**
     * @brief kill thread
     * 
     */
    void stop_thread();

    /**
     * @brief return flag of thread state
     * 
     * @return true : thread stopped
     * @return false : thread working
     */
    bool is_thread_stoped();

    /**
     * @brief initialize MYSQL database
     * 
     * this is just for internal test code
     * @return true : init success
     * @return false : init fail
     */
    bool initialize_MySQL_connector();

    // mqsql publish given string type payload (will be modified)
    /**
     * @brief publish mqtt message
     * 
     * @param payload : mqtt message
     * @param topic : subscribe toip
     */
    void mqtt_publish(std::string payload,std::string topic);

    /**
     * @brief upload image 
     * 
     * @param json : data
     * @return true : upload success
     * @return false : upload fail
     */
    bool post_image(std::string json);

    // create json type response form

    /**
     * @brief Create a response form object
     * 
     * @param json : json to response form
     * @param type : message type
     * @param msg : message
     * @param result :operate results
     * @return std::string : return response form
     */
    std::string create_response_form(std::string json, std::string type, std::string msg, bool result);

    /**
     * @brief init json member
     * 
     * @param type : mqtt message type
     * @return std::vector<std::string> : json data
     */
    std::vector<std::string> init_json_member(std::string type);

    /**
     * @brief upload data to internal database
     * 
     * @param iter 
     * @param env_id 
     * @param type : message type
     * @return int64_t 
     */
    int64_t database_upload(cv::Mat iter, std::string env_id, std::string type);

    /**
     * @brief : operation of setting camera module config
     * 
     * @param event_payload : command data from server
     * @return true : command success
     * @return false : command fail
     */
    bool operate_camera_module_set(std::string event_payload);
    
    /**
     * @brief : operation of getting camera module config
     * 
     * @param event_payload : command data from server
     * @return true : command success
     * @return false : command fail
     */
    bool operate_camera_module_get(std::string event_payload);
    
    /**
     * @brief : operation of saying hello to new customer
     * 
     * @param event_payload : command data from server
     * @return true : command success
     * @return false : command fail
     */
    bool operate_customer_attribute(std::string event_payload);
    
    /**
     * @brief : operation of saying goodbye to the customer
     * 
     * @param event_payload : command data from server
     * @return true : command success
     * @return false : command fail
     */
    bool operate_goodbye(std::string event_payload);
    
    /**
     * @brief : operation of setting sound volume
     * 
     * @param event_payload : command data from server
     * @return true : command success
     * @return false : command fail
     */
    bool operate_set_sound(std::string event_payload);
    
    /**
     * @brief : operation of grab image
     * 
     * @param event_payload : command data from server
     * @return true : command success
     * @return false : command fail
     */
    bool operate_grab_image(std::string event_payload);
    
    /**
     * @brief : operationg of collect data set
     * 
     * @param event_payload : command data from server
     * @return true : command success
     * @return false : command fail
     */
    bool operate_collect_data(std::string event_payload);
    
    /**
     * @brief : operation of open the door
     * 
     * @param event_payload : command data from server
     * @return true : command success
     * @return false : command fail
     */
    bool operate_open_door(std::string event_payload);
    
    /**
     * @brief : operation of file download(sound only now)
     * 
     * @param event_payload : command data from server
     * @return true : command success
     * @return false : command fail
     */
    bool operate_device_file_download(std::string event_payload);

    /**
     * @brief : open and close door 
     * open door -> wait open -> close door -> wait close 
     * 
     * @param event_payload : command data from server
     * @param do_resonpse : if true, response to server(mqtt message)
     * @return true 
     * @return false 
     */
    bool open_close_door(std::string event_payload,bool do_resonpse);

    /**
     * @brief : start daemon thread
     * 
     */
    void create_mqtt_message_thread();
    /**
     * @brief : thread of receive and send response 
     * receive mqtt message from server and response to server 
     */
    void mqtt_message_thread();
    
    /**
     * @brief not used yet, this function make server to know device info
     * 
     */
    void update_device_info();

    /**
     * @brief : download file from server 
     * 
     * @param event_payload : command data from server
     * @return true : download success
     * @return false : download fail
     */
    bool download_file(std::string event_payload);

    /**
     * @brief Create a mqtt connect thread object
     * 
     */
    void create_mqtt_connect_thread();
    /**
     * @brief mqtt connect thread 
     * 
     * check connection with server periodically
     * 
     */
    void mqtt_connect_thread();
};

#endif //DAEMON_PROCESS_TERMINAL_H


