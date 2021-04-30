#ifndef CAMERA_MODULE_SETTING
#define CAMERA_MODULE_SETTING

#include <list>
#include <vector>
#include <iostream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include <fstream>
#include <opencv2/opencv.hpp>
#include "logger.h"

class ConnectedInfo{
private:    
public:    
    std::string interface_type; // USB / MIPI / ETC
    std::string camera_location; // FRONT / TOP / DOWN / SIDE / BACK
    int port_num; // physically connected information
    int camera_id;  // logically saved camera id info(moduleInfolist index)
};

class CameraModuleInfo
{
private:    
public:
    ConnectedInfo connected_info;
    std::string module_name;
    
    int exposure_time; // -13 ~ 13
    bool aec; // 0 : disable  1: enable
    bool awb; // 0 : disable  1: enable
    /*
        <color_temperature>
            D50 : 5000
            D65 : 6500
            TL84: 4100 
            CWF : 3900
            A   : 2800
    */
    int color_temperature; 
    int frame_width; // image width 
    int frame_height;// image height 
};

class CameraModuleSetting
{
private:
    CameraModuleSetting();
    //CameraModuleSetting(const CameraModuleSetting& other);
    ~CameraModuleSetting();
 
    static CameraModuleSetting* instance;
    std::vector<CameraModuleInfo> moduleinfo_list;
    CameraModuleInfo default_module_info_;
    logger log = logger("MODULE"); //logger object for print log
    const int MAX_CAMERA_COUNT = 10;
    //const std::string SETTING_FILE_PATH = "/mnt/d/Beyless/0.project/2.firefly_rk3399/src/beyless_vending_terminal/camera_module_settings.json";
    const std::string SETTING_FILE_PATH = "/home/firefly/beyless_vending_terminal/camera_module_settings.json";
    
public:
    static CameraModuleSetting* GetInstance();
    
    CameraModuleInfo ConvertJsonToModuleInfo(rapidjson::Document *json_doc);
    rapidjson::Document ConvertModuleInfoToJson(CameraModuleInfo module_info);
    void PrintModuleInfo(CameraModuleInfo moduleInfo);
    // read module setting from json
    // if can't read json file, set default module setting 
    bool ReadDefaultProfile();
    bool WriteDefaultProfile(CameraModuleInfo module_info);

    bool UpdateSettings(std::vector <cv::VideoCapture> cameras);
    bool UpdateSettings(cv::VideoCapture cameras);
    CameraModuleInfo GetDefaultProfile();
    void SetDefaultProfile(CameraModuleInfo module_info);
    bool CheckSettingValue();

    
};
//CameraModuleSetting* CameraModuleSetting::instance = nullptr;



#endif
