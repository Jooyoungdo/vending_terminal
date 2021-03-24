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

    
// mqtt broker address with port number
    const std::string SERVER_ADDRESS;
    // mqtt client optional parameter
    const bool NO_LOCAL = true;
    
    // this topic is defined at https://ai0.beyless.com/yona/jangwook.oh/BAIVE/post/20#
    const std::string MQTT_CLIENT_TOPIC_DEVICE_IMAGE_UPLOAD = "device_image_upload";
    const std::string MQTT_CLIENT_TOPIC_DEVICE_OPERATION = "device_operation";
    const std::string MQTT_CLIENT_TOPIC_DEVICE_UPDATE = "device_update";
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
    const std::string MQTT_MESSAGE_TYPE_TERMINATE = "terminate";
    const std::string MQTT_MESSAGE_TYPE_SET_SOUND = "set_sound";
    const std::string MQTT_MESSAGE_TYPE_DEVICE_FILE_DOWNLOAD = "file_download";

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
    std::string server = "192.168.10.144";
    std::string user = "root";
    std::string password = "return123";
    std::string database = "test";

    // std::string event = "NONE";
    // std::string event_payload = "NONE";

    pthread_cond_t cond;
    pthread_mutex_t mutex;
    bool terminate_program = false;

    std::queue<std::pair<std::string, std::string>> received_events;
public:
    // terminal class initiaizer, also initialize all inherited classes.
    terminal(std::string _SERVER_ADDRESS, int _QOS, std::string _user_id, std::string _topic,std::string target_board);
    ~terminal();
    
    void exit_program(bool exit);
    bool is_exit_program();
    const std::string TARGET_BOARD_FIREFLY = "FIREFLY";
    const std::string TARGET_BOARD_DEEPTHINK = "DEEPTHINK";

    // initialize MQTT client, all publisher & subscriber
    void initialize_mqtt_client();
    //initialize MYSQL database
    void initialize_MySQL_connector();

    // mqsql publish given string type payload (will be modified)
    void mqtt_publish(std::string payload,std::string topic);

    // doorLock_daemon, deprecated
    void doorLock_daemon();

    // post image using HTTP protocol
    bool post_image(std::string json);

    // create json type response form
    std::string create_response_form(std::string json, std::string type, std::string msg, bool result);

    // init json member depends on its type
    std::vector<std::string> init_json_member(std::string type);

    // upload image to database
    int64_t database_upload(cv::Mat iter, std::string env_id, std::string type);
    // operations of command from server
    bool operate_camera_module_set(std::string event_payload);
    bool operate_camera_module_get(std::string event_payload);
    bool operate_customer_attribute(std::string event_payload);
    bool operate_set_sound(std::string event_payload);
    bool operate_grab_image(std::string event_payload);
    bool operate_collect_data(std::string event_payload);
    bool operate_open_door(std::string event_payload);
    bool operate_device_file_download(std::string event_payload);
    bool open_close_door(std::string event_payload,bool do_resonpse);
    void start_daemon();
    void callback_rpc();
    bool donwload_file(std::string event_payload);
};

#endif //DAEMON_PROCESS_TERMINAL_H


