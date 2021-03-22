#include "terminal.h"
//#define TEST_DOWNLOAD
// terminal class initializer which initialize all inherited classes
terminal::terminal(std::string _SERVER_ADDRESS, int _QOS,
        std::string _user_id, std::string _topic,std::string target_board):
        SERVER_ADDRESS(_SERVER_ADDRESS),
        cli(SERVER_ADDRESS, "DEVICE_"+_user_id),
        sub1(cli, _topic, _QOS),
        sub2(cli, MQTT_SERVER_TOPIC_DEVICE_PREFIX+_topic, _QOS),
        sub3(cli, MQTT_SERVER_TOPIC_UPDATER_PREFIX+_topic, _QOS),
        sub4(cli, MQTT_SERVER_TOPIC_REMOTE_PREFIX+_topic, _QOS),
        // pub1(cli, MQTT_TOPIC_DEVICE_OPERATION, _QOS),
        // pub2(cli, MQTT_TOPIC_DEVICE_UPDATE, _QOS),
        camera("auto_detect"),
//        camera(camera_index, num),
        doorLock(target_board)
{
    this->QOS = _QOS;
    this->user_id = _user_id;
    this->topic = _topic;

    this->subjects.push_back(mqtt::topic(cli, _topic, _QOS));
    this->subjects.push_back(mqtt::topic(cli, MQTT_SERVER_TOPIC_DEVICE_PREFIX+_topic, _QOS));
    this->subjects.push_back(mqtt::topic(cli, MQTT_SERVER_TOPIC_UPDATER_PREFIX+_topic, _QOS));
    this->subjects.push_back(mqtt::topic(cli, MQTT_SERVER_TOPIC_REMOTE_PREFIX+_topic, _QOS));
    this->subjects.push_back(mqtt::topic(cli, MQTT_SERVER_TOPIC_AI_PREFIX+_topic, _QOS));
    

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
        std::vector<mqtt::topic>::iterator iter;
        for(iter = subjects.begin(); iter != subjects.end(); iter++){
            iter->subscribe(subOpts)->wait();
        }
        log.print_log("all subscriber connected");
        // sub1.subscribe(subOpts)->wait();
        // log.print_log("subscriber1 connected");
        // sub2.subscribe(subOpts)->wait();
        // log.print_log("subscriber2 connected");
        // sub3.subscribe(subOpts)->wait();
        // log.print_log("subscriber3 connected");

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
std::string terminal::create_response_form(std::string json, std::string type, std::string msg, bool result){
    rapidjson::Document return_form;
    return_form.SetObject();

    rapidjson::Document::AllocatorType& allocator = return_form.GetAllocator();
    size_t sz = allocator.Size();

    rapidjson::Document input_form;
    input_form.Parse(json.c_str());

    std::vector<std::string> json_members;

    json_members = init_json_member(type);

    // TODO: refactoring : SetJsonmember
    for (auto iter = json_members.begin(); iter!= json_members.end(); iter++){
        if (input_form.HasMember(iter->c_str())){
            rapidjson::Value name;
            name.SetString((iter->c_str()), iter->length());
            if (*iter == "type") {
                return_form["type"].SetString(type.data(),type.size(),allocator);
            }
            else{
                return_form.AddMember(name, input_form[iter->c_str()], allocator);
            }
        } else {
            if (*iter == "msg") {
                rapidjson::Value str_value;
                str_value.SetString(msg.c_str(), msg.length());
                return_form.AddMember("msg", str_value, allocator);
            } else if (*iter == "ret_code") {
                return_form.AddMember("ret_code", (result ? "0000" : "0001"), allocator);
            } else {
                log.print_log("can't find return type handler ... abort");
                log.print_log(*iter);
            }
        }
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    return_form.Accept(writer);

    return buffer.GetString();
}

std::vector<std::string> terminal::init_json_member(std::string type){
    std::vector<std::string> json_members;
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
    else if (type.compare("file_download_resp") == 0){
        json_members.assign({"device_id", "request_id", "type", "file_type",
                             "file_url", "file_md5", "msg", "ret_code"});
    }
    else{
        log.print_log("unknown type : "+ type);
        log.print_log("can't init json member, please contact develover(parand0320@beyless.com)");
    }
        
    return json_members;
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
    while(!is_exit_program())
    {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond,&mutex);
        log.print_log("received command");
        if (event == MQTT_MESSAGE_TYPE_OPEN_DOOR) {
            operate_open_door(event_payload);
        } else if (event == MQTT_MESSAGE_TYPE_COLLECT_DATASET) {
            operate_collect_data(event_payload);
        } else if (event == MQTT_MESSAGE_TYPE_GRAB_IMAGE) {
            operate_grab_image(event_payload);
        } else if(event == MQTT_MESSAGE_TYPE_CAMERA_MODULE_SET){
            operate_camera_module_set(event_payload);
        } else if(event == MQTT_MESSAGE_TYPE_SET_SOUND){
            operate_set_sound(event_payload);
        }else if(event == MQTT_MESSAGE_TYPE_CAMERA_MODULE_GET){
            operate_camera_module_get(event_payload);
        } else if (event == MQTT_MESSAGE_TYPE_CUSTOMER_ATTRIBUTE) {
            operate_customer_attribute(event_payload);
        } else if (event == MQTT_MESSAGE_TYPE_DEVICE_FILE_DOWNLOAD) {
            operate_device_file_download(event_payload);
        }else if (event == MQTT_MESSAGE_TYPE_TERMINATE) {
            pthread_mutex_unlock(&mutex);
            clear_event_data();
            break;
        } 
        else if(event != "NONE"){
            log.print_log("received unkonwn command");
            log.print_log(event);
        }

        
        clear_event_data();
        pthread_mutex_unlock(&mutex);
    }
    
}

bool terminal::operate_camera_module_set(std::string event_payload){
    std::string res_form;

    if(!set_module_profile(event_payload)){
        res_form = create_response_form(event_payload, "camera_module_set_resp", "set_module_profile fail", false);
        mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_UPDATE);
        return false;
    }
    if(!update_module_profile()){
        res_form = create_response_form(event_payload, "camera_module_set_resp", "update_module_profile fail", false);
        mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_UPDATE);
        return false;
    }

    res_form = create_response_form(event_payload, "camera_module_set_resp", "operate_camera_module_set success", true);
    mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_UPDATE);    

    return true;
}

