#include "module.h"

CameraModuleSetting* CameraModuleSetting::instance = nullptr;

CameraModuleSetting::CameraModuleSetting(){
    if(!ReadDefaultProfile()){
        log.print_log("ReadDefaultProfile fail");
    }
}

CameraModuleSetting::~CameraModuleSetting(){}

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

CameraModuleSetting* CameraModuleSetting::GetInstance(){
    if (instance == nullptr){
        instance = new CameraModuleSetting();
    }
    return instance;
}

CameraModuleInfo CameraModuleSetting::ConvertJsonToModuleInfo(rapidjson::Document *json_doc)
{
    CameraModuleInfo module_info;
    
    module_info.connected_info.camera_id = (*json_doc)["camera_id"].GetInt();
    module_info.connected_info.port_num = (*json_doc)["port_num"].GetInt();
    module_info.connected_info.camera_location = (*json_doc)["camera_location"].GetString();
    module_info.connected_info.interface_type = (*json_doc)["interface_type"].GetString();
    module_info.awb = (*json_doc)["awb"].GetBool();
    module_info.aec = (*json_doc)["aec"].GetBool();
    module_info.color_temperature = (*json_doc)["color_temperature"].GetInt();
    module_info.exposure_time = (*json_doc)["exposure_time"].GetInt();
    module_info.frame_height = (*json_doc)["frame_height"].GetInt();
    module_info.frame_width = (*json_doc)["frame_width"].GetInt();
    module_info.module_name = (*json_doc)["module_name"].GetString();
    return module_info;
}
rapidjson::Document CameraModuleSetting::ConvertModuleInfoToJson(CameraModuleInfo module_info){
    rapidjson::Document return_form;
    return_form.SetObject();
    rapidjson::Document::AllocatorType& allocator = return_form.GetAllocator();
    rapidjson::Value json_value;
    size_t sz = allocator.Size();

    json_value.SetString(module_info.module_name.c_str(), module_info.module_name.length());
    return_form.AddMember("module_name", json_value, allocator);
    json_value.SetString(module_info.connected_info.interface_type.c_str(), module_info.connected_info.interface_type.length());
    return_form.AddMember("interface_type", json_value, allocator);
    json_value.SetString(module_info.connected_info.camera_location.c_str(), module_info.connected_info.camera_location.length());
    return_form.AddMember("camera_location", json_value, allocator);
    json_value.SetInt(module_info.connected_info.port_num);
    return_form.AddMember("port_num", json_value, allocator);
    json_value.SetInt(module_info.connected_info.camera_id);
    return_form.AddMember("camera_id", json_value, allocator);
    json_value.SetBool(module_info.aec);
    return_form.AddMember("aec", json_value, allocator);
    json_value.SetBool(module_info.awb);
    return_form.AddMember("awb", json_value, allocator);
    json_value.SetInt(module_info.color_temperature);
    return_form.AddMember("color_temperature", json_value, allocator);
    json_value.SetInt(module_info.exposure_time);
    return_form.AddMember("exposure_time", json_value, allocator);
    json_value.SetInt(module_info.frame_width);
    return_form.AddMember("frame_width", json_value, allocator);
    json_value.SetInt(module_info.frame_height);
    return_form.AddMember("frame_height", json_value, allocator);

    return return_form;
}

bool CameraModuleSetting::ReadDefaultProfile(){
    std::ifstream input_file_stream(SETTING_FILE_PATH);
    if(!input_file_stream){
        log.print_log(("camera module default setting file is not exist(" + SETTING_FILE_PATH + ")")); 
        return false;
    }
    rapidjson::IStreamWrapper input_stream_wrapper(input_file_stream);
    
    try {
        rapidjson::Document json_doc;
        json_doc.ParseStream(input_stream_wrapper);
        default_module_info_ = ConvertJsonToModuleInfo(&json_doc);
    } catch (int e) {
        log.print_log("camera module default setting file is not exist"); 
        return false;
    }
   
    return true; 
}
bool CameraModuleSetting::WriteDefaultProfile(CameraModuleInfo module_info){
    std::ofstream output_file_stream;
    rapidjson::Document json_doc;

    output_file_stream.open(SETTING_FILE_PATH);
    if (!output_file_stream.is_open()){
        return false;
    }
    json_doc = ConvertModuleInfoToJson(module_info);
    rapidjson::OStreamWrapper osw(output_file_stream);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
    json_doc.Accept(writer);
    output_file_stream.close();
    return true;
}
bool CameraModuleSetting::UpdateSettings(std::vector <cv::VideoCapture> cameras){

    if(!CheckSettingValue()){
        return false;
    }
    for (int i = 0; i < cameras.size(); i++){
        cameras[i].set(cv::CAP_PROP_FRAME_WIDTH, default_module_info_.frame_width);
        cameras[i].set(cv::CAP_PROP_FRAME_HEIGHT, default_module_info_.frame_height);
        // to enable auto exposure control, set 3
        // to disable auto exposure control, set 1
        cameras[i].set(cv::CAP_PROP_AUTO_EXPOSURE, default_module_info_.aec? 3 :1);
        cameras[i].set(cv::CAP_PROP_AUTO_WB, default_module_info_.awb);
        cameras[i].set(cv::CAP_PROP_EXPOSURE, default_module_info_.exposure_time);
        cameras[i].set(cv::CAP_PROP_WB_TEMPERATURE, default_module_info_.color_temperature);
    }

    if(cameras.size() == 0 ){
        log.print_log("camera is not exist"); 
        return false;
    }
    return true;
}

bool CameraModuleSetting::PrintSettings(std::vector <cv::VideoCapture> cameras){

    
    for (int i = 0; i < cameras.size(); i++){
        CameraModuleInfo moduleInfo;
        moduleInfo.frame_width = cameras[i].get(cv::CAP_PROP_FRAME_WIDTH);
        moduleInfo.frame_height = cameras[i].get(cv::CAP_PROP_FRAME_HEIGHT);
        moduleInfo.aec = cameras[i].get(cv::CAP_PROP_AUTO_EXPOSURE);
        moduleInfo.awb = cameras[i].get(cv::CAP_PROP_AUTO_WB);
        moduleInfo.exposure_time = cameras[i].get(cv::CAP_PROP_EXPOSURE);
        moduleInfo.color_temperature =  cameras[i].get(cv::CAP_PROP_WB_TEMPERATURE);
        PrintModuleInfo(moduleInfo);
        
    }
    return true;
}

CameraModuleInfo CameraModuleSetting::GetDefaultProfile(){
    return default_module_info_;
}
void CameraModuleSetting::SetDefaultProfile(CameraModuleInfo module_info){
    default_module_info_ = module_info;
}

bool CameraModuleSetting::CheckSettingValue(){
    if(default_module_info_.frame_width < 0)
        return false;
    if (default_module_info_.frame_height < 0)
        return false;
    if(default_module_info_.color_temperature < 0 || default_module_info_.color_temperature > 10000)
        return false;
    if(default_module_info_.frame_width < 0)
        return false;
    return true;            
}
