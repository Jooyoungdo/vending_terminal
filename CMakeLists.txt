cmake_minimum_required(VERSION 3.13)
project(daemon_process)

set(CMAKE_CXX_STANDARD 17)
set(PAHO_CPP_LIB paho-mqttpp3)
set(PAHO_C_LIB paho-mqtt3a)

find_package( OpenCV REQUIRED )
add_executable(daemon_process main.cpp doorLock.cpp doorLock.h camera.cpp camera.h terminal.cpp terminal.h logger.cpp logger.h module.cpp module.h)
target_link_libraries( daemon_process ${OpenCV_LIBS} ${PAHO_C_LIB} ${PAHO_CPP_LIB} pthread curl mysqlclient stdc++fs)

add_definitions(-DOPENSSL)

message("-- Opencv version ${OpenCV_VERSION}")
if(OpenCV_VERSION VERSION_LESS "4")
    message("-- Add OLD_OPENCV flag")
    add_definitions(-DOLD_OPENCV)
endif()
