
#ifndef VENDOR_STORAGE
#define VENDOR_STORAGE
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <vector>


static const std::string DEVICE_NAME_DEEPTHINK = "DT";
static const std::string DEVICE_NAME_FIREFLY = "FF";


class VendorStorage{
public:
    VendorStorage();
    ~VendorStorage();

    std::string GetDeviceName();
    std::string GetSerialNumber();
    std::string GetBrokerIP();
    std::string GetDeviceID();
    void SetDeviceName(const std::string device_name);
    void SetSerialNumber(const std::string serial_number);
    void SetBrokerIP(const std::string broker_ip);
    void SetDeviceID(const std::string device_id);
    void SetProjectName(const std::string project_name);
    void SetHwRev(const std::string hw_rev);    
    bool ParseDeviceInfo();
    
private:
    std::string device_name_;
    std::string serial_number_;
    std::string broker_ip_;
    std::string device_id_;
    std::string hw_rev_;
    std::string project_name_;
    int VendorRead(int vendor_id, char *data, int size);
    int VendorWrite(int vendor_id, const char *data, int size);

        
	// bool ReadSerialNumber();
	// bool ReadDeviceName();
	// bool ReadBrokerIp();
	// bool ReadDeviceId();


};
#endif