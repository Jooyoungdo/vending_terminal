#include "module.h"

CameraModuleSetting* CameraModuleSetting::instance = nullptr;

std::vector<CameraModuleInfo> CameraModuleSetting::getModuleInfoList(){
    return moduleInfoList;
}

int CameraModuleSetting::getModuleInfoCount(){
    return moduleInfoList.size();
}

void CameraModuleSetting::printModuleInfo(CameraModuleInfo moduleInfo){
    log.print_log(("-- Connected Info -- "));
    log.print_log(("port_num : " + std::to_string(moduleInfo.connected_info.port_num)));
    log.print_log(("interface_type : " + std::string(moduleInfo.connected_info.interface_type)));
    log.print_log(("camera_location : " + std::string(moduleInfo.connected_info.camera_location)));

    log.print_log(("-- Camera Setting Info -- "));
    log.print_log(("module_name : " + std::string(moduleInfo.module_name)));
    log.print_log(("exposure_time : " + std::to_string(moduleInfo.exposure_time)));
    log.print_log(("aec : " + std::string(moduleInfo.aec?"true":"false")));
    log.print_log(("awb : " + std::string(moduleInfo.awb?"true":"false")));
    log.print_log(("color_temperature : " + std::to_string(moduleInfo.color_temperature)));
    log.print_log(("frame_width : " + std::to_string(moduleInfo.frame_width)));
    log.print_log(("frame_height : " + std::to_string(moduleInfo.frame_height)));
}



bool CameraModuleSetting::setModuleInfo(std::string json){
    rapidjson::Document jsonData;
    jsonData.Parse(json.c_str());

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
        if(find_port > 0){
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
}

bool CameraModuleSetting::saveProfile(CameraModuleInfo moduleInfo){
    return true;
}

CameraModuleInfo CameraModuleSetting::getProfile(ConnectedInfo connInfo){
    return moduleInfoList[connInfo.port_num];
}

bool CameraModuleSetting::updateProfile(std::vector <cv::VideoCapture> cameras){

    for(int i=0;i<cameras.size();i++){
        for(int j=0;j<moduleInfoList.size();j++){
            if (i == moduleInfoList[j].connected_info.camera_id){
                if(moduleInfoList[j].frame_width > 0)
                    cameras[i].set(cv::CAP_PROP_FRAME_WIDTH,moduleInfoList[j].frame_width);
                if(moduleInfoList[j].frame_height > 0)
                    cameras[i].set(cv::CAP_PROP_FRAME_HEIGHT,moduleInfoList[j].frame_height);
                else
                    return false;
                cameras[i].set(cv::CAP_PROP_AUTO_EXPOSURE,moduleInfoList[j].aec);
                cameras[i].set(cv::CAP_PROP_AUTO_WB,moduleInfoList[j].awb);
                if(moduleInfoList[j].exposure_time <=13 && moduleInfoList[j].exposure_time >=-13)
                    cameras[i].set(cv::CAP_PROP_EXPOSURE,moduleInfoList[j].exposure_time);
                else
                    return false;    
                if(moduleInfoList[j].color_temperature <=10000 && moduleInfoList[j].color_temperature >=0)
                    cameras[i].set(cv::CAP_PROP_WB_TEMPERATURE,moduleInfoList[j].color_temperature);
                else
                    return false;
            }
        }
        
    }
    return true;
}
bool CameraModuleSetting::updateDefaultProfile(std::vector <cv::VideoCapture> cameras){

    CameraModuleInfo defaultModuleInfo = getDefaultModuleInfo();
    for(int i=0;i<cameras.size();i++){
        cameras[i].set(cv::CAP_PROP_FRAME_WIDTH, defaultModuleInfo.frame_width);
        cameras[i].set(cv::CAP_PROP_FRAME_HEIGHT, defaultModuleInfo.frame_height);
        cameras[i].set(cv::CAP_PROP_AUTO_EXPOSURE, defaultModuleInfo.aec);
        cameras[i].set(cv::CAP_PROP_AUTO_WB, defaultModuleInfo.awb);
        cameras[i].set(cv::CAP_PROP_EXPOSURE, defaultModuleInfo.exposure_time);
        cameras[i].set(cv::CAP_PROP_WB_TEMPERATURE, defaultModuleInfo.color_temperature);
    }
    return true;
}

CameraModuleInfo CameraModuleSetting::getDefaultModuleInfo(){
    CameraModuleInfo defaultModuleSetting;
    // TODO: default setting value is hardcoded
    // TODO: this setting value should be from saved value
    defaultModuleSetting.frame_width=1920;
    defaultModuleSetting.frame_height=1080;
    
    defaultModuleSetting.aec=true;
    defaultModuleSetting.awb=true;
    defaultModuleSetting.exposure_time=0; // if aec is true, this value is meaningless
    defaultModuleSetting.color_temperature=0; // if awb is true, this value is meaningless

    return defaultModuleSetting;
}

bool CameraModuleSetting::addModuleInfo(CameraModuleInfo moduleInfo){
    moduleInfoList.push_back(moduleInfo);
    return true;
}
CameraModuleSetting* CameraModuleSetting::getInstance(){
    if (instance == nullptr){
        instance = new CameraModuleSetting();
    }
    return instance;
}
    

// TODO
// 1. save_profile()
// 2. update_profile()
// 

// bool CameraModuleSetting::updateModuleInfo(std::string json){


