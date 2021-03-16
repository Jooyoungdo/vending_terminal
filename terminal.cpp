#include "terminal.h"

// terminal class initializer which initialize all inherited classes
terminal::terminal(std::string _SERVER_ADDRESS, int _QOS,
        std::string _user_id, std::string _topic, int lock, int door, int trigger):
        SERVER_ADDRESS(_SERVER_ADDRESS),
        cli(SERVER_ADDRESS, "DEVICE_"+_user_id),
        sub1(cli, _topic, _QOS),
        sub2(cli, MQTT_SERVER_TOPIC_DEVICE_PREFIX+_topic, _QOS),
        sub3(cli, MQTT_SERVER_TOPIC_UPDATER_PREFIX+_topic, _QOS),
        // pub1(cli, MQTT_TOPIC_DEVICE_OPERATION, _QOS),
        // pub2(cli, MQTT_TOPIC_DEVICE_UPDATE, _QOS),
        camera("auto_detect", "/dev/v4l/by-path/", "platform-fe3c0000.usb-usb-0:1.(\\d):1.0-video-index0"),
//        camera(camera_index, num),
        doorLock(lock, door, trigger)
{
    this->QOS = _QOS;
    this->user_id = _user_id;
    this->topic = _topic;
    cond = PTHREAD_COND_INITIALIZER;
    mutex = PTHREAD_MUTEX_INITIALIZER;
}

terminal::~terminal(){
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}
// send taken pictures to remote server using http protocol
size_t noop_cb(void *ptr, size_t size, size_t nmemb, void *data) {
    return size * nmemb;
}
bool terminal::post_image(std::string json) {
    rapidjson::Document d;
    d.Parse(json.c_str());

    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        if (std::string(d["type"].GetString()) == "grab_image"){
            log.print_log("http upload grab");
            curl_easy_setopt(curl, CURLOPT_URL, "http://ai0.beyless.com:8080/detection/grab/upload-images");
        } else if (std::string(d["type"].GetString()) == "open_door" || std::string(d["type"].GetString()) == "close_door"){
            log.print_log("http upload purchase");
            curl_easy_setopt(curl, CURLOPT_URL, "http://ai0.beyless.com:8080/detection/upload-images");
        } else {
            log.print_log(d["type"].GetString());
        }
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
//        curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: multipart/form-data");
        headers = curl_slist_append(headers, "Expect:");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, noop_cb);
        curl_mime *mime;
        curl_mimepart *part;
        mime = curl_mime_init(curl);
        for (int i = 0; i < get_image_count(); i++){
            part = curl_mime_addpart(mime);
            curl_mime_name(part, (std::to_string(i) + std::string(".jpg")).c_str());
            curl_mime_filedata(part,(std::string("/home/changseok/Desktop/image") + std::to_string(i) + std::string(".jpeg")).c_str());
        }

        if (d.HasMember("device_id")){
            part = curl_mime_addpart(mime);
            curl_mime_name(part, "device_id");
            curl_mime_data(part, std::to_string(d["device_id"].GetInt64()).c_str(), CURL_ZERO_TERMINATED);
        }
        if (d.HasMember("type")){
            part = curl_mime_addpart(mime);
            curl_mime_name(part, "stage");
            curl_mime_data(part, d["type"].GetString(), CURL_ZERO_TERMINATED);
        }
        if (d.HasMember("token")){
            part = curl_mime_addpart(mime);
            curl_mime_name(part, "token");
            curl_mime_data(part, d["token"].GetString(), CURL_ZERO_TERMINATED);
        }
        if (d.HasMember("operation_log_id")){
            part = curl_mime_addpart(mime);
            curl_mime_name(part, "operation_log_id");
            curl_mime_data(part, std::to_string(d["operation_log_id"].GetInt64()).c_str(), CURL_ZERO_TERMINATED);
        } else {
            part = curl_mime_addpart(mime);
            curl_mime_name(part, "operation_log_id");
            curl_mime_data(part, std::to_string(d["request_id"].GetInt64()).c_str(), CURL_ZERO_TERMINATED);
        }

        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        res = curl_easy_perform(curl);
        curl_mime_free(mime);
    }
    curl_easy_cleanup(curl);
    if (res == CURLE_OK){
        log.print_log("HTTP POST SUCCESS");
        return true;
    } else{
        log.print_log("HTTP POST FAIL");
        return false;
    }

}

