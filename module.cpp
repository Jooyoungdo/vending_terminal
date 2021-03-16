#include "module.h"

CameraModuleSetting* CameraModuleSetting::instance = nullptr;

CameraModuleSetting::CameraModuleSetting(){
    //moduleinfo_list.resize(MAX_CAMERA_COUNT);
    //TODO: change hardcoding path
    ReadJsonFile("/mnt/d/Beyless/0.project/2.firefly_rk3399/src/beyless_vending_terminal/camera_module_settings.json",&moduleinfo_list);
    InitDefaultProfile();
}

CameraModuleSetting::~CameraModuleSetting(){

}

std::vector<CameraModuleInfo> CameraModuleSetting::GetProfileList(){
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


//not used 
bool CameraModuleSetting::SetModuleinfo(std::string json){
    rapidjson::Document json_data;
    json_data.Parse(json.c_str());

    // port_num가 지정되어 있으면 특정 포트의 카메라만 설정한다
    if(!json_data["port_num"].IsNull()){
        int port_num = json_data["port_num"].GetInt();
        int find_port = -1;
        for (int i=0;i<moduleinfo_list.size();i++){
            if(port_num == moduleinfo_list[i].connected_info.port_num ){
                find_port =i;
                break;
            }
        }
        if(find_port > 0){
            moduleinfo_list[find_port].module_name = json_data["module_name"].GetString();
            moduleinfo_list[find_port].connected_info.interface_type = json_data["interface_type"].GetString();
            moduleinfo_list[find_port].connected_info.camera_location = json_data["camera_location"].GetString();
            moduleinfo_list[find_port].exposure_time = json_data["exposure_time"].GetInt();
            moduleinfo_list[find_port].aec = json_data["aec"].GetBool();
            moduleinfo_list[find_port].awb = json_data["awb"].GetBool();
            moduleinfo_list[find_port].color_temperature = json_data["color_temperature"].GetInt();
            moduleinfo_list[find_port].frame_width = json_data["frame_width"].GetInt();
            moduleinfo_list[find_port].frame_height = json_data["frame_height"].GetInt();
        }else{
            log.print_log(("can't find port_number(" + std::to_string(port_num)+")"));
            return false;
        }
    }else{
    // 특정 port_num가 지정되어 있지 않으면 하나의 설정으로 전부 셋팅 한다.    
         for (int i=0;i<moduleinfo_list.size();i++){
            if(!json_data["module_name"].IsNull()){
                moduleinfo_list[i].module_name = json_data["module_name"].GetString();
            }
            if(!json_data["interface_type"].IsNull()){
                moduleinfo_list[i].connected_info.interface_type = json_data["interface_type"].GetString();
            }
            if(!json_data["camera_location"].IsNull()){
                moduleinfo_list[i].connected_info.camera_location = json_data["camera_location"].GetString();
            }
            if(!json_data["exposure_time"].IsNull()){
                moduleinfo_list[i].exposure_time = json_data["exposure_time"].GetInt();
            }
            if(!json_data["aec"].IsNull()){
                moduleinfo_list[i].aec = json_data["aec"].GetBool();
            }
            if(!json_data["awb"].IsNull()){
                moduleinfo_list[i].awb = json_data["awb"].GetBool();
            }
            if(!json_data["color_temperature"].IsNull()){
                moduleinfo_list[i].color_temperature = json_data["color_temperature"].GetInt();
            }
            if(!json_data["frame_width"].IsNull()){
                moduleinfo_list[i].frame_width = json_data["frame_width"].GetInt();
            }
            if(!json_data["frame_height"].IsNull()){
                moduleinfo_list[i].frame_height = json_data["frame_height"].GetInt();
            }
         }
    }
    return true;
}

bool CameraModuleSetting::SaveProfileAsDefault(std::vector<CameraModuleInfo> moduleInfo){
    return true;
}

CameraModuleInfo CameraModuleSetting::GetProfile(ConnectedInfo connInfo){
    return moduleinfo_list[connInfo.camera_id];
}

bool CameraModuleSetting::UpdateProfile(std::vector <cv::VideoCapture> cameras){

    for(int i=0;i<cameras.size();i++){
        if(moduleinfo_list[i].frame_width > 0)
            cameras[i].set(cv::CAP_PROP_FRAME_WIDTH,moduleinfo_list[i].frame_width);
        if(moduleinfo_list[i].frame_height > 0)
            cameras[i].set(cv::CAP_PROP_FRAME_HEIGHT,moduleinfo_list[i].frame_height);
        else
            return false;
        cameras[i].set(cv::CAP_PROP_AUTO_EXPOSURE,moduleinfo_list[i].aec);
        cameras[i].set(cv::CAP_PROP_AUTO_WB,moduleinfo_list[i].awb);
        if(moduleinfo_list[i].exposure_time <=13 && moduleinfo_list[i].exposure_time >=-13)
            cameras[i].set(cv::CAP_PROP_EXPOSURE,moduleinfo_list[i].exposure_time);
        else
            return false;    
        if(moduleinfo_list[i].color_temperature <=10000 && moduleinfo_list[i].color_temperature >=0)
            cameras[i].set(cv::CAP_PROP_WB_TEMPERATURE,moduleinfo_list[i].color_temperature);
        else
            return false;
    }
    return true;
}
bool CameraModuleSetting::UpdateDefaultProfile(std::vector <cv::VideoCapture> cameras){

    for(int i=0;i<cameras.size();i++){
        if(default_moduleinfo_list[i].frame_width > 0)
            cameras[i].set(cv::CAP_PROP_FRAME_WIDTH,default_moduleinfo_list[i].frame_width);
        if(default_moduleinfo_list[i].frame_height > 0)
            cameras[i].set(cv::CAP_PROP_FRAME_HEIGHT,default_moduleinfo_list[i].frame_height);
        else
            return false;
        cameras[i].set(cv::CAP_PROP_AUTO_EXPOSURE,default_moduleinfo_list[i].aec);
        cameras[i].set(cv::CAP_PROP_AUTO_WB,default_moduleinfo_list[i].awb);
        if(default_moduleinfo_list[i].exposure_time <=13 && default_moduleinfo_list[i].exposure_time >=-13)
            cameras[i].set(cv::CAP_PROP_EXPOSURE,default_moduleinfo_list[i].exposure_time);
        else
            return false;    
        if(default_moduleinfo_list[i].color_temperature <=10000 && default_moduleinfo_list[i].color_temperature >=0)
            cameras[i].set(cv::CAP_PROP_WB_TEMPERATURE,default_moduleinfo_list[i].color_temperature);
        else
            return false;
    }
    return true;
}

CameraModuleInfo CameraModuleSetting::GetDefaultProfile(int port_num, std::string camera_type){
    
    //CameraModuleInfo defaultModuleSetting;
    int find_port = -1;
    for (int i=0;i<default_moduleinfo_list.size();i++){
        if(port_num == default_moduleinfo_list[i].connected_info.port_num 
        && camera_type.compare(default_moduleinfo_list[i].connected_info.interface_type)==0){
            find_port =i;
            break;
        }
    }
    if(find_port >= 0){
        return default_moduleinfo_list[find_port];
    }else{
        log.print_log(("can't find matched default profile, return default_moduleinfo_list[0]"));
        return default_moduleinfo_list[0];
    }

}

CameraModuleInfo CameraModuleSetting::GetDefaultProfile(int camera_id){
   return default_moduleinfo_list[camera_id];
}

bool CameraModuleSetting::SetDefaultProfile(int camera_id,CameraModuleInfo moduleInfo){
    default_moduleinfo_list[camera_id] = moduleInfo;
    return true;
}
bool CameraModuleSetting::InitDefaultProfile(){
    //TODO: read default profile from file
    default_moduleinfo_list.resize(MAX_CAMERA_COUNT);
    default_moduleinfo_list[0].connected_info.camera_id = 0;
    default_moduleinfo_list[0].connected_info.port_num  = 0;
    default_moduleinfo_list[0].connected_info.interface_type ="USB";
    default_moduleinfo_list[0].connected_info.camera_location ="TOP";
    default_moduleinfo_list[0].aec =true;
    default_moduleinfo_list[0].awb =true;
    default_moduleinfo_list[0].color_temperature =5000;
    default_moduleinfo_list[0].exposure_time = 0;
    default_moduleinfo_list[0].frame_width =1920;
    default_moduleinfo_list[0].frame_height =1080;
    default_moduleinfo_list[0].module_name = "DEEP_SNIK";
    
    default_moduleinfo_list[1].connected_info.camera_id = 1;
    default_moduleinfo_list[1].connected_info.port_num  = 1;
    default_moduleinfo_list[1].connected_info.interface_type ="USB";
    default_moduleinfo_list[1].connected_info.camera_location ="TOP";
    default_moduleinfo_list[1].aec =true;
    default_moduleinfo_list[1].awb =true;
    default_moduleinfo_list[1].color_temperature =5000;
    default_moduleinfo_list[1].exposure_time = 0;
    default_moduleinfo_list[1].frame_width =1920;
    default_moduleinfo_list[1].frame_height =1080;
    default_moduleinfo_list[1].module_name = "DEEP_SNIK";

    return true;
}

bool CameraModuleSetting::InsertModuleInfo(int index,CameraModuleInfo moduleInfo){
    if(index +1 > moduleinfo_list.size()){
        moduleinfo_list.resize(index +1);
    }
    moduleinfo_list[index] = moduleInfo;
    return true;
}

bool CameraModuleSetting::AddModuleInfo(CameraModuleInfo moduleInfo){
    moduleinfo_list.push_back(moduleInfo);
    return true;
}
CameraModuleSetting* CameraModuleSetting::GetInstance(){
    if (instance == nullptr){
        instance = new CameraModuleSetting();
    }
    return instance;
}

bool CameraModuleSetting::ReadJsonFile(std::string file_name,std::vector<CameraModuleInfo>* moduleinfo_list){

    std::ifstream input_file_stream(file_name);
    if(!input_file_stream){
        return false;
    }
    rapidjson::IStreamWrapper input_stream_wrapper(input_file_stream);
    
    rapidjson::Document json_doc;
    json_doc.ParseStream(input_stream_wrapper);
    try {
        *moduleinfo_list = ConvertJsonToModuleinfoList(&json_doc);
    } catch (int error) {
        log.print_log(("CATCH")); 
    }
    return true; 
}

bool CameraModuleSetting::WriteJsonFile(std::string file_name,std::vector<CameraModuleInfo> moduleinfo_list){
    rapidjson::Document json_doc;
    json_doc = ConvertModuleinfoListToJson(moduleinfo_list);
    std::ofstream output_stream(file_name);
    if(!output_stream){
        return false;
    }

    rapidjson::OStreamWrapper output_stream_wrapper(output_stream);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(output_stream_wrapper);
    json_doc.Accept(writer);
    return true;
}

std::vector<CameraModuleInfo> CameraModuleSetting::ConvertJsonToModuleinfoList(rapidjson::Document *json_doc){
    std::vector<CameraModuleInfo> module_infos;
    // TODO:여기까진 동작 함 , 확인은 해야 함, 방어 코드 추가 해야 함!!!!!!!
    for (auto& json_data : json_doc->GetArray()){
        CameraModuleInfo module_info;
        module_info.connected_info.camera_id = json_data["camera_id"].GetInt();
        module_info.connected_info.port_num = json_data["port_num"].GetInt();
        module_info.connected_info.camera_location = json_data["camera_location"].GetString();
        module_info.connected_info.interface_type = json_data["interface_type"].GetString();
        module_info.awb = json_data["awb"].GetBool();
        module_info.aec = json_data["aec"].GetBool();
        module_info.color_temperature=json_data["color_temperature"].GetInt();
        module_info.exposure_time=json_data["exposure_time"].GetInt();
        module_info.frame_height=json_data["frame_height"].GetInt();
        module_info.frame_width=json_data["frame_width"].GetInt();
        module_info.module_name=json_data["module_name"].GetString();
        module_infos.push_back(module_info);
    }
    return module_infos;
}
rapidjson::Document CameraModuleSetting::ConvertModuleinfoListToJson(std::vector<CameraModuleInfo> moduleInfo){
    rapidjson::Document json_doc;
    // rapidjson::AllocatorType& allocator = json_doc.GetAllocator();
    
    // for(int i=0;i<moduleInfo.size();i++){
    //     rapidjson::Value v1(moduleInfo[0]);
    //     json_doc.SetObject(CameraModuleInfo).PushBack(v1,allocator);
    //     //json_doc.SetObject(CameraModuleInfo).push_back(v1,allocator);
    // }
    
    // Value v2(v1, allocator);                      // make a copy
    // //TODO: 구현 

    return json_doc;
}
// bool CameraModuleSetting::IsSameModuleInfo(CameraModuleInfo read,CameraModuleInfo saved){
//     moduleInfo_read.connected_info.camera_id
//     moduleinfo_list.push_back(moduleInfo);
//     return true;
// }


// bool CameraModuleSetting::updateModuleInfo(std::string json){


