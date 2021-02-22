//
// Created by changseok on 20. 6. 12..
//

#include "camera.h"
#define TEST_DEBUG
// default camera class initializer, only for debuggin, deprecated
camera::camera(){
    //default Video_device is 0
    video_device.push_back(0);

    cv::VideoCapture _cap;
#ifdef OLD_OPENCV
    _cap.open("/dev/video0", CV_CAP_V4L);
#else
    _cap.open("/dev/video0", cv::CAP_V4L);
#endif

    caps.push_back(_cap);
}

camera::camera(std::string mode, std::string prefix_path, std::string regex_grammer) {

    char* string_buffer;
    // if mode is not in auto detect, call default initializer
    std::cout << mode  << std::endl;
    cameraModuleSetting = CameraModuleSetting::getInstance();
    if (mode != "auto_detect"){
        //default Video_device is 0
        video_device.push_back(0);

        cv::VideoCapture _cap;
#ifdef OLD_OPENCV
        _cap.open("/dev/video0", CV_CAP_V4L);
#else
        _cap.open("/dev/video0", cv::CAP_V4L);
#endif
        caps.push_back(_cap);
    }
    else {
        std::regex re (regex_grammer.c_str());
        std::smatch match;

        // TODO: check directory is exist or not
        if(std::experimental::filesystem::is_directory(prefix_path.c_str())){
            for (auto & entry : std::experimental::filesystem::directory_iterator(prefix_path.c_str())){
                std::string str = entry.path().string();
                if (std::regex_search(str, match, re, std::regex_constants::match_default)){
                    CameraModuleInfo module;
                    string_buffer = new char[50];
                    sprintf(string_buffer, "camera device found on port number : %s", match[1].str().c_str());
                    log.print_log(string_buffer);
                    delete string_buffer;
                    module.connected_info.port_num = std::stoi(match[1].str());

                    // if video_device size is 0, camera id is 0 
                    module.connected_info.camera_id = video_device.size();

                    //FIXME: hardcoded code should be changed if mipi camera is used  
                    module.connected_info.interface_type = std::string("USB");
                    cameraModuleSetting->AddModuleInfo(module);
                    video_device.push_back(std::stoi(match[1].str()));
                    //cv::VideoCapture _cap(prefix_path + match.str(), cv::CAP_V4L);
    //                _cap.open(());
#ifdef OLD_OPENCV
                    caps.emplace_back(cv::VideoCapture(prefix_path.c_str() + match.str(), CV_CAP_V4L));
#else
                    caps.emplace_back(cv::VideoCapture(prefix_path.c_str() + match.str(), cv::CAP_V4L));
#endif

                }
            }
        
        }
#ifdef TEST_DEBUG        
        else{
            //Set Dummy Camrea Info
            CameraModuleInfo module;
            module.connected_info.port_num = 0;
            module.connected_info.camera_id = 0;
            module.exposure_time = 0;
            module.color_temperature = 5000;
            module.frame_width = 1920;
            module.frame_height = 1080;
            cameraModuleSetting->AddModuleInfo(module);
            caps.emplace_back(cv::VideoCapture(0,cv::CAP_V4L));
            module.connected_info.port_num = 1;
            module.connected_info.camera_id = 1;
            module.exposure_time = 0;
            module.color_temperature = 5000;
            module.frame_width = 1920;
            module.frame_height = 1080;
            cameraModuleSetting->AddModuleInfo(module);
            caps.emplace_back(cv::VideoCapture(1,cv::CAP_V4L));
        }
#endif        
    }
}

// camera class initializer, gets number of camera devices and list of video indexs
camera::camera(int* camera_index, int num){
    //hard copy
    for(int i = 0; i < num; i++){
        video_device.push_back(camera_index[i]);
        cv::VideoCapture _cap;
        _cap.open(camera_index[i]);
        caps.push_back(_cap);
    }
}

// grab frame from camera devices.
bool camera::grab_frame() {
    // flush images container
    images.clear();

    // grab images from predefined capture devices
    try{
        std::vector<cv::VideoCapture>::iterator iter;
        for(iter = caps.begin(); iter != caps.end(); iter++){
#ifdef OLD_OPEN_CV
            iter->set(CV_CAP_PROP_FRAME_WIDTH, 1920);
            iter->set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
#else
            iter->set(cv::CAP_PROP_FRAME_WIDTH, 1920);
            iter->set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
#endif
            // for stable output quality, get 5 frames and use last one
            cv::Mat img;
            for(int i = 0; i<5; i++)
                *iter >> img;
            cv::Rect bounds(0,0,img.cols,img.rows);
            cv::Rect r(330,0,1290,1080); // partly outside
            cv::Mat roi = img( r & bounds ); // cropped to fit image

            images.push_back(roi);
            roi.release();
        }
    } catch (cv::Exception& e) {
        log.print_log(("EXCEPTION CAUGHT : " + std::string(e.what())));
        return false;
    }

    log.print_log("GET FRAME");
    return true;
}

