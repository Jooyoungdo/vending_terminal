//
// Created by changseok on 20. 6. 12..
//

#ifndef DAEMON_PROCESS_CAMERA_H
#include <iostream>
//#include <opencv2/opencv.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <iterator>
#include <chrono>
#include <ctime>
#include <experimental/filesystem>
#include <regex>

#include "module.h"

#include "logger.h"
#include "debug.h"
#define DAEMON_PROCESS_CAMERA_H

class camera {
private:
    std::vector <cv::VideoCapture> camera_capture; // list of activated video capture devices
    
    std::vector <cv::Mat> images; // list of latest images from camera
    CameraModuleSetting* cameraModuleSetting;
    logger log = logger("CAMERA"); //logger object for print log
public:
    camera(); //defualt initializer, deprecated
    camera(std::string mode);
    camera(int* camera_index, int num); // initializer with number of camera device and it's index. registrate all camera devices
    
    bool grab_frame(); // grab image from camera devices
    std::vector<cv::Mat> get_frame(); // return latest taken images from camera
    int get_image_count();
    bool save_frame(std::string PATH); //save image in PATH

    bool set_module_profile(std::string jsonData);
    bool update_module_profile();
};


#endif //DAEMON_PROCESS_CAMERA_H
