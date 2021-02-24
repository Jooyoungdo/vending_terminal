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
    CameraModuleSetting();
    //CameraModuleSetting(const CameraModuleSetting& other);
    ~CameraModuleSetting();
 
    static CameraModuleSetting* instance;
    std::vector<CameraModuleInfo> moduleinfo_list;
    std::vector<CameraModuleInfo> default_moduleinfo_list;
    logger log = logger("CAM_MODULE"); //logger object for print log
    const int MAX_CAMERA_COUNT = 10;
    
public:
    static CameraModuleSetting* GetInstance();
    std::vector<CameraModuleInfo> GetProfileList();
    int GetModuleinfoCount();
    bool SetModuleinfo(std::string json);
    void PrintModuleInfo(CameraModuleInfo moduleInfo);
    
    bool InsertModuleInfo(int index,CameraModuleInfo moduleInfo);
    bool AddModuleInfo(CameraModuleInfo moduleInfo);
    bool UpdateProfile(std::vector <cv::VideoCapture> cameras);
    CameraModuleInfo GetProfile(ConnectedInfo connInfo);
    bool SetProfile(int camera_id,CameraModuleInfo moduleInfo);

    bool UpdateDefaultProfile(std::vector <cv::VideoCapture> cameras);
    CameraModuleInfo GetDefaultProfile(int port_num, std::string camera_type);
    CameraModuleInfo GetDefaultProfile(int camera_id);
    bool SetDefaultProfile(int camera_id,CameraModuleInfo moduleInfo);
    bool InitDefaultProfile();

    bool SaveProfileAsDefault(std::vector<CameraModuleInfo> moduleInfo);
    const int SETTING_ALL_CAMERA_ID = -1;

    
};
//CameraModuleSetting* CameraModuleSetting::instance = nullptr;



#endif