bool terminal::operate_camera_module_get(std::string event_payload){
    // rapidjson::Document json_data;
    // json_data.Parse(event_payload.c_str());
    std::string res_form;
    // TODO: module 정보 가져가는 부분 구현 필요
    if(true){
        res_form = create_response_form(event_payload, "camera_module_get_resp", "camera_module_get success", true);
    }
    else{
        res_form = create_response_form(event_payload, "camera_module_get_resp", "camera_module_get fail", false);
    }
    mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_UPDATE);

    return true;
}

bool terminal::operate_customer_attribute(std::string event_payload){
    //TODO: sound play 기능 구현
    // sound play 동작은 response 보낼 필요 없음
    rapidjson::Document json_doc;
    json_doc.Parse(event_payload.c_str());
    AudioManager *audio = AudioManager::GetInstance();
    audio->PlaySound(SOUND_TYPE_GREETING);

    int new_customer = json_doc["new_customer"].GetInt64();
    int age = json_doc["age"].GetInt64();
    int gender = json_doc["gender"].GetInt64();
    int attractive = json_doc["attractive"].GetInt64();
    int glass = json_doc["glass"].GetInt64();
    int emotion = json_doc["emotion"].GetInt64();
    int mask = json_doc["mask"].GetInt64();
    int smileScore = json_doc["smileScore"].GetInt64();

    std::string is_new_customer = (new_customer == 2 ? "false" : "true");
    log.print_log("new_customer : " + is_new_customer);
    log.print_log("age : " + std::to_string(age));
    log.print_log("gender : " + std::to_string(gender));
    log.print_log("attractive score : " + std::to_string(attractive));
    log.print_log("emotion : " + std::to_string(emotion));
    switch (glass){
        case 0:
            log.print_log("glass : no");
            break;
        case 1:
            log.print_log("glass : ordinary glass");
            break;
        case 2:
            log.print_log("glass : sunglass");
            break;
    }
    
    std::string is_wear_customer = (mask == 1 ? "wearing mask": "not wearing mask");
    log.print_log("mask : " + is_wear_customer);
    log.print_log("smile score : " + std::to_string(smileScore));

    /*
        "new_customer": 1, // 1:New ,2:Old
        "age": 27,
        "gender": 1, // 0:Female, 1:Male
        "attractive": 0, // 0 ~ 100
        "emotion": -2, // 1:Calm, 3:Happy, -2:Other, -1:Unknown
        "glass": 0, // 0:No, 1:Ordinary glasses, 2:Sunglasses
        "mask": 1, // 0:Not wearing a mask, 1:Wearing a Mask
        "smileScore": 0, // 0 ~ 100
    */
    return true;
}

