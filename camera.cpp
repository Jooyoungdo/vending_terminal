//
// Created by changseok on 20. 6. 12..
//

#include "camera.h"

// default camera class initializer, only for debuggin, deprecated
camera::camera(){
    cv::VideoCapture _cap;
#ifdef OLD_OPENCV
    _cap.open("/dev/video0", CV_CAP_V4L);
#else
    _cap.open("/dev/video0", cv::CAP_V4L);
#endif

    camera_capture.push_back(_cap);
}

camera::camera(std::string mode) {

    char* string_buffer;
    std::string regex_grammer[] ={"platform-fe3c0000.usb-usb-0:1.(\\d):1.0-video-index0",
                                    "platform-fe380000.usb-usb-0:1.(\\d):1.0-video-index0"};
    std::string prefix_path = "/dev/v4l/by-path/" ;
    
    // if mode is not in auto detect, call default initializer
    std::cout << mode  << std::endl;
    cameraModuleSetting = CameraModuleSetting::GetInstance();
    if (mode != "auto_detect"){
        cv::VideoCapture _cap;
#ifdef OLD_OPENCV
        _cap.open("/dev/video0", CV_CAP_V4L);
#else
        _cap.open("/dev/video0", cv::CAP_V4L);
#endif
        camera_capture.push_back(_cap);
    }
    else {


        if(std::experimental::filesystem::is_directory(prefix_path.c_str())){
            for (auto & entry : std::experimental::filesystem::directory_iterator(prefix_path.c_str())){
                std::string str = entry.path().string();
                std::regex re (regex_grammer[0].c_str());
                std::smatch match;
                if (std::regex_search(str, match, re, std::regex_constants::match_default)){
                    //FIXME: hardcoded code should be changed if mipi camera is used  
                    log.print_log( "camera device found on port number :" + match[1].str());

                    int port_num = std::stoi(match[1].str());
#ifdef OLD_OPENCV
                    camera_capture.emplace_back(cv::VideoCapture(prefix_path.c_str() + match.str(), CV_CAP_V4L));
#else
                    camera_capture.emplace_back(cv::VideoCapture(prefix_path.c_str() + match.str(), cv::CAP_V4L));
#endif
                }
            }
            // this is only for deepthink board, it has 10 usb port
            for (auto & entry : std::experimental::filesystem::directory_iterator(prefix_path.c_str())){
                std::string str = entry.path().string();
                std::regex re (regex_grammer[1].c_str());
                std::smatch match;
                if (std::regex_search(str, match, re, std::regex_constants::match_default)){
                    //FIXME: hardcoded code should be changed if mipi camera is used  
                    int port_num = std::stoi(match[1].str()) + 5 ;
                    log.print_log( "camera device found on port number :" + std::to_string(port_num));
#ifdef OLD_OPENCV
                    camera_capture.emplace_back(cv::VideoCapture(prefix_path.c_str() + match.str(), CV_CAP_V4L));
#else
                    camera_capture.emplace_back(cv::VideoCapture(prefix_path.c_str() + match.str(), cv::CAP_V4L));
#endif

                }
            }
            cameraModuleSetting->UpdateSettings(camera_capture);
            //TODO: 기본 카메라의 프로필의 연결 정보와 장착된 카메라의 연결 정보 비교
            //TODO: 동일 할 경우 기본 프로필의 카메라 설정 정보로 업데이트
            //TODO: 다를 경우 ?로그 찍고 업데이트 ㄴㄴ
        
        }

        else{
            log.print_log("can't find connected USB type camrea");
        }
    }
}

// camera class initializer, gets number of camera devices and list of video indexs
camera::camera(int* camera_index, int num){
    //hard copy
    for(int i = 0; i < num; i++){
        cv::VideoCapture _cap;
        _cap.open(camera_index[i]);
        camera_capture.push_back(_cap);
    }
}

// grab frame from camera devices.
bool camera::grab_frame() {
    // flush images container
    images.clear();

    // grab images from predefined capture devices
    try{
        std::vector<cv::VideoCapture>::iterator iter;
        for(iter = camera_capture.begin(); iter != camera_capture.end(); iter++){
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
    return camera_capture.size();
}

bool camera::set_module_profile(std::string json){
    rapidjson::Document jsonData;
    jsonData.Parse(json.c_str());
    CameraModuleInfo module_info;
    CameraModuleInfo default_module_info =cameraModuleSetting->GetDefaultProfile();

    module_info.connected_info.camera_id = default_module_info.connected_info.camera_id;
    module_info.connected_info.interface_type = default_module_info.connected_info.interface_type;
    module_info.connected_info.camera_location = default_module_info.connected_info.camera_location;
    module_info.connected_info.port_num = default_module_info.connected_info.port_num;
    module_info.module_name = default_module_info.module_name;

    module_info.exposure_time = jsonData["exposure_time"].GetInt();
    module_info.aec = jsonData["aec"].GetBool();
    module_info.awb = jsonData["awb"].GetBool();
    module_info.color_temperature = jsonData["color_temperature"].GetInt();
    module_info.frame_width = jsonData["frame_width"].GetInt();
    module_info.frame_height = jsonData["frame_height"].GetInt();
    cameraModuleSetting->SetDefaultProfile(module_info);
    cameraModuleSetting->WriteDefaultProfile(module_info);
    cameraModuleSetting->PrintModuleInfo(module_info);
    return true;
}

bool camera::update_module_profile(){
    cameraModuleSetting->UpdateSettings(camera_capture);
    return true;
}