// return images
std::vector<cv::Mat> camera::get_frame(){
    return images;
}

// save images in _PATH/image#.jpeg
bool camera::save_frame(std::string _PATH){
    int image_counter = 0;
    std::vector<cv::Mat>::iterator iter;
    char PATH[100];
    for (iter = images.begin(); iter!= images.end(); iter++){
        sprintf(PATH, "%simage%d.jpeg", _PATH.c_str(), image_counter++);

        try{
            cv::imwrite(PATH, *iter);
        } catch (cv::Exception& e){
            log.print_log(("EXCEPTION CAUGHT : " + std::string(e.what())));
            return false;
        }
    }
    log.print_log("SAVE IMAGE TO " + _PATH);
    return true;
}

int camera::get_image_count() {

    return video_device.size();

}

bool camera::set_module_profile(std::string json){
    rapidjson::Document jsonData;
    jsonData.Parse(json.c_str());
    std::vector<CameraModuleInfo> moduleInfoList = cameraModuleSetting->getModuleInfoList();
    // port_num가 지정되어 있으면 특정 포트의 카메라만 설정한다
    if(!jsonData["port_num"].IsNull()){
        int port_num = jsonData["port_num"].GetInt();
        int find_port = -1;
        for (int i=0;i<moduleInfoList.size();i++){
            if(port_num == moduleInfoList[i].connected_info.port_num ){
                find_port =i;
                break;
            }
        }
        if(find_port >= 0 && find_port <moduleInfoList.size() ){
            moduleInfoList[find_port].module_name = jsonData["module_name"].GetString();
            moduleInfoList[find_port].connected_info.interface_type = jsonData["interface_type"].GetString();
            moduleInfoList[find_port].connected_info.camera_location = jsonData["camera_location"].GetString();
            moduleInfoList[find_port].exposure_time = jsonData["exposure_time"].GetInt();
            moduleInfoList[find_port].aec = jsonData["aec"].GetBool();
            moduleInfoList[find_port].awb = jsonData["awb"].GetBool();
            moduleInfoList[find_port].color_temperature = jsonData["color_temperature"].GetInt();
            moduleInfoList[find_port].frame_width = jsonData["frame_width"].GetInt();
            moduleInfoList[find_port].frame_height = jsonData["frame_height"].GetInt();
        }else{
            log.print_log(("can't find port_number(" + std::to_string(port_num)+")"));
            return false;
        }
    }else{
    // 특정 port_num가 지정되어 있지 않으면 하나의 설정으로 전부 셋팅 한다.    
         for (int i=0;i<moduleInfoList.size();i++){
            if(!jsonData["module_name"].IsNull()){
                moduleInfoList[i].module_name = jsonData["module_name"].GetString();
            }
            if(!jsonData["interface_type"].IsNull()){
                moduleInfoList[i].connected_info.interface_type = jsonData["interface_type"].GetString();
            }
            if(!jsonData["camera_location"].IsNull()){
                moduleInfoList[i].connected_info.camera_location = jsonData["camera_location"].GetString();
            }
            if(!jsonData["exposure_time"].IsNull()){
                moduleInfoList[i].exposure_time = jsonData["exposure_time"].GetInt();
            }
            if(!jsonData["aec"].IsNull()){
                moduleInfoList[i].aec = jsonData["aec"].GetBool();
            }
            if(!jsonData["awb"].IsNull()){
                moduleInfoList[i].awb = jsonData["awb"].GetBool();
            }
            if(!jsonData["color_temperature"].IsNull()){
                moduleInfoList[i].color_temperature = jsonData["color_temperature"].GetInt();
            }
            if(!jsonData["frame_width"].IsNull()){
                moduleInfoList[i].frame_width = jsonData["frame_width"].GetInt();
            }
            if(!jsonData["frame_height"].IsNull()){
                moduleInfoList[i].frame_height = jsonData["frame_height"].GetInt();
            }
         }
    }
    for(int i=0;i<moduleInfoList.size();i++){
        cameraModuleSetting->PrintModuleInfo(cameraModuleSetting->getModuleInfoList()[i]);
    }
    return true;
}

bool camera::update_module_profile(){
    cameraModuleSetting->updateProfile(caps);
    return true;
}