// initialize MQTT client service, register call back function with lambda function (will be modified soon)
void terminal::initialize_mqtt_client() {
    connOpts.set_keep_alive_interval(1200);
//    connOpts.set_mqtt_version(MQTTVERSION_5);
    connOpts.set_clean_session(false);
//    connOpts.set_clean_start(true);
    connOpts.set_user_name("beyless");
    connOpts.set_password("ws-beyless");

    cli.set_connection_lost_handler([this](const std::string &) {
        log.print_log("connection lost");
        exit(2);
    });

    try {
        auto tok = cli.connect(connOpts);
        tok->wait();

        // auto subOpts = mqtt::subscribe_options(NO_LOCAL);
        // pub1.subscribe(subOpts)->wait();
        // log.print_log("publisher1 connected");
        // pub2.subscribe(subOpts)->wait();
        // log.print_log("publisher2 connected");

    } catch (const mqtt::exception &exc) {
        std::cerr << exc.what() << std::endl;
    }
    
    cli.set_message_callback([this](mqtt::const_message_ptr msg) {
            std::string json = std::string(msg->to_string());
            
            rapidjson::Document d;
            d.Parse(json.c_str());
            std::string type = d["type"].GetString();
            pthread_mutex_lock(&mutex);
            event = type;
            event_payload = json;
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&mutex);
    });

    try {
        auto subOpts = mqtt::subscribe_options(NO_LOCAL);
        sub1.subscribe(subOpts)->wait();
        log.print_log("subscriber1 connected");
        sub2.subscribe(subOpts)->wait();
        log.print_log("subscriber2 connected");
        sub3.subscribe(subOpts)->wait();
        log.print_log("subscriber3 connected");

    } catch (const mqtt::exception &exc) {
        std::cerr << exc.what() << std::endl;
    }
}

// initialize MYSQL database (require auto reconnect )
void terminal::initialize_MySQL_connector() {
    if( !(conn = mysql_init((MYSQL*)NULL))){
        log.print_log("init fail");
        exit(1);
    }
    log.print_log("mysql initialize success.");

    bool reconnect = 1;
    mysql_options(conn, MYSQL_OPT_RECONNECT, &reconnect);

    if(!mysql_real_connect(conn, server.c_str(), user.c_str(), password.c_str(), NULL, 3306, NULL, 0)){
        log.print_log("connect error.");
        exit(1);
    }

    log.print_log("mysql connect success");

    if(mysql_select_db(conn, database.c_str()) != 0){
        mysql_close(conn);
        log.print_log("select db fail");
        exit(1);
    }
    log.print_log("select db success");
}

// publish stirng type payload using pub1 object
void terminal::mqtt_publish(std::string payload, std::string topic)
{
    mqtt::message_ptr msg{mqtt::message::create(topic, payload)};
    cli.publish(msg);  
}

// create response json form
std::string terminal::create_response_form(std::string json, std::string type, std::string stage, std::string msg, bool result){
    rapidjson::Document return_form;
    return_form.SetObject();

    rapidjson::Document::AllocatorType& allocator = return_form.GetAllocator();
    size_t sz = allocator.Size();

    rapidjson::Document input_form;
    input_form.Parse(json.c_str());

    std::vector<std::string> json_members;

    // TODO: refactoring : InitJsonmember
    if (type.compare("image_upload") == 0){
        json_members.assign({"msg_id", "token", "operation_log_id", "type",
                             "stage", "msg", "ret_code",
                             "upload_duration", "timestamp"});
    }
    else if (type.compare("door_open_close") == 0){
        json_members.assign({"msg_id", "operation_log_id", "type",
                             "token", "msg", "ret_code", "timestamp"});
    }
    else if (type.compare("ack") == 0){
        json_members.assign({"msg", "ret_code", "env_id"});
    }
    else if (type.compare("camera_module_set_resp") == 0){
        json_members.assign({"device_id", "request_id", "type",
                             "msg", "ret_code"});
    }
    else if (type.compare("camera_module_get_resp") == 0){
        json_members.assign({"device_id", "request_id", "type",
                             "msg", "ret_code"});
    }
    else
        return NULL;

    // TODO: refactoring : SetJsonmember
    for (auto iter = json_members.begin(); iter!= json_members.end(); iter++){
        if (input_form.HasMember(iter->c_str())){
            rapidjson::Value name;
            name.SetString((iter->c_str()), iter->length());
            return_form.AddMember(name, input_form[iter->c_str()], allocator);
        } else {
            if (*iter == "upload_duration"){
                return_form.AddMember("upload_duration", -0, allocator);
            } else if (*iter == "stage") {
                rapidjson::Value str_value;
                str_value.SetString(stage.c_str(), stage.length());
                return_form.AddMember("stage", str_value, allocator);
            } else if (*iter == "msg") {
                rapidjson::Value str_value;
                str_value.SetString(msg.c_str(), msg.length());
                return_form.AddMember("msg", str_value, allocator);
            } else if (*iter == "ret_code") {
                    if (result)
                        return_form.AddMember("ret_code", "0000", allocator);
                    else
                        return_form.AddMember("ret_code", "0001", allocator);
            } else {
                log.print_log("can't find return type handler ... abort");
                log.print_log(*iter);
            }
        }
    }
    if(strstr(msg.c_str(),"open_door") != NULL){
        return_form["type"] = "open_door_resp";
    }else if(strstr(msg.c_str(),"close_door") != NULL){
        return_form["type"] = "close_door_resp";
    }else if(strstr(msg.c_str(),"camera_module_set") != NULL){
        return_form["type"] = "camera_module_set_resp";
    }else if(strstr(msg.c_str(),"camera_module_get") != NULL){
        return_form["type"] = "camera_module_get_resp";
    }
    
    // if (msg.find("open_door")>=0){
    //     return_form["type"] = "open_door_resp";
    // } else if (msg.find("close_door")>=0) {
    //     return_form["type"] = "close_door_resp";
    // }else if(msg.find("camera_module_set") >= 0){
    //     return_form["type"] = "camera_module_set_resp";
    // }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    return_form.Accept(writer);

    return buffer.GetString();
}

