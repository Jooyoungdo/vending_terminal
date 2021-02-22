#include "module.h"

CameraModuleSetting* CameraModuleSetting::instance = nullptr;

std::vector<CameraModuleInfo> CameraModuleSetting::getModuleInfoList(){
    return moduleinfo_list;
}

int CameraModuleSetting::GetModuleinfoCount(){
    return moduleinfo_list.size();
}

void CameraModuleSetting::PrintModuleInfo(CameraModuleInfo moduleInfo){
    log.print_log(("-- Connected Info -- "));
    log.print_log(("port_num : " + std::to_string(moduleInfo.connected_info.port_num)));
    log.print_log(("interface_type : " + std::string(moduleInfo.connected_info.interface_type)));
    log.print_log(("camera_location : " + std::string(moduleInfo.connected_info.camera_location)));

    log.print_log(("-- Setting Info -- "));
    log.print_log(("module_name : " + std::string(moduleInfo.module_name)));
    log.print_log(("exposure_time : " + std::to_string(moduleInfo.exposure_time)));
    log.print_log(("aec : " + std::string(moduleInfo.aec?"true":"false")));
    log.print_log(("awb : " + std::string(moduleInfo.awb?"true":"false")));
    log.print_log(("color_temperature : " + std::to_string(moduleInfo.color_temperature)));
    log.print_log(("frame_width : " + std::to_string(moduleInfo.frame_width)));
    log.print_log(("frame_height : " + std::to_string(moduleInfo.frame_height)));
}



bool CameraModuleSetting::SetModuleinfo(std::string json){
    rapidjson::Document jsonData;
    jsonData.Parse(json.c_str());

    // port_num가 지정되어 있으면 특정 포트의 카메라만 설정한다
    if(!jsonData["port_num"].IsNull()){
        int port_num = jsonData["port_num"].GetInt();
        int find_port = -1;
        for (int i=0;i<moduleinfo_list.size();i++){
            if(port_num == moduleinfo_list[i].connected_info.port_num ){
                find_port =i;
                break;
            }
        }
        if(find_port > 0){
            moduleinfo_list[find_port].module_name = jsonData["module_name"].GetString();
            moduleinfo_list[find_port].connected_info.interface_type = jsonData["interface_type"].GetString();
            moduleinfo_list[find_port].connected_info.camera_location = jsonData["camera_location"].GetString();
            moduleinfo_list[find_port].exposure_time = jsonData["exposure_time"].GetInt();
            moduleinfo_list[find_port].aec = jsonData["aec"].GetBool();
            moduleinfo_list[find_port].awb = jsonData["awb"].GetBool();
            moduleinfo_list[find_port].color_temperature = jsonData["color_temperature"].GetInt();
            moduleinfo_list[find_port].frame_width = jsonData["frame_width"].GetInt();
            moduleinfo_list[find_port].frame_height = jsonData["frame_height"].GetInt();
        }else{
            log.print_log(("can't find port_number(" + std::to_string(port_num)+")"));
            return false;
        }
    }else{
    // 특정 port_num가 지정되어 있지 않으면 하나의 설정으로 전부 셋팅 한다.    
         for (int i=0;i<moduleinfo_list.size();i++){
            if(!jsonData["module_name"].IsNull()){
                moduleinfo_list[i].module_name = jsonData["module_name"].GetString();
            }
            if(!jsonData["interface_type"].IsNull()){
                moduleinfo_list[i].connected_info.interface_type = jsonData["interface_type"].GetString();
            }
            if(!jsonData["camera_location"].IsNull()){
                moduleinfo_list[i].connected_info.camera_location = jsonData["camera_location"].GetString();
            }
            if(!jsonData["exposure_time"].IsNull()){
                moduleinfo_list[i].exposure_time = jsonData["exposure_time"].GetInt();
            }
            if(!jsonData["aec"].IsNull()){
                moduleinfo_list[i].aec = jsonData["aec"].GetBool();
            }
            if(!jsonData["awb"].IsNull()){
                moduleinfo_list[i].awb = jsonData["awb"].GetBool();
            }
            if(!jsonData["color_temperature"].IsNull()){
                moduleinfo_list[i].color_temperature = jsonData["color_temperature"].GetInt();
            }
            if(!jsonData["frame_width"].IsNull()){
                moduleinfo_list[i].frame_width = jsonData["frame_width"].GetInt();
            }
            if(!jsonData["frame_height"].IsNull()){
                moduleinfo_list[i].frame_height = jsonData["frame_height"].GetInt();
            }
         }
    }
    return true;
}

bool CameraModuleSetting::SaveProfile(CameraModuleInfo moduleInfo){
    return true;
}

CameraModuleInfo CameraModuleSetting::getProfile(ConnectedInfo connInfo){
    return moduleinfo_list[connInfo.port_num];
}

bool CameraModuleSetting::updateProfile(std::vector <cv::VideoCapture> cameras){

    for(int i=0;i<cameras.size();i++){
        for(int j=0;j<moduleinfo_list.size();j++){
            if (i == moduleinfo_list[j].connected_info.camera_id){
                if(moduleinfo_list[j].frame_width > 0)
                    cameras[i].set(cv::CAP_PROP_FRAME_WIDTH,moduleinfo_list[j].frame_width);
                if(moduleinfo_list[j].frame_height > 0)
                    cameras[i].set(cv::CAP_PROP_FRAME_HEIGHT,moduleinfo_list[j].frame_height);
                else
                    return false;
                cameras[i].set(cv::CAP_PROP_AUTO_EXPOSURE,moduleinfo_list[j].aec);
                cameras[i].set(cv::CAP_PROP_AUTO_WB,moduleinfo_list[j].awb);
                if(moduleinfo_list[j].exposure_time <=13 && moduleinfo_list[j].exposure_time >=-13)
                    cameras[i].set(cv::CAP_PROP_EXPOSURE,moduleinfo_list[j].exposure_time);
                else
                    return false;    
                if(moduleinfo_list[j].color_temperature <=10000 && moduleinfo_list[j].color_temperature >=0)
                    cameras[i].set(cv::CAP_PROP_WB_TEMPERATURE,moduleinfo_list[j].color_temperature);
                else
                    return false;
            }
        }
        
    }
    return true;
}
bool CameraModuleSetting::UpdateDefaultProfile(std::vector <cv::VideoCapture> cameras){

    CameraModuleInfo defaultModuleInfo = GetDefaultModuleInfo();
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

CameraModuleInfo CameraModuleSetting::GetDefaultModuleInfo(){
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

bool CameraModuleSetting::AddModuleInfo(CameraModuleInfo moduleInfo){
    moduleinfo_list.push_back(moduleInfo);
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


