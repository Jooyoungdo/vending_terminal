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

camera::camera(std::string mode, std::string prefix_path, std::string regex_grammer) {

    char* string_buffer;
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
        std::regex re (regex_grammer.c_str());
        std::smatch match;

        if(std::experimental::filesystem::is_directory(prefix_path.c_str())){
            for (auto & entry : std::experimental::filesystem::directory_iterator(prefix_path.c_str())){
                std::string str = entry.path().string();
                if (std::regex_search(str, match, re, std::regex_constants::match_default)){

                    // CameraModuleInfo module;
                    // module.connected_info.camera_id = camera_capture.size();
                    // module.connected_info.port_num = std::stoi(match[1].str());
                    // //FIXME: hardcoded code should be changed if mipi camera is used  
                    // module.connected_info.interface_type = "USB";
                    // module.aec=true;
                    // module.awb=true;
                    // module.color_temperature=5000;
                    // module.exposure_time = 0;
                    // module.frame_width =1920;
                    // module.frame_height =1080;
                    log.print_log( "camera device found on port number :" + match[1].str());
                    // if camera_capture size is 0, camera id is 0 
                    int port_num = std::stoi(match[1].str());
                    CameraModuleInfo module = cameraModuleSetting->GetDefaultProfile(port_num,"USB");
                    cameraModuleSetting->AddModuleInfo(module);
                    
                    //cv::VideoCapture _cap(prefix_path + match.str(), cv::CAP_V4L);
    //                _cap.open(());
#ifdef OLD_OPENCV
                    camera_capture.emplace_back(cv::VideoCapture(prefix_path.c_str() + match.str(), CV_CAP_V4L));
#else
                    camera_capture.emplace_back(cv::VideoCapture(prefix_path.c_str() + match.str(), cv::CAP_V4L));
#endif

                }
            }

            //TODO: 기본 카메라의 프로필의 연결 정보와 장착된 카메라의 연결 정보 비교
            //TODO: 동일 할 경우 기본 프로필의 카메라 설정 정보로 업데이트
            //TODO: 다를 경우 ?로그 찍고 업데이트 ㄴㄴ
        
        }

        else{
            log.print_log("can't find connected USB type camrea");
#ifdef DEBUG_BAIVE
            //Set Dummy Camrea Info
            log.print_log("set dummy camera info");
            CameraModuleInfo module;
            module.connected_info.port_num = 0;
            module.connected_info.camera_id = 0;
            module.exposure_time = 0;
            module.color_temperature = 5000;
            module.frame_width = 1920;
            module.frame_height = 1080;
            module.awb = true;
            module.aec = true;
            cameraModuleSetting->AddModuleInfo(module);
            //log.print_log("camera[0] info 1920 x 1080, port num 0, camera_id 0, exposure time 0,  ");
            camera_capture.emplace_back(cv::VideoCapture(0,cv::CAP_V4L));
            module.connected_info.port_num = 1;
            module.connected_info.camera_id = 1;
            module.exposure_time = 0;
            module.color_temperature = 5000;
            module.frame_width = 1920;
            module.frame_height = 1080;
            module.awb = true;
            module.aec = true;
            cameraModuleSetting->AddModuleInfo(module);
            camera_capture.emplace_back(cv::VideoCapture(1,cv::CAP_V4L));
#endif
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
    CameraModuleInfo module;
    int module_count = cameraModuleSetting->GetModuleinfoCount();
    if(!jsonData["camera_id"].IsNull() && jsonData["camera_id"].GetInt() < module_count){
        int camera_id = jsonData["camera_id"].GetInt();

        // module name / interface type / camera location / port number is not changed 
        module.module_name = cameraModuleSetting->GetProfileList()[camera_id].module_name;
        module.connected_info.interface_type = cameraModuleSetting->GetProfileList()[camera_id].connected_info.interface_type;
        module.connected_info.camera_location = cameraModuleSetting->GetProfileList()[camera_id].connected_info.camera_location;
        module.connected_info.port_num = cameraModuleSetting->GetProfileList()[camera_id].connected_info.port_num;
        
        module.connected_info.camera_id = jsonData["camera_id"].GetInt();
        module.exposure_time = jsonData["exposure_time"].GetInt();
        module.aec = jsonData["aec"].GetBool();
        module.awb = jsonData["awb"].GetBool();
        module.color_temperature = jsonData["color_temperature"].GetInt();
        module.frame_width = jsonData["frame_width"].GetInt();
        module.frame_height = jsonData["frame_height"].GetInt();

        if(camera_id >=0 && camera_id < module_count){
            //setting single camera profile
            cameraModuleSetting->InsertModuleInfo(camera_id,module);
        }else if(camera_id == -1){
            //setting all camera profile
            for(int i =0;i<module_count;i++){
                cameraModuleSetting->InsertModuleInfo(i,module);
            }
        }else{
            log.print_log(("can't find camera_id(" + std::to_string(camera_id)+")"));
            return false;
        }
    }else{
        std::string port_string = jsonData["camera_id"].IsNull()?"NULL": std::to_string( jsonData["camera_id"].GetInt());
        log.print_log(("can't find camera_id(" +port_string +")"));
        return false;
    }
    for(int i=0;i<module_count;i++){
        cameraModuleSetting->PrintModuleInfo(cameraModuleSetting->GetProfileList()[i]);
    }
    return true;
}

bool camera::update_module_profile(){
    cameraModuleSetting->UpdateProfile(camera_capture);
    return true;
}