// upload taken pictures to MYSQL database
int64_t terminal::database_upload(cv::Mat iter, std::string env_id, std::string type){
    char* log_string = new char [1000];

    //jpeg compression parameters
    std::vector<uchar> buff;
    std::vector<int> param = std::vector<int>(2);
    param[0]=cv::IMWRITE_JPEG_QUALITY;
    param[1]=100;

    //jpeg compression
    imencode(".jpg",iter,buff,param);
    const char* data = reinterpret_cast<char*>(buff.data());
    int length = buff.size();
    sprintf(log_string, "JPEG encoding, size : %d", length);
    log.print_log(log_string);
    delete log_string;
    
    // -----------------------------------------------------------------------------------------------
    char *query,*end;
    
    query = new char [2*length + 1000];
    end = stpcpy(query,"INSERT INTO Image (env_id, img, type, check_num) VALUES('");
    end = stpcpy(end, env_id.c_str());
    end = stpcpy(end, "','");
    end += mysql_real_escape_string(conn,end,data,length);
    end = stpcpy(end,"','");
    end = stpcpy(end, type.c_str());
    end = stpcpy(end, "','");
    end = stpcpy(end, "1')");

    log_string = new char [1000];
    if (mysql_real_query(conn,query,(unsigned int) (end - query)))
    {
        std::string error = mysql_error(conn);
        sprintf(log_string, "Failed to insert row, Error: %s", error.c_str());
        log.print_log(log_string);
        delete log_string;

    } else {
        log.print_log("save image in mysql_server success");
    }

    delete query;
//    delete data;
    // -----------------------------------------------------------------------------------------------
    return mysql_insert_id(conn);
}

void terminal::start_daemon() {
    std::thread t0(&terminal::callback_rpc, this);
    t0.detach();
}

void terminal::callback_rpc() {
    std::string res_form;
    while(true)
    {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond,&mutex);
        log.print_log("received signal");
        if (event == MQTT_MESSAGE_TYPE_OPEN_DOOR) {
            operate_open_door(event_payload);
            clear_event_data();
        } else if (event == MQTT_MESSAGE_TYPE_COLLECT_DATASET) {
            operate_collect_data(event_payload);
            clear_event_data();
        } else if (event == MQTT_MESSAGE_TYPE_GRAB_IMAGE) {
            operate_grab_image(event_payload);
            clear_event_data();
        } else if(event == MQTT_MESSAGE_TYPE_CAMERA_MODULE_SET){
            operate_camera_module_set(event_payload);
            clear_event_data();
        }else if(event == MQTT_MESSAGE_TYPE_CAMERA_MODULE_GET){
            operate_camera_module_get(event_payload);
            clear_event_data();
        } else if (event == MQTT_MESSAGE_TYPE_TERMINATE) {
            pthread_mutex_unlock(&mutex);
            break;
        } else if (event == MQTT_MESSAGE_TYPE_REACT_HUMAN) {
            operate_play_sound(event_payload);
            clear_event_data();
        }else if(event != "NONE"){
            log.print_log("received unkonwn command");
            log.print_log(event);
            clear_event_data();
        }
        pthread_mutex_unlock(&mutex);
    }
    
}

