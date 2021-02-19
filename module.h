#ifndef CAMERA_MODULE_SETTING
#define CAMERA_MODULE_SETTING

#include <list>
#include <vector>
#include <iostream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
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
    CameraModuleSetting() {};
    //CameraModuleSetting(const CameraModuleSetting& other);
    ~CameraModuleSetting() {};
 
    static CameraModuleSetting* instance;
    std::vector<CameraModuleInfo> moduleInfoList;
    logger log = logger("CAM_MODULE"); //logger object for print log
public:
    static CameraModuleSetting* getInstance();
    std::vector<CameraModuleInfo> getModuleInfoList();
    int getModuleInfoCount();
    bool setModuleInfo(std::string json);
    void printModuleInfo(CameraModuleInfo moduleInfo);
    bool saveProfile(CameraModuleInfo moduleInfo);
    bool addModuleInfo(CameraModuleInfo moduleInfo);
    CameraModuleInfo getProfile(ConnectedInfo connInfo);
    bool updateProfile(std::vector <cv::VideoCapture> cameras);
    bool updateDefaultProfile(std::vector <cv::VideoCapture> cameras);
    CameraModuleInfo getDefaultModuleInfo();
    
};
//CameraModuleSetting* CameraModuleSetting::instance = nullptr;



#endif
