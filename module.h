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
    /**
     * @brief Get the Instance object
     * 
     * @return CameraModuleSetting* 
     */
    static CameraModuleSetting* GetInstance();
    
    /**
     * @brief convert json format to CameraModuleInfo
     * 
     * @param json_doc - containing camera module info with json format
     * @return CameraModuleInfo, containing camera module info with struct CameraModuleInfo
     */
    CameraModuleInfo ConvertJsonToModuleInfo(rapidjson::Document *json_doc);

    /**
     * @brief convert CameraModuleInfo to json format
     * 
     * @param module_info 
     * @return rapidjson::Document 
     */
    rapidjson::Document ConvertModuleInfoToJson(CameraModuleInfo module_info);

    /**
     * @brief print module information
     * 
     * @param moduleInfo 
     */
    void PrintModuleInfo(CameraModuleInfo moduleInfo);
    
    /**
     * @brief read module setting from json file(camera_module_settings.json)
     * if can't read json file, set default module setting   
     * 
     * @return if read default profile success, return true  
     */
    bool ReadDefaultProfile();

    /**
     * @brief write module setting as json file to save(camera_module_settings.json)
     * 
     * @param module_info 
     * @return if write success, return true
     */
    bool WriteDefaultProfile(CameraModuleInfo module_info);

    /**
     * @brief if received command for update new camera setting from server, update camera setting
     * 
     * @param cameras :  
     * @return if update success, return true 
     */
    bool UpdateSettings(std::vector <cv::VideoCapture> cameras);

    /**
     * @brief : Print Settings of USB Cameras
     * 
     * @param cameras : USB Camera Objects
     * @return true   : Print Success
     * @return false  : Print Fail
     */
    bool PrintSettings(std::vector <cv::VideoCapture> cameras);
    /**
     * @brief Get the Default Profile object
     * 
     * @return CameraModuleInfo 
     */
    CameraModuleInfo GetDefaultProfile();

    /**
     * @brief Set the Default Profile object
     * 
     * @param module_info 
     */
    void SetDefaultProfile(CameraModuleInfo module_info);

    /**
     * @brief Validate Camera Module Setting Values
     * 
     * @return if setting value is valid, return true  
     */
    bool CheckSettingValue();

    
};
//CameraModuleSetting* CameraModuleSetting::instance = nullptr;



#endif