bool terminal::operate_camera_module_set(std::string event_payload){
    std::string res_form;

    if(!set_module_profile(event_payload)){
        res_form = create_response_form(event_payload, "camera_module_set_resp", "camera_module_set", "set_module_profile fail", false);
        mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_UPDATE);
        return false;
    }
    if(!update_module_profile()){
        res_form = create_response_form(event_payload, "camera_module_set_resp", "camera_module_set", "update_module_profile fail", false);
        mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_UPDATE);
        return false;
    }

    res_form = create_response_form(event_payload, "camera_module_set_resp", "camera_module_set", "operate_camera_module_set success", true);
    mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_UPDATE);    

    return true;
}

bool terminal::operate_camera_module_get(std::string event_payload){
    // rapidjson::Document json_data;
    // json_data.Parse(event_payload.c_str());
    std::string res_form;
    // TODO: module 정보 가져가는 부분 구현 필요
    if(true){
        res_form = create_response_form(event_payload, "camera_module_get_resp", "camera_module_get", "camera_module_get success", true);
    }
    else{
        res_form = create_response_form(event_payload, "camera_module_get_resp", "camera_module_get", "camera_module_get fail", false);
    }
    mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_UPDATE);

    return true;
}

bool terminal::operate_play_sound(std::string event_payload){
    //TODO: sound play 기능 구현
    // sound play 동작은 response 보낼 필요 없음
    return true;
}

bool terminal::operate_grab_image(std::string event_payload){
     grab_frame();
     save_frame("/home/changseok/Desktop/");
     post_image(event_payload);
     return true;
}
bool terminal::operate_collect_data(std::string event_payload){
    rapidjson::Document d;
    std::string res_form;
    d.Parse(event_payload.c_str());
    int64_t image_id;
    if (is_ready())
    {
        door_open();
        wait_open();
        wait_close();
        door_close();
        grab_frame();
        std::vector<cv::Mat> images = get_frame();
        std::vector<cv::Mat>::iterator iter;

        for (iter = images.begin(); iter != images.end(); iter++)
        {
            image_id = database_upload(*iter, d["env_id"].GetString(), d["image_type"].GetString());
        }
        res_form = create_response_form(event_payload, "ack", "", std::to_string(image_id), true);
        mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_OPERATION);
    }
    else
    {
        res_form = create_response_form(event_payload, "ack", "", "", false);
        mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_OPERATION);
        return false;
    }
    return true;
}

bool terminal::operate_open_door(std::string event_payload){
    std::string res_form;
    if (is_ready()){
        grab_frame();
        save_frame("/home/changseok/Desktop/");
        //TODO: 사진 촬영 실패하더라도 문 여는 동작하도록 되어 있음, 이거 나중에 문제 될 가능성 있음
        // 실패 처리를 해야 될 듯
        if (post_image(event_payload))
            res_form = create_response_form(event_payload, "image_upload", "open_door", "image_upload", true);
        else
            res_form = create_response_form(event_payload, "image_upload", "open_door", "image_upload", false);
        mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_OPERATION);

        if (door_open())
            res_form = create_response_form(event_payload, "door_open_close", "open_door", "open_door", true);
        else
            res_form = create_response_form(event_payload, "door_open_close", "open_door", "open_door", false);
        mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_OPERATION);

        if (wait_open())
            wait_close();

        if (door_close())
            res_form = create_response_form(event_payload, "door_open_close", "close_door", "close_door", true);
        else
            res_form = create_response_form(event_payload, "door_open_close", "close_door", "close_door",false);
        mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_OPERATION);

        grab_frame();
        save_frame("/home/changseok/Desktop/");

        std::regex re("\"type\":\"open_door\"");
        event_payload = std::regex_replace(event_payload, re, "\"type\":\"close_door\"");
        if (post_image(event_payload))
            res_form = create_response_form(event_payload, "image_upload", "open_door", "image_upload", true);
        else
            res_form = create_response_form(event_payload, "image_upload", "open_door", "image_upload", false);
        mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_OPERATION);
    }else{
        return false;
    }

    return true;
}

void terminal::clear_event_data(){
    event = "NONE";
    event_payload = "NONE";
}