bool terminal::operate_set_sound(std::string event_payload){
    // sound play 동작은 response 보낼 필요 없음
    rapidjson::Document json_doc;
    json_doc.Parse(event_payload.c_str());
    long volume_percent =(long) json_doc["sound"].GetInt64();
    if(volume_percent >=0 && volume_percent <=100){
        AudioManager *audio = AudioManager::GetInstance();
        audio->SetSpeakerVolume(volume_percent);
    }else{
        log.print_log("operate_set_sound fail(sound is not valid)");
        return false;
    }
    
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
        open_close_door(event_payload,false);
        grab_frame();
        std::vector<cv::Mat> images = get_frame();
        std::vector<cv::Mat>::iterator iter;

        for (iter = images.begin(); iter != images.end(); iter++)
        {
            image_id = database_upload(*iter, d["env_id"].GetString(), d["image_type"].GetString());
        }
        res_form = create_response_form(event_payload, "ack", std::to_string(image_id), true);
        mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_OPERATION);
    }
    else
    {
        res_form = create_response_form(event_payload, "ack", "", false);
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
            res_form = create_response_form(event_payload, "image_upload", "image_upload", true);
        else
            res_form = create_response_form(event_payload, "image_upload", "image_upload", false);
        mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_OPERATION);

        open_close_door(event_payload,true);    
        grab_frame();
        save_frame("/home/changseok/Desktop/");

        std::regex re("\"type\":\"open_door\"");
        event_payload = std::regex_replace(event_payload, re, "\"type\":\"close_door\"");
        if (post_image(event_payload))
            res_form = create_response_form(event_payload, "image_upload", "image_upload", true);
        else
            res_form = create_response_form(event_payload, "image_upload", "image_upload", false);
        mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_OPERATION);
    }else{
        return false;
    }

    return true;
}

bool terminal::operate_device_file_download(std::string event_payload){
 
    std::string res_form;

    if (donwload_file(event_payload))
        res_form = create_response_form(event_payload, "file_download_resp", "download success", true);
    else
        res_form = create_response_form(event_payload, "file_download_resp", "download fail", false);
    mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_REMOTE);
    
    return true;
}

bool terminal::open_close_door(std::string event_payload,bool do_response){
    int result = false;
    std::string res_form;
    AudioManager* audio = AudioManager::GetInstance();
    result = door_open();
    if(result){
        audio->PlaySound(SOUND_TYPE_OPEN);
    }
    if (do_response){
        res_form = create_response_form(event_payload, "open_door_resp", (result ? "open door success" : "open door fail"), result);
        mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_OPERATION);
    }
    if (wait_open())
        wait_close();
    result = door_close();
    if (result){
        audio->PlaySound(SOUND_TYPE_CLOSE);
    }
    if (do_response){
        res_form = create_response_form(event_payload, "close_door_resp", (result ? "close door success" : "close door fail"), result);
        mqtt_publish(res_form, MQTT_CLIENT_TOPIC_DEVICE_OPERATION);
    }

    return true;
}

bool terminal::donwload_file(std::string event_payload){
    rapidjson::Document json_doc;
    CURL *curl;
    CURLcode res;
    FILE *fp = NULL;
    bool result = true;

#ifdef TEST_DOWNLOAD
    std::string file_url = "https://ai0.beyless.com/object-storage/vending/file/close_voice.mp3";
    std::string file_type = "close_door_sound";
    std::string file_md5 = "";
    AudioManager* audio = AudioManager::GetInstance();
    std::string file_name = "/mnt/d/Beyless/0.project/2.firefly_rk3399/src/beyless_vending_terminal/build/";
#else
    
    std::string file_url = json_doc["file_url"].GetString();
    std::string file_type = json_doc["file_type"].GetString();
    std::string file_md5 = json_doc["file_md5"].GetString();
    AudioManager* audio = AudioManager::GetInstance();
    std::string file_name = audio->GetSoundFileRoot();
#endif    
    

    if(file_type.compare("open_door_sound")==0){
        file_name += "open_voice.wav";
    }else if(file_type.compare("close_door_sound")==0){
        file_name += "close_voice.wav";
    }else if(file_type.compare("greeting_sound")==0){
        file_name += "greeting_voice.wav";
    }else{
        log.print_log("unknown file type, file is not updated");
        result = false;
        goto FUNC_END;
    }
    json_doc.Parse(event_payload.c_str());

    curl = curl_easy_init();                                                                                                                                                                                                                                                           
    fp = fopen(file_name.c_str(),"wb");
    if (fp != NULL && curl){   
        curl_easy_setopt(curl, CURLOPT_URL, file_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
        fp = NULL;
    }else{
        result = false;
        goto FUNC_END;
    }   
    //TODO: MD5 check function add 
    //TODO: if MD5 check got fail, how should I do?restore sound file?
FUNC_END:
    if(fp !=NULL){
        fclose(fp);
        fp = NULL;
    }
    return result;
}

// bool terminal::check_md5(){
//     system("");
// }

void terminal::exit_program(bool exit){
    terminate_program = exit;
    if(exit){
        pthread_cond_signal(&cond);
    }
}

bool terminal::is_exit_program(){
    return terminate_program;
}

void terminal::clear_event_data(){
    event = "NONE";
    event_payload = "NONE";
}